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


class TranslateCommandExtension(Extension):
    """
    The extension point for interface definition translation.

    The following attributes must be defined
    * `input_format`
    * `output_format`

    The following methods must be defined:
    * `translate`
    """

    def translate(
        self,
        package_name,
        interface_files,
        include_paths,
        output_path
    ):
        """
        Translate interface definition files.

        The path to an interface definition file is a relative path optionally
        prefixed by an absolute path followed by a colon ':', in which case
        path resolution is to be performed against that absolute path.

        On output, the directory structure specified by this relative path
        will be replicated e.g. an ``msg/Empty.foo`` file will result in a
        ``msg/Empty.bar`` file under `output_path`.

        :param package_name: name of the package `interface_file` belongs to
        :param interface_files: list of paths to interface definition files
        :param include_paths: list of paths to include dependency interface
          definition files from
        :param output_path: path to directory to hold translated interface
          definition files
        :returns: list of paths to translated interface definition files
        """
        raise NotImplementedError()


def load_translate_extensions(**kwargs):
    """Load extensions for interface definition translation."""
    return load_extensions(
        'rosidl_cli.command.translate.extensions', **kwargs
    )
