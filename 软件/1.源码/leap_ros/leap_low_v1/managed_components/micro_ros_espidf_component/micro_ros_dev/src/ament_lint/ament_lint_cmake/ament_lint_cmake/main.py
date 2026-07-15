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
import os
import sys
import time
from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr

import ament_lint_cmake.cmakelint as cmakelint


# override filename check to allow any filename to be checked
def is_valid_file(filename):
    return True


cmakelint.IsValidFile = is_valid_file


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description='Check CMake code against the style conventions.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='The files or directories to check. For directories files named '
             "'CMakeLists.txt' and ending in '.cmake' or '.cmake.in' will be "
             'considered.')
    parser.add_argument(
        '--filters',
        default='',
        help='Filters for lint_cmake, for a list of filters see: '
             'https://github.com/richq/cmake-lint/blob/master/README.md#usage')
    parser.add_argument(
        '--linelength', metavar='N', type=int, default=140,
        help='The maximum line length')
    # not using a file handle directly
    # in order to prevent leaving an empty file when something fails early
    parser.add_argument(
        '--xunit-file',
        help='Generate a xunit compliant XML file')
    args = parser.parse_args(argv)

    if args.xunit_file:
        start_time = time.time()

    files = get_files(args.paths)
    if not files:
        print('No files found', file=sys.stderr)
        return 1

    # hook into error reporting
    DefaultError = cmakelint.Error  # noqa: N806
    report = []

    # invoke cmake lint
    cmakelint._lint_state.config = cmakelint._DEFAULT_CMAKELINTRC
    cmakelint._lint_state.SetFilters(args.filters)
    if args.linelength is not None:
        cmakelint._lint_state.SetLineLength(str(args.linelength))
    for filename in files:
        # hook into error reporting
        errors = []

        def custom_error(filename, linenumber, category, message):
            if cmakelint.ShouldPrintError(category):
                errors.append({
                    'linenumber': linenumber,
                    'category': category,
                    'message': message,
                })
            DefaultError(filename, linenumber, category, message)
        cmakelint.Error = custom_error

        cmakelint.ProcessFile(filename)
        report.append((filename, errors))
        if errors:
            print('')

    # print summary
    print('')
    if not cmakelint._lint_state.errors:
        print('No problems found')
        rc = 0
    else:
        print('%d errors' % cmakelint._lint_state.errors)
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


def get_files(paths):
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

                # select files by name  / extension
                for filename in sorted(filenames):
                    fname_low = filename.lower()
                    if (
                        fname_low == 'CMakeLists.txt'.lower() or
                        fname_low.endswith('.cmake') or
                        fname_low.endswith('.cmake.in')
                    ):
                        files.append(os.path.join(dirpath, filename))
        if os.path.isfile(path):
            files.append(path)
    return [os.path.normpath(f) for f in files]


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
  errors="0"
  failures="%(error_count)d"
  time="%(time)s"
>
""" % data

    for (filename, errors) in report:

        if errors:
            # report each lint_cmake error as a failing testcase
            for error in errors:
                data = {
                    'quoted_location': quoteattr(
                        '%s (%s:%d)' % (
                            error['category'], filename, error['linenumber'])),
                    'testname': testname,
                    'quoted_message': quoteattr(error['message']),
                }
                xml += """  <testcase
    name=%(quoted_location)s
    classname="%(testname)s"
  >
      <failure message=%(quoted_message)s/>
  </testcase>
""" % data

        else:
            # if there are no lint_cmake errors report a single successful test
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
