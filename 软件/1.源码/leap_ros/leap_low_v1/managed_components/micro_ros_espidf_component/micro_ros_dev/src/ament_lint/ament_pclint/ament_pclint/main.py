#!/usr/bin/env python3

# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
# Copyright 2017-2018 Apex.AI, Inc.
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
import concurrent.futures
import multiprocessing
import os
from shutil import which
import subprocess
import sys
import time
from xml.etree import ElementTree
from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr


def main(argv=sys.argv[1:]):
    extensions = ['c', 'cc', 'cpp', 'cxx', 'c++']

    parser = argparse.ArgumentParser(
        description='Perform static code analysis using PC-lint.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='Files and/or directories to be checked. Directories are searched recursively for '
             'files ending in one of %s.' %
             ', '.join(["'.%s'" % e for e in extensions]))
    parser.add_argument(
        '--language',
        choices=['c', 'cpp'],
        help='Force pclint to analyze files as specified language. '
             '(default: Automatically detected)')
    parser.add_argument(
        '--pclint-config-file',
        help='Specify a custom pclint configuration file. '
             '(default: config/gcc/{co-gcc|co-g++}.lnt on Linux, '
             'config/gcc/{co-osx-gcc|co-osx-g++}.lnt on OSX, '
             'config/msvc/{co-cl|co-cl++}.lnt on Windows.)')
    parser.add_argument(
        '--include-directories',
        nargs='*',
        help='Paths to directories that are included in the files that should be analyzed.\n'
             'Paths in AMENT_PREFIX_PATH will be included by default.\n'
             'Use --exclude-ament-prefix-path to exclude them.')
    parser.add_argument(
        '--exclude-ament-prefix-path',
        action='store_true',
        help='Include folders in AMENT_PREFIX_PATH will be included by default, use this option'
             'to exclude them.')
    parser.add_argument(
        '--compiler-definitions',
        nargs='*',
        default='',
        help='Compiler definitions that are necessary for PCLint.')
    # not using a file handle directly
    # in order to prevent leaving an empty file when something fails early
    parser.add_argument(
        '--xunit-file',
        help='Generate a xunit compliant XML file')
    parser.add_argument(
        '--debug',
        action='store_true',
        help='Print the full pclint command that is being called'
    )
    args = parser.parse_args(argv)

    if args.xunit_file:
        start_time = time.time()

    files = get_files(args.paths, extensions)
    if not files:
        print('No files found', file=sys.stderr)
        return 1

    exec_name = 'pclp64'  # Name of Windows executable, other OSes have a suffix
    if sys.platform == 'linux':
        exec_name += '_linux'
    elif sys.platform == 'darwin':
        exec_name += '_osx'
    pclint_bin = find_executable(exec_name)
    if not pclint_bin:
        print("Could not find pclint executable '{}'".format(exec_name), file=sys.stderr)
        return 1

    if args.pclint_config_file:
        pclint_config_dir = os.path.dirname(args.pclint_config_file)
    elif os.name == 'nt':
        pclint_config_dir = \
            os.path.join(os.path.dirname(os.path.realpath(__file__)), 'config', 'msvc')
    else:
        pclint_config_dir = \
            os.path.join(os.path.dirname(os.path.realpath(__file__)), 'config', 'gcc')

    # Prepare pclint command
    base_cmd = [pclint_bin,
                '-i"%s"' % pclint_config_dir,
                '-i"%s"' % os.path.join(os.path.dirname(os.path.realpath(__file__)), 'config'),
                'env-xml.lnt']

    # Add include directories from arguments
    for directory in (args.include_directories or []):
        base_cmd.extend(['-i"%s"' % directory])

    # Add include folders in AMENT_PREFIX_PATH
    if not args.exclude_ament_prefix_path and os.environ['AMENT_PREFIX_PATH']:
        if os.name == 'nt':
            paths = os.environ['AMENT_PREFIX_PATH'].split(';')
        else:
            paths = os.environ['AMENT_PREFIX_PATH'].split(':')
        for path in paths:
            include_folder = os.path.join(path, 'include')
            if os.path.exists(include_folder):
                base_cmd.extend(['-i"{}"'.format(include_folder)])

    # Add compiler definitions from arguments
    for definition in (args.compiler_definitions or []):
        base_cmd.extend(['-d"%s"' % definition])

    # try to determine the number of CPU cores
    jobs = 1
    try:
        jobs = multiprocessing.cpu_count()
    except NotImplementedError:
        # the number of cores cannot be determined, don't parallelize
        pass

    result = [''] * len(files)
    idx = 0
    threads = []
    retvals = [0] * len(files)
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as executor:
        for f in files:
            retvals[idx] = 0
            cmd = base_cmd[:]
            if args.pclint_config_file:
                pclint_config_file = os.path.basename(args.pclint_config_file)
            else:
                pclint_config_file = get_lnt_file(f, args.language)

            cmd.extend([pclint_config_file])

            # Add file at the end
            cmd.append(f)

            # For debug: prints full command to be copied and pasted.
            if args.debug:
                print(str(cmd).replace("'", '').replace(',', '').replace('[', '').replace(']', ''))

            # We start one thread per file
            threads.append(executor.submit(execute_analysis, cmd, result, retvals, idx))
            idx += 1

    concurrent.futures.wait(threads)

    aggregate_xml = _format_pclint_xml_out(result)

    try:
        root = ElementTree.fromstring(aggregate_xml)
    except ElementTree.ParseError as e:
        print('Invalid XML in pclint output: %s' % str(e),
              file=sys.stderr)
        return 1

    # output errors
    report = {}
    for doc in root.findall('doc'):
        for error in doc.findall('error'):
            report[error.find('file').text] = []
        for error in doc.findall('error'):
            filename = error.find('file').text
            data = {
                'line': int(error.find('line').text),
                'id': error.find('code').text,
                'severity': error.find('type').text,
                'msg': error.find('desc').text,
            }
            report[filename].append(data)

            data = dict(data)
            data['filename'] = filename
            print('[%(filename)s:%(line)d]: (%(severity)s: %(id)s) %(msg)s' % data,
                  file=sys.stderr)

    # output summary
    error_count = sum(len(r) for r in report.values())
    if not error_count:
        print('No problems found')
        rc = 0
    else:
        print('%d errors' % error_count, file=sys.stderr)
        if args.debug:
            for r in result:
                print(r)
        rc = 1

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

    return rc


