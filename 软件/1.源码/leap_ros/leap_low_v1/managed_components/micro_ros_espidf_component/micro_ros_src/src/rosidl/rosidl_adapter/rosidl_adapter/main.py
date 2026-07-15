# Copyright 2018 Open Source Robotics Foundation, Inc.
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
import json
import os
import pathlib
import sys


from rosidl_adapter import convert_to_idl


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description='Convert interface files to .idl')
    parser.add_argument(
        '--package-name', required=True,
        help='The name of the package')
    parser.add_argument(
        '--arguments-file', required=True,
        help='The JSON file containing the non-idl tuples to convert to .idl')
    parser.add_argument(
        '--output-dir', required=True,
        help='The base directory to create .idl files in')
    parser.add_argument(
        '--output-file', required=True,
        help='The output file containing the tuples for the generated .idl '
             'files')
    args = parser.parse_args(argv)
    output_dir = pathlib.Path(args.output_dir)
    output_file = pathlib.Path(args.output_file)

    with open(args.arguments_file, 'r') as h:
        data = json.load(h)

    idl_tuples = []
    for non_idl_tuple in data['non_idl_tuples']:
        # only take the filastrst : for separation, since the first tuple
        # contains an absolute path which on Windows contains a colon
        basepath, relative_path = non_idl_tuple.rsplit(':', 1)
        abs_idl_file = convert_to_idl(
            pathlib.Path(basepath), args.package_name,
            pathlib.Path(relative_path), output_dir)
        idl_tuples.append((output_dir, abs_idl_file.relative_to(output_dir)))

    output_file.parent.mkdir(exist_ok=True)
    with output_file.open('w') as h:
        for basepath, relative_path in idl_tuples:
            line = f'{basepath}:{relative_path}\n'
            # use CMake friendly separator
            line = line.replace(os.sep, '/')
            h.write(line)
