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

from pyflakes.api import checkPath
from pyflakes.messages import Message
from pyflakes.reporter import Reporter


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description='Check code using pyflakes.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='The files or directories to check. For directories files ending '
             "in '.py' will be considered.")
    parser.add_argument(
        '--exclude',
        metavar='filename',
        nargs='*',
        dest='excludes',
        help='The filenames to exclude.')
    # not using a file handle directly
    # in order to prevent leaving an empty file when something fails early
    parser.add_argument(
        '--xunit-file',
        help='Generate a xunit compliant XML file')
    args = parser.parse_args(argv)

    if args.xunit_file:
        start_time = time.time()

    filenames = get_files(args.paths)
    if args.excludes:
        filenames = [f for f in filenames if os.path.basename(f) not in args.excludes]
    if not filenames:
        print('No files found', file=sys.stderr)
        return 1

    report = []

    # invoke pyflakes for each file
    for filename in filenames:
        reporter = CustomReporter()
        print(filename)
        checkPath(filename, reporter=reporter)
        for error in reporter.errors:
            try:
                print(error, file=sys.stderr)
            except TypeError:
                # this can happen if the line contains percent characters
                print(error.__dict__, file=sys.stderr)
        report.append((filename, reporter.errors))
        print('')

    # output summary
    error_count = sum(len(r[1]) for r in report)
    if not error_count:
        print('No problems found')
        rc = 0
    else:
        print('%d errors' % error_count, file=sys.stderr)
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

                # select files by extension
                for filename in sorted(filenames):
                    if filename.endswith('.py'):
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
            # report each pyflakes error as a failing testcase
            for error in errors:
                try:
                    msg = error.message % error.message_args
                except TypeError:
                    # this can happen if the line contains percent characters
                    msg = error.message + ' ' + str(error.message_args)
                data = {
                    'quoted_name': quoteattr(
                        '%s (%s:%d)' % (
                            type(error).__name__, filename, error.lineno)),
                    'testname': testname,
                    'quoted_message': quoteattr(msg),
                }
                xml += """  <testcase
    name=%(quoted_name)s
    classname="%(testname)s"
  >
      <failure message=%(quoted_message)s/>
  </testcase>
""" % data

        else:
            # if there are no pyflakes errors report a single successful test
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


class CustomReporter(Reporter):

    def __init__(self):
        super(CustomReporter, self).__init__(sys.stdout, sys.stderr)
        self.errors = []

    def unexpectedError(self, filename, msg):  # noqa: N802
        self.errors.append(UnexpectedError(filename, msg))

    def syntaxError(self, filename, msg, lineno, offset, text):  # noqa: N802
        self.errors.append(SyntaxError(filename, msg, lineno, offset, text))

    def flake(self, message):
        self.errors.append(message)


class Location:

    def __init__(self, lineno, col_offset=None):
        self.lineno = lineno
        if col_offset is not None:
            self.col_offset = col_offset


class SyntaxError(Message):
    message = 'syntax error %r'

    def __init__(self, filename, msg, lineno, offset, text):
        loc = Location(lineno, col_offset=offset)
        super(SyntaxError, self).__init__(filename, loc)
        self.message_args = (msg, text,)


class UnexpectedError(Message):
    message = 'unexpected error %r'

    def __init__(self, filename, msg):
        loc = Location(0)
        super(UnexpectedError, self).__init__(filename, loc)
        self.message_args = (msg,)


if __name__ == '__main__':
    sys.exit(main())
