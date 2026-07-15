#!/usr/bin/env python3

# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import glob
import os
import re
import sys
import time
from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr

from ament_cpplint import cpplint
from ament_cpplint.cpplint import _cpplint_state
from ament_cpplint.cpplint import ParseArguments
from ament_cpplint.cpplint import ProcessFile


# use custom header guard with two underscore between the name parts
def custom_get_header_guard_cpp_variable(filename):
    from ament_cpplint.cpplint import _root
    from ament_cpplint.cpplint import FileInfo
    # Restores original filename in case that cpplint is invoked from Emacs's
    # flymake.
    filename = re.sub(r'_flymake\.h$', '.h', filename)
    filename = re.sub(r'/\.flymake/([^/]*)$', r'/\1', filename)
    # Replace 'c++' with 'cpp'.
    filename = filename.replace('C++', 'cpp').replace('c++', 'cpp')

    fileinfo = FileInfo(filename)
    file_path_from_root = fileinfo.RepositoryName()
    if _root:
        prefix = _root + os.sep
        # use consistent separator on Windows
        if os.sep != '/':
            prefix = prefix.replace(os.sep, '/')
        if file_path_from_root.startswith(prefix):
            file_path_from_root = file_path_from_root[len(prefix):]
        else:
            filename = filename.replace(os.sep, '/')
            if filename.startswith(prefix):
                file_path_from_root = filename[len(prefix):]
    # use double separator
    file_path_from_root = file_path_from_root.replace('/', '//')
    return re.sub(r'[^a-zA-Z0-9]', '_', file_path_from_root).upper() + '_'


cpplint.GetHeaderGuardCPPVariable = custom_get_header_guard_cpp_variable


def main(argv=sys.argv[1:]):
    extensions = ['c', 'cc', 'cpp', 'cxx']
    headers = ['h', 'hh', 'hpp', 'hxx']

    parser = argparse.ArgumentParser(
        description='Check code against the Google style conventions using '
                    'cpplint.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        '--filters', metavar='FILTER,FILTER,...', type=str,
        help='A comma separated list of category filters to apply')
    parser.add_argument(
        '--linelength', metavar='N', type=int, default=100,
        help='The maximum line length')
    parser.add_argument(
        '--root', type=str,
        help='The --root option for cpplint')
    parser.add_argument(
        '--exclude', default=[],
        nargs='*',
        help='Exclude C/C++ files from being checked.')
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='The files or directories to check. For directories files ending '
             'in %s will be considered.' %
             ', '.join(["'.%s'" % e for e in extensions + headers]))
    # not using a file handle directly
    # in order to prevent leaving an empty file when something fails early
    parser.add_argument(
        '--xunit-file',
        help='Generate a xunit compliant XML file')
    args = parser.parse_args(argv)

    if args.xunit_file:
        start_time = time.time()

    argv = []
    # collect category based counts
    argv.append('--counting=detailed')
    argv.append('--extensions=%s' % ','.join(extensions))
    argv.append('--headers=%s' % ','.join(headers))
    filters = [
        # we do allow C++11
        '-build/c++11',
        # we consider passing non-const references to be ok
        '-runtime/references',
        # we wrap open curly braces for namespaces, classes and functions
        '-whitespace/braces',
        # we don't indent keywords like public, protected and private with one space
        '-whitespace/indent',
        # we allow closing parenthesis to be on the next line
        '-whitespace/parens',
        # we allow the developer to decide about whitespace after a semicolon
        '-whitespace/semicolon',
    ]
    if args.filters:
        filters += args.filters.split(',')
    argv.append('--filter=%s' % ','.join(filters))

    argv.append('--linelength=%d' % args.linelength)

    groups = get_file_groups(args.paths, extensions + headers, args.exclude)
    if not groups:
        print('No files found', file=sys.stderr)
        return 1

    # hook into error reporting
    DefaultError = cpplint.Error  # noqa: N806
    report = []

    # invoke cpplint for each root group of files
    _cpplint_state.ResetErrorCounts()
    for root in sorted(groups.keys()):
        files = groups[root]

        arguments = list(argv)
        if args.root:
            root = os.path.abspath(args.root)
        if root:
            root_arg = '--root=%s' % root
            arguments.append(root_arg)
            print("Using '%s' argument" % root_arg)
        else:
            print("Not using '--root'")
        print('')

        arguments += files
        filenames = ParseArguments(arguments)

        for filename in filenames:
            # hook into error reporting
            errors = []

            def custom_error(filename, linenum, category, confidence, message):
                if cpplint._ShouldPrintError(category, confidence, linenum):
                    errors.append({
                        'linenum': linenum,
                        'category': category,
                        'confidence': confidence,
                        'message': message,
                    })
                DefaultError(filename, linenum, category, confidence, message)
            cpplint.Error = custom_error

            ProcessFile(filename, _cpplint_state.verbose_level)
            report.append((filename, errors))
            print('')

    # output summary
    for category in sorted(_cpplint_state.errors_by_category.keys()):
        count = _cpplint_state.errors_by_category[category]
        print("Category '%s' errors found: %d" % (category, count),
              file=sys.stderr)
    if _cpplint_state.error_count:
        print('Total errors found: %d' % _cpplint_state.error_count,
              file=sys.stderr)
    else:
        print('No problems found')

    # generate xunit file
    if args.xunit_file:
        folder_name = os.path.basename(os.path.dirname(args.xunit_file))
        file_name = os.path.basename(args.xunit_file)
        suffix = '.xml'
        if file_name.endswith(suffix):
            file_name = file_name[0:-len(suffix)]
            suffix = '.xunit'
            if file_name.endswith(suffix):
                file_name = file_name[0:-len(suffix)]
        testname = '%s.%s' % (folder_name, file_name)

        xml = get_xunit_content(report, testname, time.time() - start_time)
        path = os.path.dirname(os.path.abspath(args.xunit_file))
        if not os.path.exists(path):
            os.makedirs(path)
        with open(args.xunit_file, 'w') as f:
            f.write(xml)

    return 1 if _cpplint_state.error_count else 0


