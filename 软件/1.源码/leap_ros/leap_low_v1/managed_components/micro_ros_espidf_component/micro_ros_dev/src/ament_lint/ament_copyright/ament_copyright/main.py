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
from itertools import groupby
import os
import re
import sys
import time
from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr

from ament_copyright import CONTRIBUTING_FILETYPE
from ament_copyright import get_copyright_names
from ament_copyright import get_licenses
from ament_copyright import LICENSE_FILETYPE
from ament_copyright import SOURCE_FILETYPE
from ament_copyright import UNKNOWN_IDENTIFIER
from ament_copyright.crawler import get_files
from ament_copyright.parser import get_comment_block
from ament_copyright.parser import get_index_of_next_line
from ament_copyright.parser import parse_file
from ament_copyright.parser import scan_past_coding_and_shebang_lines
from ament_copyright.parser import scan_past_empty_lines
from ament_copyright.parser import search_copyright_information


def main(argv=sys.argv[1:]):
    extensions = [
        'c', 'cc', 'cpp', 'cxx', 'h', 'hh', 'hpp', 'hxx',
        'cmake',
        'py',
    ]

    parser = argparse.ArgumentParser(
        description='Check code files for copyright and license information.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='The files or directories to check. For directories files ending '
             "in %s will be considered (except directories starting with '.' "
             "or '_' and 'setup.py' files beside 'package.xml' files)." %
             ', '.join(["'.%s'" % e for e in extensions]))
    parser.add_argument(
        '--exclude',
        metavar='filename',
        nargs='*',
        default=[],
        dest='excludes',
        help='The filenames to exclude.')
    group = parser.add_mutually_exclusive_group()
    group.add_argument(
        '--add-missing',
        nargs=2,
        metavar=('COPYRIGHT_NAME', 'LICENSE'),
        help=(
            'Add missing copyright notice and license information using the '
            'passed copyright holder and license'))
    group.add_argument(
        '--add-copyright-year',
        nargs='*',
        type=int,
        help='Add the current year to existing copyright notices')
    group.add_argument(
        '--list-copyright-names',
        action='store_true',
        help='List names of known copyright holders')
    group.add_argument(
        '--list-licenses',
        action='store_true',
        help='List names of known licenses')
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Show all files instead of only the ones with errors / modifications')
    # not using a file handle directly
    # in order to prevent leaving an empty file when something fails early
    group.add_argument(
        '--xunit-file',
        help='Generate a xunit compliant XML file')
    args = parser.parse_args(argv)

    names = get_copyright_names()
    if args.list_copyright_names:
        for key in sorted(names.keys()):
            print('%s: %s' % (key, names[key]))
        return 0

    licenses = get_licenses()
    if args.list_licenses:
        for key in sorted(licenses.keys()):
            print('%s: %s' % (key, licenses[key].name))
        return 0

    if args.xunit_file:
        start_time = time.time()

    filenames = get_files(args.paths, extensions, args.excludes)
    if not filenames:
        print('No repository roots and files found')

    file_descriptors = {}
    for filename in sorted(filenames):
        file_descriptors[filename] = parse_file(filename)

    if args.add_missing:
        name = names.get(args.add_missing[0], args.add_missing[0])
        if args.add_missing[1] not in licenses:
            parser.error(
                "'LICENSE' argument must be a known license name. "
                "Use the '--list-licenses' options to see alist of valid license names.")
        add_missing_header(
            file_descriptors, name, licenses[args.add_missing[1]], args.verbose)
        return 0

    if args.add_copyright_year is not None:
        if not args.add_copyright_year:
            args.add_copyright_year.append(time.strftime('%Y'))
        args.add_copyright_year = [int(year) for year in args.add_copyright_year]
        add_copyright_year(file_descriptors, args.add_copyright_year, args.verbose)
        return 0

    report = []

    # check each directory for CONTRIBUTING.md and LICENSE files
    for path in sorted(file_descriptors.keys()):
        file_descriptor = file_descriptors[path]
        message = None
        has_error = False

        if file_descriptor.filetype == SOURCE_FILETYPE:
            if not file_descriptor.exists:
                message = 'file not found'
                has_error = True

            elif not file_descriptor.content:
                message = 'file empty'

            elif not file_descriptor.copyright_identifiers:
                message = 'could not find copyright notice'
                has_error = True

            else:
                message = 'copyright=%s, license=%s' % \
                    (', '.join([str(c) for c in file_descriptor.copyrights]),
                     file_descriptor.license_identifier)
                has_error = file_descriptor.license_identifier == UNKNOWN_IDENTIFIER

        elif file_descriptor.filetype in [CONTRIBUTING_FILETYPE, LICENSE_FILETYPE]:
            if not file_descriptor.exists:
                message = 'file not found'
                has_error = True

            elif not file_descriptor.content:
                message = 'file empty'
                has_error = True

            elif file_descriptor.license_identifier:
                message = file_descriptor.license_identifier
                has_error = file_descriptor.license_identifier == UNKNOWN_IDENTIFIER

            else:
                assert False, file_descriptor

        else:
            assert False, 'Unknown filetype: ' + file_descriptor.filetype

        if args.verbose or has_error:
            print('%s: %s' % (file_descriptor.path, message),
                  file=sys.stderr if has_error else sys.stdout)
        report.append((file_descriptor.path, not has_error, message))

    # output summary
    error_count = len([r for r in report if not r[1]])
    if not error_count:
        print('No problems found, checked %d files' % len(report))
        rc = 0
    else:
        print('%d errors, checked %d files' % (error_count, len(report)), file=sys.stderr)
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
        with open(args.xunit_file, 'w', encoding='utf-8') as f:
            f.write(xml)

    return rc


