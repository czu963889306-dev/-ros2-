# Copyright 2019 Robert Bosch GmbH
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

"""Module for tracing."""

import sys
from typing import List


def tracing_supported() -> bool:
    """
    Check if tracing is supported on this platform.

    It does not mean a tracer is installed.
    """
    return sys.platform == 'linux'


def print_names_list(
    names: List[str],
    prefix: str = '\t',
) -> None:
    for name in names:
        print(f'{prefix}{name}')