def get_file_groups(paths, extensions, exclude_patterns):
    excludes = []
    for exclude_pattern in exclude_patterns:
        excludes.extend(glob.glob(exclude_pattern))
    excludes = {os.path.realpath(x) for x in excludes}

    # dict mapping root path to files
    groups = {}
    for path in paths:
        if os.path.isdir(path):
            for dirpath, dirnames, filenames in os.walk(path):
                if 'AMENT_IGNORE' in dirnames + filenames:
                    dirnames[:] = []
                    continue
                # ignore folder starting with . or _
                dirnames[:] = [d for d in dirnames if d[0] not in ['.', '_']]
                dirnames.sort()

                # select files by extension
                for filename in sorted(filenames):
                    _, ext = os.path.splitext(filename)
                    if ext in ('.%s' % e for e in extensions):
                        filepath = os.path.join(dirpath, filename)
                        if os.path.realpath(filepath) not in excludes:
                            append_file_to_group(groups, filepath)

        if os.path.isfile(path):
            if os.path.realpath(path) not in excludes:
                append_file_to_group(groups, path)

    return groups


def append_file_to_group(groups, path):
    path = os.path.abspath(path)

    root = ''

    # try to determine root from path
    base_path = os.path.dirname(path)
    # find longest subpath which ends with one of the following subfolder names
    subfolder_names = ['include', 'src', 'test']
    matches = [
        re.search(
            '^(.+%s%s)%s' %
            (re.escape(os.sep), re.escape(subfolder_name), re.escape(os.sep)), path)
        for subfolder_name in subfolder_names]
    match_groups = [match.group(1) for match in matches if match]
    if match_groups:
        match_groups = [{'group_len': len(x), 'group': x} for x in match_groups]
        sorted_groups = sorted(match_groups, key=lambda k: k['group_len'])
        base_path = sorted_groups[-1]['group']
        root = base_path

    # try to find repository root
    repo_root = None
    p = path
    while p and repo_root is None:
        # abort if root is reached
        if os.path.dirname(p) == p:
            break
        p = os.path.dirname(p)
        for marker in ['.git', '.hg', '.svn']:
            if os.path.exists(os.path.join(p, marker)):
                repo_root = p
                break

    # compute relative --root argument
    if repo_root and repo_root > base_path:
        root = os.path.relpath(base_path, repo_root)

    # add the path to the appropriate group
    if root not in groups:
        groups[root] = []
    groups[root].append(path)


def get_xunit_content(report, testname, elapsed):
    test_count = sum(max(len(r[1]), 1) for r in report)
    error_count = sum(len(r[1]) for r in report)
    data = {
        'testname': testname,
        'test_count': test_count,
        'error_count': error_count,
        'time': '%.3f' % round(elapsed, 3),
    }
    xml = """<?xml version="1.0" encoding="UTF-8"?>
<testsuite
  name="%(testname)s"
  tests="%(test_count)d"
  failures="%(error_count)d"
  errors="0"
  time="%(time)s"
>
""" % data

    for (filename, errors) in report:

        if errors:
            # report each cpplint error as a failing testcase
            for error in errors:
                data = {
                    'quoted_name': quoteattr(
                        '%s [%s] (%s:%d)' % (
                            error['category'], error['confidence'],
                            filename, error['linenum'])),
                    'testname': testname,
                    'quoted_message': quoteattr(error['message']),
                }
                xml += """  <testcase
    name=%(quoted_name)s
    classname="%(testname)s"
  >
      <failure message=%(quoted_message)s/>
  </testcase>
""" % data

        else:
            # if there are no cpplint errors report a single successful test
            data = {
                'quoted_location': quoteattr(filename),
                'testname': testname,
            }
            xml += """  <testcase
    name=%(quoted_location)s
    classname="%(testname)s"/>
""" % data

    # output list of checked files
    data = {
        'escaped_files': escape(''.join(['\n* %s' % r[0] for r in report])),
    }
    xml += """  <system-out>Checked files:%(escaped_files)s</system-out>
""" % data

    xml += '</testsuite>\n'
    return xml


if __name__ == '__main__':
    sys.exit(main())