def add_missing_header(file_descriptors, name, license_, verbose):
    copyright_ = 'Copyright %d %s' % (int(time.strftime('%Y')) - 1 + 1, name)
    header = license_.file_headers[0].format(**{
        'copyright': copyright_,
        'copyright_holder': name})
    lines = header.splitlines()

    if verbose:
        print('Adding the following copyright / license header or repository level files:')
        for line in lines:
            print('+', line)
        print()

    for path in sorted(file_descriptors.keys()):
        file_descriptor = file_descriptors[path]
        skip = False

        if file_descriptor.filetype == SOURCE_FILETYPE:
            # ignore empty files
            if not file_descriptor.content:
                skip = True
            # ignore files which already have a header
            if file_descriptor.copyright_identifiers:
                skip = True
        elif file_descriptor.exists:
            # ignore non-source files if they already exist
            skip = True

        if skip:
            if verbose:
                print(' ', file_descriptor.path)
            continue

        if file_descriptor.filetype == SOURCE_FILETYPE:
            print('*', file_descriptor.path)
            add_header(file_descriptor, header)

        elif file_descriptor.filetype == CONTRIBUTING_FILETYPE:
            print('+', file_descriptor.path)
            with open(file_descriptor.path, 'w', encoding='utf-8') as h:
                h.write(license_.contributing_files[0])

        elif file_descriptor.filetype == LICENSE_FILETYPE:
            print('+', file_descriptor.path)
            with open(file_descriptor.path, 'w', encoding='utf-8') as h:
                h.write(license_.license_files[0])

        else:
            assert False, 'Unknown filetype: ' + file_descriptor.filetype