def execute_analysis(cmd, result, retvals, idx):
    try:
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        xml = p.communicate()[0].decode('utf-8')  # Read xml from stdout
    except subprocess.CalledProcessError as e:
        print("The invocation of 'pclint' failed with error code %d: %s" %
              (e.returncode, e), file=sys.stderr)
        return False
    result[idx] = xml
    retvals[idx] = p.returncode


def find_executable(file_name, additional_paths=None):
    path = None
    if additional_paths:
        path = os.getenv('PATH', os.defpath)
        path += os.path.pathsep + os.path.pathsep.join(additional_paths)
    return which(file_name, path=path)


def _format_pclint_xml_out(xml_arr):
    aggregate_xml = \
'<?xml version="1.0" encoding="UTF-8"?>\n\
<docs> \n'  # noqa: E122

    for xml in xml_arr:
        aggregate_xml += xml

    aggregate_xml += '</docs>'
    return aggregate_xml


def get_lnt_file(filename, args_language):
    if os.name == 'nt':
        cc_config = 'co-cl.lnt'
        cpp_config = 'co-cl++.lnt'
    elif sys.platform == 'darwin':
        cc_config = 'co-osx-gcc.lnt'
        cpp_config = 'co-osx-g++.lnt'
    else:
        cc_config = 'co-gcc.lnt'
        cpp_config = 'co-g++.lnt'

    if (filename.endswith('.cpp') or filename.endswith('.c++') or filename.endswith('.cxx')):
        pclint_config_file = cpp_config
    else:
        pclint_config_file = cc_config

    # Allow user to force cpp files to be analyzed as c files.
    # and c files to be analyzed as cpp files.
    if args_language == 'c':
        pclint_config_file = cc_config
    elif args_language == 'cpp':
        pclint_config_file = cpp_config
    return pclint_config_file


def get_files(paths, extensions):
    files = []
    for path in paths:
        if os.path.isdir(path):
            for dirpath, dirnames, filenames in os.walk(path):
                if 'AMENT_IGNORE' in dirnames + filenames:
                    dirnames[:] = []
                    continue
                # ignore folder starting with . or _
                dirnames[:] = [d for d in dirnames if d[0] not in ['.', '_'] and d not in ['test']]
                dirnames.sort()

                # select files by extension
                for filename in sorted(filenames):
                    _, ext = os.path.splitext(filename)
                    if ext in ('.%s' % e for e in extensions):
                        files.append(os.path.join(dirpath, filename))
        if os.path.isfile(path):
            files.append(path)
    return [str(os.path.normpath(f)) for f in files]


def get_xunit_content(report, testname, elapsed):
    test_count = sum(max(len(r), 1) for r in report.values())
    error_count = sum(len(r) for r in report.values())
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
  errors="0"
  failures="%(error_count)d"
  time="%(time)s"
>
""" % data

    for filename in sorted(report.keys()):
        errors = report[filename]

        if errors:
            # report each pclint error as a failing testcase
            for error in errors:
                data = {
                    'quoted_name': quoteattr(
                        '%s: %s (%s:%d)' % (
                            error['severity'], error['id'],
                            filename, error['line'])),
                    'testname': testname,
                    'quoted_message': quoteattr(error['msg']),
                }
                xml += """  <testcase
    name=%(quoted_name)s
    classname="%(testname)s"
  >
      <failure message=%(quoted_message)s/>
  </testcase>
""" % data

        else:
            # if there are no pclint errors report a single successful test
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
        'escaped_files': escape(''.join(['\n* %s' % r
                                         for r in sorted(report.keys())])),
    }
    xml += """  <system-out>Checked files:%(escaped_files)s</system-out>
""" % data

    xml += '</testsuite>\n'
    return xml


if __name__ == '__main__':
    sys.exit(main())
