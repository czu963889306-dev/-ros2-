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

from rosidl_cli.common import get_first_line_doc


def test_getting_first_line_from_no_docstring():
    func = test_getting_first_line_from_no_docstring
    line = get_first_line_doc(func)
    assert line == ''


def test_getting_first_line_from_docstring():
    """Check it gets the first line."""
    func = test_getting_first_line_from_docstring
    line = get_first_line_doc(func)
    assert line == 'Check it gets the first line'


def test_getting_first_line_from_multiline_docstring():
    """
    Check it really gets the first non-empty line.

    Additional paragraph to please flake8.
    """
    func = test_getting_first_line_from_multiline_docstring
    line = get_first_line_doc(func)
    assert line == 'Check it really gets the first non-empty line'
