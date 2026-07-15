#!/usr/bin/env python3
#
# Copyright 2019 Canonical, Ltd.
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
import re
import sys
import textwrap
import time
from typing import List, Match, Optional, Tuple
from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr

import mypy.api  # type: ignore


def main(argv: List[str] = sys.argv[1:]) -> int:
    """Command line tool for static type analysis with mypy."""
    parser = argparse.ArgumentParser(
        description='Check code using mypy',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        '--config',
        metavar='path',
        dest='config_file',
        default=os.path.join(os.path.dirname(__file__), 'configuration', 'ament_mypy.ini'),
        help='The config file'
    )
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='The files or directories to check. For directories files ending '
             "in '.py' will be considered."
    )
    parser.add_argument(
        '--exclude',
        metavar='filename',
        nargs='*',
        dest='excludes',
        help='The filenames to exclude.'
    )
    parser.add_argument(
        '--cache-dir',
        metavar='cache',
        default=os.devnull,
        dest='cache_dir',
        help='The location mypy will place its cache in. Defaults to system '
             'null device'
    )

    # not using a file handle directly
    # in order to prevent leaving an empty file when something fails early
    parser.add_argument(
        '--xunit-file',
        help='Generate a xunit compliant XML file'
    )
    args = parser.parse_args(argv)

    if args.xunit_file:
        start_time = time.time()

    if args.config_file and not os.path.exists(args.config_file):
        print("Could not find config file '{}'".format(args.config_file), file=sys.stderr)
        return 1

    filenames = _get_files(args.paths)
    if args.excludes:
        filenames = [f for f in filenames
                     if os.path.basename(f) not in args.excludes]
    if not filenames:
        print('No files found', file=sys.stderr)
        return 1

    normal_report, error_messages, exit_code = _generate_mypy_report(
        filenames,
        args.config_file,
        args.cache_dir
    )

    if error_messages:
        print('mypy error encountered', file=sys.stderr)
        print(error_messages, file=sys.stderr)
        print('\nRegular report continues:')
        print(normal_report, file=sys.stderr)
        return exit_code

    errors_parsed = _get_errors(normal_report)

    print('\n{} files checked'.format(len(filenames)))
    if not normal_report:
        print('No errors found')
    else:
        print('{} errors'.format(len(errors_parsed)))

    print(normal_report)

    print('\nChecked files:')
    print(''.join(['\n* {}'.format(f) for f in filenames]))

    # generate xunit file
    if args.xunit_file:
        folder_name = os.path.basename(os.path.dirname(args.xunit_file))
        file_name = os.path.basename(args.xunit_file)
        suffix = '.xml'
        if file_name.endswith(suffix):
            file_name = file_name[:-len(suffix)]
            suffix = '.xunit'
            if file_name.endswith(suffix):
                file_name = file_name[:-len(suffix)]
        testname = '{}.{}'.format(folder_name, file_name)

        xml = _get_xunit_content(errors_parsed, testname, filenames, time.time() - start_time)
        path = os.path.dirname(os.path.abspath(args.xunit_file))
        if not os.path.exists(path):
            os.makedirs(path)
        with open(args.xunit_file, 'w') as f:
            f.write(xml)

    return exit_code


def _generate_mypy_report(paths: List[str],
                          config_file: Optional[str] = None,
                          cache_dir: str = os.devnull) -> Tuple[str, str, int]:
    mypy_argv = []
    mypy_argv.append('--cache-dir')
    mypy_argv.append(str(cache_dir))
    if cache_dir == os.devnull:
        mypy_argv.append('--no-incremental')
    if config_file:
        mypy_argv.append('--config-file')
        mypy_argv.append(str(config_file))
    mypy_argv.append('--show-error-context')
    mypy_argv.append('--show-column-numbers')
    mypy_argv += paths
    res = mypy.api.run(mypy_argv)  # type: Tuple[str, str, int]
    return res


def _get_xunit_content(errors: List[Match],
                       testname: str,
                       filenames: List[str],
                       elapsed: float) -> str:
    xml = textwrap.dedent("""\
        <?xml version="1.0" encoding="UTF-8"?>
        <testsuite
        name="{test_name:s}"
        tests="{test_count:d}"
        errors="0"
        failures="{error_count:d}"
        time="{time:s}"
        >
    """).format(
                test_name=testname,
                test_count=max(len(errors), 1),
                error_count=len(errors),
                time='{:.3f}'.format(round(elapsed, 3))
    )

    if errors:
        # report each mypy error/warning as a failing testcase
        for error in errors:
            pos = ''
            if error.group('lineno'):
                pos += ':' + str(error.group('lineno'))
                if error.group('colno'):
                    pos += ':' + str(error.group('colno'))
            xml += _dedent_to("""\
                <testcase
                    name={quoted_name}
                    classname="{test_name}"
                >
                    <failure message={quoted_message}/>
                </testcase>
                """, '  ').format(
                    quoted_name=quoteattr(
                        '{0[type]} ({0[filename]}'.format(error) + pos + ')'),
                    test_name=testname,
                    quoted_message=quoteattr('{0[msg]}'.format(error) + pos)
                )
    else:
        # if there are no mypy problems report a single successful test
        xml += _dedent_to("""\
            <testcase
              name="mypy"
              classname="{}"/>
            """, '  ').format(testname)

    # output list of checked files
    xml += '  <system-out>Checked files:{escaped_files}\n  </system-out>\n'.format(
        escaped_files=escape(''.join(['\n* %s' % f for f in filenames]))
    )

    xml += '</testsuite>\n'
    return xml


def _get_files(paths: List[str]) -> List[str]:
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
                    elif filename.endswith('.pyi'):
                        type_stub_path = os.path.join(dirpath, filename)
                        files.append(type_stub_path)

                        # removes suffix i
                        regular_file_path = type_stub_path[:-len('i')]
                        # Use type stub over file
                        if regular_file_path in files:
                            files.remove(regular_file_path)
        elif os.path.isfile(path):
            files.append(path)
    return [os.path.normpath(f) for f in files]


def _get_errors(report_string: str) -> List[Match]:
    return list(re.finditer(r'^(?P<filename>([a-zA-Z]:)?([^:])+):((?P<lineno>\d+):)?((?P<colno>\d+):)?\ (?P<type>error|warning|note):\ (?P<msg>.*)$', report_string, re.MULTILINE))  # noqa: E501


def _dedent_to(text: str, prefix: str) -> str:
    return textwrap.indent(textwrap.dedent(text), prefix)


if __name__ == '__main__':  # pragma: no cover
    sys.exit(main())
