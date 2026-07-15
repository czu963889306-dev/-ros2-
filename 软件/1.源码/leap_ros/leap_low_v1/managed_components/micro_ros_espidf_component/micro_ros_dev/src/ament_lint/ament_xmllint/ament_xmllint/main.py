#!/usr/bin/env python3

# Copyright 2014-2018 Open Source Robotics Foundation, Inc.
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
import shutil
import subprocess
import sys
import time
from xml.etree import ElementTree
from xml.sax import make_parser
from xml.sax import SAXParseException
from xml.sax.handler import ContentHandler
from xml.sax.saxutils import escape
from xml.sax.saxutils import quoteattr


def main(argv=sys.argv[1:]):
    extensions = ['xml']

    parser = argparse.ArgumentParser(
        description='Check XML markup using xmllint.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        'paths',
        nargs='*',
        default=[os.curdir],
        help='The files or directories to check. For directories files ending '
             'in %s will be considered.' %
             ', '.join(["'.%s'" % e for e in extensions]))
    parser.add_argument(
        '--exclude',
        nargs='*',
        default=[],
        help='Exclude specific file names and directory names from the check')
    # not using a file handle directly
    # in order to prevent leaving an empty file when something fails early
    parser.add_argument(
        '--xunit-file',
        help='Generate a xunit compliant XML file')
    args = parser.parse_args(argv)

    if args.xunit_file:
        start_time = time.time()

    files = get_files(args.paths, extensions, args.exclude)
    if not files:
        print('No files found', file=sys.stderr)
        return 1
    files = [os.path.abspath(f) for f in files]

    xmllint_bin = shutil.which('xmllint')
    if not xmllint_bin:
        return "Could not find 'xmllint' executable"

    report = []

    # invoke xmllint on all files
    for filename in files:
        # parse file to extract desired validation information
        parser = make_parser()
        handler = CustomHandler()
        parser.setContentHandler(handler)
        try:
            parser.parse(filename)
        except SAXParseException:
            pass

        cmd = [xmllint_bin, '--noout', filename]
        # choose validation options based on handler information
        for attributes in handler.xml_model_attributes:
            schematypens = attributes.get('schematypens')
            href = attributes.get('href')
            if schematypens is None or href is None:
                continue
            # check for XML schema
            if schematypens == 'http://www.w3.org/2001/XMLSchema':
                cmd += ['--schema', href]
            # check for RelaxNG
            elif schematypens == 'http://relaxng.org/ns/structure/1.0':
                cmd += ['--relaxng', href]
            # check for Schematron
            elif schematypens == 'http://purl.oclc.org/dsdl/schematron':
                cmd += ['--schematron', href]
        if 'xsi:noNamespaceSchemaLocation' in handler.root_attributes:
            cmd += [
                '--schema',
                handler.root_attributes['xsi:noNamespaceSchemaLocation']]

        try:
            subprocess.check_output(
                cmd, cwd=os.path.dirname(filename), stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            errors = e.output.decode()
        else:
            errors = None

        filename = os.path.relpath(filename, start=os.getcwd())
        report.append((filename, errors))

    for (filename, errors) in report:
        if errors is not None:
            print("File '%s' is invalid:" % filename, file=sys.stderr)
            for line in errors.splitlines():
                print(line, file=sys.stderr)
            print('', file=sys.stderr)
        else:
            print("File '%s' is valid" % filename)
            print('')

    # output summary
    error_count = sum(1 if r[1] else 0 for r in report)
    if not error_count:
        print('No problems found')
        rc = 0
    else:
        print('%d files are invalid' % error_count, file=sys.stderr)
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


def get_files(paths, extensions, excludes=[]):
    files = []
    for path in paths:
        if os.path.isdir(path):
            for dirpath, dirnames, filenames in os.walk(path):
                if 'AMENT_IGNORE' in dirnames + filenames:
                    dirnames[:] = []
                    continue
                # ignore folder starting with . or _
                dirnames[:] = [d for d in dirnames if d[0] not in ['.', '_']]
                # ignore excluded folders
                dirnames[:] = [d for d in dirnames if d not in excludes]
                dirnames.sort()

                # select files by extension
                for filename in sorted(filenames):
                    if filename in excludes:
                        continue
                    _, ext = os.path.splitext(filename)
                    if ext not in ['.%s' % e for e in extensions]:
                        continue
                    files.append(os.path.join(dirpath, filename))
        if os.path.isfile(path):
            files.append(path)
    return [os.path.normpath(f) for f in files]


class CustomHandler(ContentHandler):

    def __init__(self):
        super().__init__()
        self.xml_model_attributes = []
        self.root_attributes = {}
        self._first_node = False

    def processingInstruction(self, target, data):
        if target != 'xml-model':
            return

        root = ElementTree.fromstring('<data ' + data + '/>')
        self.xml_model_attributes.append(root.attrib)

    def startDocument(self):
        self._first_node = True

    def startElement(self, name, attrs):
        if not self._first_node:
            return
        self._first_node = False
        for attr_name in attrs.getNames():
            self.root_attributes[attr_name] = attrs.getValue(attr_name)


def get_xunit_content(report, testname, elapsed):
    test_count = len(report)
    error_count = len([r for r in report if r[1]])
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

    for (filename, diff_lines) in report:

        if diff_lines:
            # report any diff as a failing testcase
            data = {
                'quoted_location': quoteattr(filename),
                'testname': testname,
                'quoted_message': quoteattr(
                    'Diff with %d lines' % len(diff_lines)
                ),
                'cdata': ''.join(diff_lines),
            }
            xml += """  <testcase
    name=%(quoted_location)s
    classname="%(testname)s"
  >
      <failure message=%(quoted_message)s><![CDATA[%(cdata)s]]></failure>
  </testcase>
""" % data

        else:
            # if there is no diff report a single successful test
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
