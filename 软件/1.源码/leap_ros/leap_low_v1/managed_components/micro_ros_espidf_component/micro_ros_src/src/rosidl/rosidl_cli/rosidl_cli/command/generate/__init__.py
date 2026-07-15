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

from .api import generate


class GenerateCommand(Command):
    """Generate source code from interface definition files."""

    name = 'generate'

    def add_arguments(self, parser):
        parser.add_argument(
            '-o', '--output-path', metavar='PATH',
            type=pathlib.Path, default=None,
            help=('Path to directory to hold generated '
                  "source code files. Defaults to '.'."))
        parser.add_argument(
            '-t', '--type', metavar='TYPE',
            dest='types', action='append', default=[],
            help='Target type representations for generation.')
        parser.add_argument(
            '-ts', '--type-support', metavar='TYPESUPPORT',
            dest='typesupports', action='append', default=[],
            help='Target type supports for generation.')
        parser.add_argument(
            '-I', '--include-path', type=pathlib.Path, metavar='PATH',
            dest='include_paths', action='append', default=[],
            help='Paths to include dependency interface definition files from.')
        parser.add_argument(
            'package_name', help='Name of the package to generate code for')
        parser.add_argument(
            'interface_files', metavar='interface_file', nargs='+',
            help=('Relative path to an interface definition file. '
                  "If prefixed by another path followed by a colon ':', "
                  'path resolution is performed against such path.'))

    def main(self, *, args):
        generate(
            package_name=args.package_name,
            interface_files=args.interface_files,
            include_paths=args.include_paths,
            output_path=args.output_path,
            types=args.types,
            typesupports=args.typesupports
        )
