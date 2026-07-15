#!/usr/bin/env python3

# Copyright 2014-2016 Open Source Robotics Foundation, Inc.
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
import os
import subprocess
import sys
import time
from xml.etree import ElementTree
from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr

import yaml


def main(argv=sys.argv[1:]):
    config_file = os.path.join(
        os.path.dirname(__file__), 'configuration', '.clang-format')
    extensions = ['c', 'cc', 'cpp', 'cxx', 'h', 'hh', 'hpp', 'hxx']

    parser = argparse.ArgumentParser(
        description='Check code style using clang_format.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        '--config',
        metavar='path',
        default=config_file,
        dest='config_file',
        help='The config file')
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='The files or directories to check. For directories files ending '
             'in %s will be considered.' %
             ', '.join(["'.%s'" % e for e in extensions]))
    parser.add_argument(
        '--clang-format-version',
        help='The version of clang-format to use.')
    parser.add_argument(
        '--reformat',
        action='store_true',
        help='Reformat the files in place')
    # not using a file handle directly
    # in order to prevent leaving an empty file when something fails early
    parser.add_argument(
        '--xunit-file',
        help='Generate a xunit compliant XML file')
    args = parser.parse_args(argv)

    if not os.path.exists(args.config_file):
        print("Could not find config file '%s'" % args.config_file,
              file=sys.stderr)
        return 1

    if args.xunit_file:
        start_time = time.time()

    files = get_files(args.paths, extensions)
    if not files:
        print('No files found', file=sys.stderr)
        return 1

    bin_names = [
        'clang-format',
        'clang-format-3.8',
        'clang-format-3.7',
        'clang-format-3.6',
        'clang-format-3.5',
        'clang-format-3.4',
        'clang-format-3.3',
    ]

    if args.clang_format_version:
        bin_names = ['clang-format-' + args.clang_format_version]

    clang_format_bin = find_executable(bin_names)
    if not clang_format_bin:
        print('Could not find %s executable' %
              ' / '.join(["'%s'" % n for n in bin_names]), file=sys.stderr)
        return 1

    report = []

    # invoke clang_format
    with open(args.config_file, 'r') as h:
        content = h.read()
    data = yaml.safe_load(content)
    style = yaml.dump(data, default_flow_style=True, width=float('inf'))
    cmd = [clang_format_bin,
           '-output-replacements-xml',
           '-style=%s' % style]
    cmd.extend(files)
    try:
        output = subprocess.check_output(cmd)
    except subprocess.CalledProcessError as e:
        print("The invocation of '%s' failed with error code %d: %s" %
              (os.path.basename(clang_format_bin), e.returncode, e),
              file=sys.stderr)
        return 1

    # output errors
    report = {}
    for filename in files:
        report[filename] = []

    xmls = output.split(b"<?xml version='1.0'?>")[1:]
    changed_files = []
    for filename, xml in zip(files, xmls):
        try:
            root = ElementTree.fromstring(xml)
        except ElementTree.ParseError as e:
            print('Invalid XML in clang format output: %s' % str(e),
                  file=sys.stderr)
            return 1

        replacements = root.findall('replacement')
        if replacements:
            changed_files.append(filename)
            if not args.reformat:
                print("Code style divergence in file '%s':" % filename,
                      file=sys.stderr)
                print('', file=sys.stderr)
            else:
                print("%d code style divergences in file '%s': reformatted "
                      'file' % (len(replacements), filename))
            with open(filename, 'r') as h:
                content = h.read()
            for replacement in replacements:
                data = {
                    'offset': int(replacement.get('offset')),
                    'length': int(replacement.get('length')),
                    'replacement': replacement.text or '',
                }
                # to-be-replaced snippet
                data['original'] = content[
                    data['offset']:data['offset'] + data['length']]
                # map global offset to line number and offset in line
                index_of_line_start = find_index_of_line_start(
                    content, data['offset'])
                index_of_line_end = find_index_of_line_end(
                    content, data['offset'] + data['length'])
                data['line_no'] = get_line_number(content, index_of_line_start)
                data['offset_in_line'] = data['offset'] - index_of_line_start

                # generate diff like changes
                subcontent = content[
                    index_of_line_start:index_of_line_end]
                data['deletion'] = subcontent
                data['addition'] = subcontent[0:data['offset_in_line']] + \
                    data['replacement'] + \
                    subcontent[data['offset_in_line'] + data['length']:]

                # make common control characters visible
                mapping = {'\n': '\\n', '\r': '\\r', '\t': '\\t'}
                for old, new in mapping.items():
                    data['replacement'] = data['replacement'].replace(old, new)
                    data['original'] = data['original'].replace(old, new)

                mapping = {'\r': '\n'}
                for old, new in mapping.items():
                    data['deletion'] = data['deletion'].replace(old, new)
                    data['addition'] = data['addition'].replace(old, new)

                # format deletion / addition as unified diff
                data['deletion'] = '\n'.join(
                    ['- ' + line for line in data['deletion'].split('\n')])
                data['addition'] = '\n'.join(
                    ['+ ' + line for line in data['addition'].split('\n')])

                report[filename].append(data)

                if not args.reformat:
                    data = dict(data)
                    data['filename'] = filename
                    print('[%(filename)s:%(line_no)d:%(offset_in_line)d]: '
                          'Replace [%(original)s] with [%(replacement)s]' %
                          data, file=sys.stderr)
                    print(data['deletion'], file=sys.stderr)
                    print(data['addition'], file=sys.stderr)
            if not args.reformat:
                print('', file=sys.stderr)
        else:
            print("No code style divergence in file '%s'" % filename)
            if not args.reformat:
                print('')

    # overwrite original with reformatted files
    if args.reformat and changed_files:
        cmd = [clang_format_bin,
               '-i',
               '-style=%s' % style]
        cmd.extend(files)
        try:
            subprocess.check_call(cmd)
        except subprocess.CalledProcessError as e:
            print("The invocation of '%s' failed with error code %d: %s" %
                  (os.path.basename(clang_format_bin), e.returncode, e),
                  file=sys.stderr)
            return 1

    # output summary
    file_count = sum(1 if report[k] else 0 for k in report.keys())
    replacement_count = sum(len(r) for r in report.values())
    if not file_count:
        print('No problems found')
        rc = 0
    else:
        print('%d files with %d code style divergences' %
              (file_count, replacement_count), file=sys.stderr)
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