def add_copyright_year(file_descriptors, new_years, verbose):
    if verbose:
        print('Adding the current year to existing copyright notices:')
        print()

    for path in sorted(file_descriptors.keys()):
        file_descriptor = file_descriptors[path]

        # ignore files which do not have a header
        if not getattr(file_descriptor, 'copyright_identifier', None):
            continue

        index = scan_past_coding_and_shebang_lines(file_descriptor.content)
        index = scan_past_empty_lines(file_descriptor.content, index)

        if file_descriptor.filetype == SOURCE_FILETYPE:
            block, block_offset = get_comment_block(file_descriptor.content, index)
            if not block:
                assert False, "Could not find comment block in file '%s'" % file_descriptor.path
        else:
            block = file_descriptor.content[index:]
            block_offset = 0
        copyright_span, years_span, name_span = search_copyright_information(block)
        if copyright_span is None:
            assert False, "Could not find copyright information in file '%s'" % \
                file_descriptor.path

        # skip if all new years are already included
        years = get_years_from_string(block[years_span[0]:years_span[1]])
        if all((new_year in years) for new_year in new_years):
            if verbose:
                print(' ', file_descriptor.path)
            continue
        print('*' if file_descriptor.exists else '+', file_descriptor.path)

        for new_year in new_years:
            years.add(new_year)
        years_string = get_string_from_years(years)

        # overwrite previous years with new years
        offset = index + block_offset
        global_years_span = [offset + years_span[0], offset + years_span[1]]
        content = file_descriptor.content[:global_years_span[0]] + years_string + \
            file_descriptor.content[global_years_span[1]:]

        # output beginning of file for debugging
        # index = global_years_span[0]
        # for _ in range(3):
        #     index = get_index_of_next_line(content, index)
        # print('<<<')
        # print(content[:index - 1])
        # print('>>>')

        with open(file_descriptor.path, 'w', encoding='utf-8') as h:
            h.write(content)


def get_years_from_string(content):
    # remove all whitespaces
    content = re.sub(r'\s', '', content)
    # split items by comma
    items = content.split(',')

    years = set()
    for item in items:
        # each item can be a plain year or a range of years
        parts = item.split('-', 1)
        if len(parts) == 1:
            # plain year
            years.add(int(parts[0]))
        else:
            # year range
            start_year = int(parts[0])
            end_year = int(parts[1])
            assert end_year >= start_year
            for year in range(start_year, end_year + 1):
                years.add(year)
    return years


def get_string_from_years(years):
    ranges = []
    for _, iterable in groupby(enumerate(sorted(years)), lambda x: x[1] - x[0]):
        r = list(iterable)
        if len(r) == 1:
            ranges.append(str(r[0][1]))
        else:
            ranges.append('%d-%d' % (r[0][1], r[-1][1]))
    return ', '.join(ranges)


def add_header(file_descriptor, header):
    """
    Add the copyright / license message to a file.

    The copyright / license is placed below an optional shebang line as
    well as an optional coding line.
    It is preceded by an empty line if not at the beginning of the file
    and always followed by an empty line.
    """
    begin_index = scan_past_coding_and_shebang_lines(file_descriptor.content)
    end_index = scan_past_empty_lines(file_descriptor.content, begin_index)

    # inject copyright message
    comment = get_comment(file_descriptor.path, header)
    inserted_block = '%s\n\n' % comment
    if begin_index > 0:
        inserted_block = '\n' + inserted_block
    content = file_descriptor.content[:begin_index] + inserted_block + \
        file_descriptor.content[end_index:]

    # output beginning of file for debugging
    # index = end_index + len(inserted_block)
    # for _ in range(3):
    #     index = get_index_of_next_line(content, index)
    # print('<<<')
    # print(content[:index - 1])
    # print('>>>')

    with open(file_descriptor.path, 'w', encoding='utf-8') as h:
        h.write(content)


def get_comment(filename, msg):
    if filename.endswith('.cmake') or filename.endswith('.py'):
        line_prefix = '#'
    else:
        line_prefix = '//'

    comment = ''
    index = 0
    while True:
        new_index = get_index_of_next_line(msg, index)
        if new_index == index:
            break
        comment += line_prefix
        line = msg[index:new_index]
        if line.splitlines()[0]:
            comment += ' '
        comment += line
        index = new_index
    return comment


def get_xunit_content(report, testname, elapsed):
    test_count = len(report)
    error_count = len([r for r in report if not r[1]])
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

    for (filename, no_error, message) in report:

        data = {
            'quoted_filename': quoteattr(filename),
            'testname': testname,
            'escaped_message': escape(message),
        }
        if not no_error:
            # report missing / unknown copyright / license as a failing testcase
            xml += """  <testcase
    name=%(quoted_filename)s
    classname="%(testname)s"
  >
      <failure message="%(escaped_message)s"/>
  </testcase>
""" % data

        else:
            # if there is a known copyright / license report a single successful test
            xml += """  <testcase
    name=%(quoted_filename)s
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
