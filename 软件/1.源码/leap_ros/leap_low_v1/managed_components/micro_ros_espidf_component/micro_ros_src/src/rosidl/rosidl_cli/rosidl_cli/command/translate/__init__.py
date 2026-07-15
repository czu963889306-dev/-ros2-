# Copyright 2021 Open Source Robotics Foundation, Inc.
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

import pathlib

from rosidl_cli.command import Command

from .api import translate


class TranslateCommand(Command):
    """Translate interface definition files from one format to another."""

    name = 'translate'

    def add_arguments(self, parser):
        parser.add_argument(
            '-o', '--output-path', metavar='PATH',
            type=pathlib.Path, default=None,
            help=('Path to directory to hold translated interface '
                  "definition files. Defaults to '.'.")
        )
        parser.add_argument(
            '--use', '--translator', metavar='TRANSLATOR',
            dest='translators', action='append', default=[],
            help=('Translator to be used. If none is specified, '
                  'all available ones will be considered.')
        )
        parser.add_argument(
            '--to', '--output-format', required=True,
            metavar='FORMAT', dest='output_format',
            help='Output format for translate interface definition files.'
        )
        parser.add_argument(
            '--from', '--input-format', default=None,
            metavar='FORMAT', dest='input_format',
            help=('Input format for all source interface definition files. '
                  'If not given, file extensions will be used to deduce '
                  'the format of each interface definition file.')
        )
        parser.add_argument(
            '-I', '--include-path', metavar='PATH', type=pathlib.Path,
            dest='include_paths', action='append', default=[],
            help='Paths to include dependency interface definition files from.'
        )
        parser.add_argument(
            'package_name',
            help='Name of the package all interface files belong to')
        parser.add_argument(
            'interface_files', metavar='interface_file', nargs='+',
            help=('Relative path to an interface definition file. '
                  "If prefixed by another path followed by a colon ':', "
                  'path resolution is performed against such path.')
        )

    def main(self, *, args):
        translate(
            package_name=args.package_name,
            interface_files=args.interface_files,
            output_format=args.output_format,
            input_format=args.input_format,
            include_paths=args.include_paths,
            output_path=args.output_path,
            translators=args.translators
        )
