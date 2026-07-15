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

from rosidl_cli.extensions import Extension
from rosidl_cli.extensions import load_extensions


class GenerateCommandExtension(Extension):
    """
    The extension point for source code generation.

    The following methods must be defined:
    * `generate`
    """

    def generate(
        self,
        package_name,
        interface_files,
        include_paths,
        output_path
    ):
        """
        Generate source code.

        Paths to interface definition files are relative paths optionally
        prefixed by an absolute path followed by a colon ':', in which case
        path resolution is to be performed against that absolute path.

        :param package_name: name of the package to generate source code for
        :param interface_files: list of paths to interface definition files
        :param include_paths: list of paths to include dependency interface
          definition files from.
        :param output_path: path to directory to hold generated source code files
        :returns: list of paths to generated source files
        """
        raise NotImplementedError()


def load_type_extensions(**kwargs):
    """Load extensions for type representation source code generation."""
    return load_extensions('rosidl_cli.command.generate.type_extensions', **kwargs)


def load_typesupport_extensions(**kwargs):
    """Load extensions for type support source code generation."""
    return load_extensions('rosidl_cli.command.generate.typesupport_extensions', **kwargs)
