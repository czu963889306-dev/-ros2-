# Copyright 2019 Open Source Robotics Foundation, Inc.
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

from contextlib import redirect_stdout
import io
import os

from ament_copyright.main import main


cases_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'cases')
f = io.StringIO()


def test_apache2_standard():
    rc = main(argv=[os.path.join(cases_path, 'apache2_license')])
    assert rc == 0, 'Found errors'


def test_boost1_cpp():
    rc = main(argv=[os.path.join(cases_path, 'boost1/case2.cpp')])
    assert rc == 0, 'Found errors'


def test_boost1_py():
    rc = main(argv=[os.path.join(cases_path, 'boost1/case.py')])
    assert rc == 0, 'Found errors'


def test_bsd_standard():
    rc = main(argv=[os.path.join(cases_path, 'bsd_license')])
    assert rc == 0, 'Found errors'


def test_bsd_indented():
    rc = main(argv=[os.path.join(cases_path, 'bsd_license_indented')])
    assert rc == 0, 'Found errors'


def test_bsd_tabs():
    rc = main(argv=[os.path.join(cases_path, 'bsd_license_tabs')])
    assert rc == 0, 'Found errors'


def test_3bsd_cpp():
    rc = main(argv=[os.path.join(cases_path, '3clause_bsd/case2.cpp')])
    assert rc == 0, 'Found errors'


def test_3bsd_py():
    rc = main(argv=[os.path.join(cases_path, '3clause_bsd/case.py')])
    assert rc == 0, 'Found errors'


def test_mit0_py():
    rc = main(argv=[os.path.join(cases_path, 'mit0/case.py')])
    assert rc == 0, 'Found errors'


def test_mit_py():
    rc = main(argv=[os.path.join(cases_path, 'mit/case.py')])
    assert rc == 0, 'Found errors'


def test_mit_cpp():
    rc = main(argv=[os.path.join(cases_path, 'mit/case2.cpp')])
    assert rc == 0, 'Found errors'


def test_incorrect_exclusion():
    """
    Checks that excluding a single filename does not work.

    `ament_copyright <path> --exclude <file-name>` should not exclude anything.
    """
    rc = main(argv=[os.path.join(cases_path, 'mit/case.py'), '--exclude', 'case.py'])
    assert rc == 0, 'Found errors'


def test_correct_exclusion():
    """
    Checks that excluding a file relatively/absolutely works as expected.

    `ament_copyright <path/filename> --exclude <path/filename>` should exclude <path/filename>.
    """
    with redirect_stdout(f):
        rc = main(
            argv=[
                os.path.join(cases_path, 'mit/case.py'),
                '--exclude',
                os.path.join(cases_path, 'mit/case.py')
            ])
    assert 'No repository roots and files found' in f.getvalue()
    assert rc == 0, 'Found errors'


def test_wildcard_exclusion():
    """
    Checks that wildcard exclusion works correctly.

    `ament_copyright <path> --exclude <path/*>` should exclude all files in <path>.
    """
    with redirect_stdout(f):
        rc = main(
            argv=[
                os.path.join(cases_path, 'mit'),
                '--exclude',
                os.path.join(cases_path, 'mit/*')
            ])
    assert 'No repository roots and files found' in f.getvalue()
    assert rc == 0, 'Found errors'
