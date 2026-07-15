# Copyright 2016 Open Source Robotics Foundation, Inc.
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

from flake8.api.legacy import get_style_guide
import pytest


@pytest.mark.flake8
@pytest.mark.linter
def test_flake8():
    # Configure flake8 using the .flake8 file in the root of this repository.
    style = get_style_guide()
    results = style.check_files()
    assert results.total_errors == 0, 'Found code style errors / warnings'
