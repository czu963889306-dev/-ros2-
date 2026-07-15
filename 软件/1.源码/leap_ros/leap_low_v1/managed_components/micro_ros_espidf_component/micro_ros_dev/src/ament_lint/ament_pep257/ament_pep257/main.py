#!/usr/bin/env python3

# Copyright 2015 Open Source Robotics Foundation, Inc.
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
import logging
import os
import sys
import time
from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr

import pydocstyle
from pydocstyle import check

try:  # as of version 1.1.0
    from pydocstyle.config import ConfigurationParser
    from pydocstyle.violations import Error
    from pydocstyle.utils import log
except ImportError:  # try version 1.0.0
    from pydocstyle import ConfigurationParser
    from pydocstyle import Error
    from pydocstyle import log

log.setLevel(logging.INFO)


_conventions = set(pydocstyle.conventions.keys())
_conventions.add('ament')

_ament_ignore = [
    'D100',
    'D101',
    'D102',
    'D103',
    'D104',
    'D105',
    'D106',
    'D107',
    'D203',
    'D212',
    'D404',
]


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description='Check docstrings against the style conventions in PEP 257.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    err_code_group = parser.add_mutually_exclusive_group()
    err_code_group.add_argument(
        '--ignore',
        nargs='+',
        default=[],
        help='Choose the list of error codes for pydocstyle NOT to check for.')
    err_code_group.add_argument(
        '--select',
        nargs='+',
        default=[],
        help='Choose the basic list of error codes for pydocstyle to check for.'
    )
    err_code_group.add_argument(
        '--convention',
        choices=_conventions,
        default='ament',
        help=(
            f'Choose a preset list of error codes. Valid options are {_conventions}.'
            f'The "ament" convention is defined as --ignore {_ament_ignore}.'
        ),
    )
    parser.add_argument(
        '--add-ignore',
        nargs='+',
        default=[],
        help='Ignore an extra error code, removing it from the list set by --(select/ignore)')
    parser.add_argument(
        '--add-select',
        nargs='+',
        default=[],
        help='Check an extra error code, adding it to the list set by --(select/ignore).'
    )
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='The files or directories to check. For directories, files ending '
             "in '.py' will be considered.")
    parser.add_argument(
        '--exclude',
        metavar='filename',
        nargs='*',
        default=[],
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

    args.ignore = ','.join(args.ignore)
    args.select = ','.join(args.select)
    args.add_select = ','.join(args.add_select)
    args.add_ignore = ','.join(args.add_ignore)
    if not (args.ignore or args.select) and args.convention == 'ament':
        args.ignore = ','.join(_ament_ignore)

    excludes = [os.path.abspath(e) for e in args.excludes]
    report = generate_pep257_report(args.paths, excludes, args.ignore, args.select,
                                    args.convention, args.add_ignore, args.add_select)
    error_count = sum(len(r[1]) for r in report)

    # print summary
    if not error_count:
        print('No problems found')
        rc = 0
    else:
        print('%d errors' % error_count)
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


def _filename_in_excludes(filename, excludes):
    absname = os.path.abspath(filename)
    return any(os.path.commonpath([absname, e]) == e for e in excludes)


def generate_pep257_report(paths, excludes, ignore, select, convention, add_ignore, add_select):
    conf = ConfigurationParser()
    sys_argv = sys.argv
    sys.argv = [
        'main',
        '--match', r'.*\.py',
        '--match-dir', r'[^\._].*',
    ]
    if ignore:
        sys.argv += ['--ignore', ignore]
    elif select:
        sys.argv += ['--select', select]
    else:
        sys.argv += ['--convention', convention]
    if add_ignore:
        sys.argv += ['--add-ignore', add_ignore]
    if add_select:
        sys.argv += ['--add-select', add_select]
    sys.argv += paths
    conf.parse()
    sys.argv = sys_argv
    files_to_check = conf.get_files_to_check()

    report = []

    files_dict = {}
    # Unpack 3 values for pydocstyle <= 6.1.1 and 4 values for pydocstyle >= 6.2.0
    for filename, checked_codes, ignore_decorators, *_ in files_to_check:
        if _filename_in_excludes(filename, excludes):
            continue
        files_dict[filename] = {
            'select': checked_codes,
            'ignore_decorators': ignore_decorators,
        }

    for filename in sorted(files_dict.keys()):
        print('checking', filename)
        errors = []
        pep257_errors = check(
            [filename],
            **files_dict[filename])
        for pep257_error in pep257_errors:
            if isinstance(pep257_error, Error):
                errors.append({
                    'category': pep257_error.code,
                    'linenumber': pep257_error.line,
                    'message': pep257_error.message,
                })
                print(
                    '%s:%d %s: %s' %
                    (pep257_error.filename, pep257_error.line, pep257_error.definition,
                     pep257_error.message))
            elif isinstance(pep257_error, SyntaxError):
                errors.append({
                    'category': str(type(pep257_error)),
                    'linenumber': '-',
                    'message': 'invalid syntax in file',
                })
                print('%s: invalid syntax' % filename)
            else:
                errors.append({
                    'category': 'unknown',
                    'linenumber': '-',
                    'message': str(pep257_error),
                })
                print('%s: %s' % (filename, pep257_error))
        report.append((filename, errors))
    return report


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
            # report each error as a failing testcase
            for error in errors:
                data = {
                    'quoted_location': quoteattr(
                        '%s (%s:%s)' % (
                            error['category'], filename, str(error['linenumber']))),
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
