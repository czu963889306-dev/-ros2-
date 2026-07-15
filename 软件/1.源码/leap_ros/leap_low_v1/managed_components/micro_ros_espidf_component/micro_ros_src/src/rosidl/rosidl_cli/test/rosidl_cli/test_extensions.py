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

import pytest

from rosidl_cli.extensions import parse_extension_specification


def test_extension_specification_parsing():
    with pytest.raises(ValueError):
        parse_extension_specification('bad[')

    with pytest.raises(ValueError):
        parse_extension_specification('bad[]')

    with pytest.raises(ValueError):
        parse_extension_specification('bad[:]')

    name, kwargs = parse_extension_specification('no_args')
    assert name == 'no_args'
    assert kwargs == {}

    name, kwargs = parse_extension_specification('with_args[key: value]')
    assert name == 'with_args'
    assert kwargs == {'key': 'value'}
