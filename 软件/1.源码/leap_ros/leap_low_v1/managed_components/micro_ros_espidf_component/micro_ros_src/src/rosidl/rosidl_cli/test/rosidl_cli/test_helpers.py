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

import json
import os
import pathlib

import pytest

from rosidl_cli.command.helpers import interface_path_as_tuple
from rosidl_cli.command.helpers import legacy_generator_arguments_file


def test_interface_path_as_tuple():
    prefix, path = interface_path_as_tuple('/tmp:msg/Empty.idl')
    assert pathlib.Path('msg/Empty.idl') == path
    assert pathlib.Path(os.path.abspath('/tmp')) == prefix

    prefix, path = interface_path_as_tuple('tmp:msg/Empty.idl')
    assert pathlib.Path('msg/Empty.idl') == path
    assert pathlib.Path.cwd() / 'tmp' == prefix

    prefix, path = interface_path_as_tuple('msg/Empty.idl')
    assert pathlib.Path('msg/Empty.idl') == path
    assert pathlib.Path.cwd() == prefix


@pytest.fixture
def current_path(request):
    path = pathlib.Path(request.module.__file__)
    path = path.resolve()
    path = path.parent
    cwd = pathlib.Path.cwd()
    try:
        os.chdir(str(path))
        yield path
    finally:
        os.chdir(str(cwd))


def test_legacy_generator_arguments_file(current_path):
    with legacy_generator_arguments_file(
        package_name='foo',
        interface_files=['msg/Foo.idl'],
        include_paths=['test_files/bar'],
        templates_path='templates',
        output_path='tmp',
    ) as path:
        with open(path, 'r') as fd:
            args = json.load(fd)
        assert args['package_name'] == 'foo'
        assert args['output_dir'] == str(current_path / 'tmp')
        assert args['template_dir'] == str(current_path / 'templates')
        assert args['idl_tuples'] == [f'{current_path}:msg/Foo.idl']
        path_to_dep = pathlib.Path('test_files/bar/msg/Bar.idl')
        assert args['ros_interface_dependencies'] == [
            'bar:' + str(current_path / path_to_dep)
        ]
    assert not pathlib.Path(path).exists()
