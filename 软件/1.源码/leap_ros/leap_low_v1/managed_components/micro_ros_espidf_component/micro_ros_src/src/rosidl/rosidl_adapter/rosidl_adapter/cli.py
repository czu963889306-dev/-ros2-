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
import pathlib
import sys

from catkin_pkg.package import package_exists_at
from catkin_pkg.package import parse_package

from rosidl_adapter.action import convert_action_to_idl
from rosidl_adapter.msg import convert_msg_to_idl
from rosidl_adapter.srv import convert_srv_to_idl

from rosidl_cli.command.helpers import interface_path_as_tuple
from rosidl_cli.command.translate.extensions import TranslateCommandExtension


def convert_files_to_idl(extension, conversion_function, argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description=f'Convert {extension} files to .idl')
    parser.add_argument(
        'interface_files', nargs='+',
        help='The interface files to convert')
    args = parser.parse_args(argv)

    for interface_file in args.interface_files:
        interface_file = pathlib.Path(interface_file)
        package_dir = interface_file.parent.absolute()
        while (
            len(package_dir.parents) and
            not package_exists_at(str(package_dir))
        ):
            package_dir = package_dir.parent
        if not package_dir.parents:
            print(
                f"Could not find package for '{interface_file}'",
                file=sys.stderr)
            continue
        warnings = []
        pkg = parse_package(package_dir, warnings=warnings)

        conversion_function(
            package_dir, pkg.name,
            interface_file.absolute().relative_to(package_dir),
            interface_file.parent)


class TranslateToIDL(TranslateCommandExtension):

    output_format = 'idl'

    def translate(
        self,
        package_name,
        interface_files,
        include_paths,
        output_path
    ):
        translated_interface_files = []
        for interface_file in interface_files:
            prefix, interface_file = interface_path_as_tuple(interface_file)
            output_dir = output_path / interface_file.parent
            translated_interface_file = self.conversion_function(
                prefix, package_name, interface_file, output_dir)
            translated_interface_file = \
                translated_interface_file.relative_to(output_path)
            translated_interface_files.append(
                f'{output_path}:{translated_interface_file.as_posix()}'
            )
        return translated_interface_files


class TranslateMsgToIDL(TranslateToIDL):

    input_format = 'msg'

    @property
    def conversion_function(self):
        return convert_msg_to_idl


class TranslateSrvToIDL(TranslateToIDL):

    input_format = 'srv'

    @property
    def conversion_function(self):
        return convert_srv_to_idl


class TranslateActionToIDL(TranslateToIDL):
    input_format = 'action'

    @property
    def conversion_function(self):
        return convert_action_to_idl
