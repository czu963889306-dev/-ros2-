# Copyright 2023 Open Source Robotics Foundation, Inc.
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

import pathlib
import tempfile

from ament_pep257.main import _ament_ignore
from ament_pep257.main import generate_pep257_report


def test_invalid_file():
    ignore = ','.join(_ament_ignore)

    report = generate_pep257_report(['non_existent_file.py'], [], ignore, [], 'ament', [], [])
    assert len(report) == 1
    filename, errors = report[0]
    assert filename == 'non_existent_file.py'
    assert len(errors) == 1
    error = errors[0]
    assert error['linenumber'] == '-'
    assert error['category'] == 'unknown'


def test_valid_file():
    ignore = ','.join(_ament_ignore)

    with tempfile.TemporaryDirectory() as temp_dir:
        temp_dir = pathlib.Path(temp_dir)
        py_file = temp_dir / 'foobar.py'
        py_file.write_text('a = 1+2\n')

        report = generate_pep257_report([str(temp_dir)], [], ignore, [], 'ament', [], [])

        assert len(report) == 1
        filename, errors = report[0]
        assert errors == []
