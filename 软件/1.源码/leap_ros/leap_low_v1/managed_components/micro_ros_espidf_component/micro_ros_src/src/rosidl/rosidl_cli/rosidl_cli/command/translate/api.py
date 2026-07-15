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

import collections
import os
import pathlib

from .extensions import load_translate_extensions


def translate(
    *,
    package_name,
    interface_files,
    output_format,
    input_format=None,
    include_paths=None,
    output_path=None,
    translators=None
):
    """
    Translate interface definition files from one format to another.

    To do so, this function leverages translation support as provided
    by third-party package extensions.

    Each path to an interface definition file is a relative path optionally
    prefixed by another path followed by a colon ':', against which the first
    relative path is to be resolved.

    The directory structure that these relative paths exhibit will be
    replicated on output (as opposed to the prefix path, which will be
    ignored).

    If no translators are specified, all available ones will be considered.

    :param package_name: name of the package all interface files belong to
    :param interface_files: list of paths to interface definition files
    :param output_format: format to translate interface definition files to
    :param input_format: optional format to assume for all interface
        definition files, deduced from file extensions if not given
    :param include_paths: optional list of paths to include dependency
        interface definition files from
    :param output_path: optional path to directory to hold translated
        interface definition files, defaults to the current working directory
    :param translators: optional list of translators to use
    :returns: list of paths to translated interface definition files
    """
    extensions = load_translate_extensions(
        specs=translators, strict=bool(translators)
    )
    if not extensions:
        raise RuntimeError('No translate extensions found')

    if not input_format:
        interface_files_per_format = collections.defaultdict(list)
        for interface_file in interface_files:
            input_format = os.path.splitext(interface_file)[-1][1:]
            interface_files_per_format[input_format].append(interface_file)
    else:
        interface_files_per_format = {input_format: interface_files}

    if include_paths is None:
        include_paths = []

    if output_path is None:
        output_path = pathlib.Path.cwd()
    else:
        os.makedirs(output_path, exist_ok=True)

    translated_interface_files = []
    for input_format, interface_files in interface_files_per_format.items():
        extension = next((
            extension for extension in extensions
            if extension.input_format == input_format and
            extension.output_format == output_format
        ), None)

        if not extension:
            raise RuntimeError('\n'.join([
                f"Cannot translate the following files to '{output_format}' format:",
                *[f'- {path}' for path in interface_files],
                'No translator found'
            ]))

        translated_interface_files.extend(extension.translate(
            package_name, interface_files, include_paths, output_path))

    return translated_interface_files