def find_executable(file_names):
    paths = os.getenv('PATH').split(os.path.pathsep)
    for file_name in file_names:
        for path in paths:
            file_path = os.path.join(path, file_name)
            if os.path.isfile(file_path) and os.access(file_path, os.X_OK):
                return file_path
    return None


def get_files(paths, extensions):
    files = []
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
                        files.append(os.path.join(dirpath, filename))
        if os.path.isfile(path):
            files.append(path)
    return [os.path.normpath(f) for f in files]


def find_index_of_line_start(data, offset):
    index_1 = data.rfind('\n', 0, offset) + 1
    index_2 = data.rfind('\r', 0, offset) + 1
    return max(index_1, index_2)


def find_index_of_line_end(data, offset):
    index_1 = data.find('\n', offset)
    if index_1 == -1:
        index_1 = len(data)
    index_2 = data.find('\r', offset)
    if index_2 == -1:
        index_2 = len(data)
    return min(index_1, index_2)


def get_line_number(data, offset):
    return data[0:offset].count('\n') + data[0:offset].count('\r') + 1


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
        replacements = report[filename]

        if replacements:
            # report each replacement as a failing testcase
            for replacement in replacements:
                data = {
                    'quoted_location': quoteattr(
                        '%s:%d:%d' % (
                            filename, replacement['line_no'],
                            replacement['offset_in_line'])),
                    'testname': testname,
                    'quoted_message': quoteattr(
                        'Replace [%s] with [%s]' %
                        (replacement['original'], replacement['replacement'])),
                    'cdata': '\n'.join([
                        '%s:%d:%d' % (
                            filename, replacement['line_no'],
                            replacement['offset_in_line']),
                        replacement['deletion'],
                        replacement['addition'],
                    ]),
                }
                xml += """  <testcase
    name=%(quoted_location)s
    classname="%(testname)s"
  >
      <failure message=%(quoted_message)s><![CDATA[%(cdata)s]]></failure>
  </testcase>
""" % data

        else:
            # if there are no replacements report a single successful test
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
