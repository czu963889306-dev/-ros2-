#!/usr/bin/env python3

# Copyright 2014 Open Source Robotics Foundation, Inc.
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

from rosidl_adapter.parser import parse_message_file


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description='Parse all recursively found .msg files.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        'paths',
        nargs='+',
        help='The base paths to search for .msg files')
    args = parser.parse_args(argv)

    files = get_files(args.paths)

    for filename in files:
        pkg_name = os.path.basename(os.path.dirname(os.path.dirname(filename)))
        try:
            parse_message_file(pkg_name, filename)
            print(pkg_name, filename)
        except Exception as e:
            print(' ', pkg_name, filename, str(e))
            raise

    return 0


def get_files(paths):
    files = []
    for path in paths:
        if os.path.isdir(path):
            for dirpath, dirnames, filenames in os.walk(path):
                # ignore folder starting with . or _
                dirnames[:] = [d for d in dirnames if d[0] not in ['.', '_']]
                dirnames.sort()

                # select files by extension
                for filename in sorted(filenames):
                    if filename.endswith('.msg'):
                        files.append(os.path.join(dirpath, filename))
        if os.path.isfile(path):
            files.append(path)
    return files


if __name__ == '__main__':
    sys.exit(main())
