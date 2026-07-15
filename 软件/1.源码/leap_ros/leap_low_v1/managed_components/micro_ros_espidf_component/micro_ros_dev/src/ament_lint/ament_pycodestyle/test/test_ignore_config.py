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

from ament_pycodestyle.main import generate_pycodestyle_report


def test_can_ignore_errors():
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_dir = pathlib.Path(temp_dir)
        config_file = temp_dir / 'pycodestyle.ini'
        py_file = temp_dir / 'foobar.py'
        config_file.write_text('[pycodestyle]\nignore = E226\n')
        py_file.write_text('a = 1+2\n')

        report = generate_pycodestyle_report(
            config_file.name, [str(temp_dir)], None)
        assert [] == report.errors
