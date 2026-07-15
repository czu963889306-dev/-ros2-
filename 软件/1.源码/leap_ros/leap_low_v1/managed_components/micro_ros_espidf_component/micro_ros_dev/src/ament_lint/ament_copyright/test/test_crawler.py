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

from pathlib import Path

from ament_copyright.crawler import get_files


cases_path = Path(__file__).parent / 'cases' / 'crawler'
extensions = [
    'c', 'cc', 'cpp', 'cxx', 'h', 'hh', 'hpp', 'hxx',
    'cmake',
    'py',
]


def test_search_nested():
    """Test that files are found across nested directories."""
    assert len(get_files([cases_path], extensions, [])) == 4


def test_search_incorrect_extensions():
    """Test that extensions are respected during search."""
    assert not get_files([cases_path], ['java'], [])


def test_search_single_file_name():
    """
    Test that the crawler finds no files with simply a filename.

    The assumption being that said filename does not exist relative to the test script.
    """
    assert not get_files(['case.py'], extensions, [])


def test_exclude_file_name_incorrect():
    """Excluding a single file name should not work, all files found."""
    excludes = [str('case.py')]
    assert len(get_files([cases_path], extensions, excludes)) == 4


def test_exclude_file_name_correct():
    """
    Checks that the crawler excludes a single file if that is excluded relatively/absolutely.

    Plus, checks that nested directories are respected.
    """
    excludes = [str(cases_path / 'case.py')]
    assert len(get_files([cases_path], extensions, excludes)) == 3
    excludes = [str(cases_path / 'subdir' / 'case.py')]
    assert len(get_files([cases_path], extensions, excludes)) == 3


def test_exclude_dir():
    """Excluding a directory alone should not work, all files found."""
    assert len(get_files([cases_path], extensions, [str(cases_path)])) == 4


def test_exclude_wildcard():
    """Wildcard exclusion should work, and nested directories are respected."""
    excludes = [str(cases_path / '*')]
    assert len(get_files([cases_path], extensions, excludes)) == 2
    excludes = [str(cases_path / 'subdir' / '*')]
    assert len(get_files([cases_path], extensions, excludes)) == 2
