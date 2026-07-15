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

import os

from ament_uncrustify.main import main


cases_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'cases')


def test_incorrect_exclusion():
    """
    Checks that excluding a single filename does not work.

    `ament_uncrustify <path> --exclude <file-name>` should not exclude anything.
    """
    rc = main(argv=[os.path.join(cases_path, 'test.cpp'), '--exclude', 'test.cpp'])
    assert rc == 0, 'Found errors'


def test_correct_exclusion():
    """
    Checks that excluding a file relatively/absolutely works as expected.

    `ament_copyright <path/filename> --exclude <path/filename>` should exclude <path/filename>.
    """
    rc = main(
        argv=[
            os.path.join(cases_path, 'test.cpp'),
            '--exclude',
            os.path.join(cases_path, 'test.cpp')
        ])
    assert rc == 1, 'Files were found'


def test_wildcard_exclusion():
    """A wildcard expression which expands to the relative path of an existing file should work."""
    rc = main(
        argv=[
            os.path.join(cases_path, 'test.cpp'),
            '--exclude',
            os.path.join(cases_path, '*')
        ])
    assert rc == 1, 'Files were found'
