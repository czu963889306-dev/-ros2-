# Copyright 2021 Christophe Bedard
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

import unittest

from tracetools_trace.tools import names
from tracetools_trace.tools import tracepoints


class TestNames(unittest.TestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
        )

    def test_tracepoint_names(self) -> None:
        # Make sure the list of default ROS events contains exactly all the
        # tracepoints defined as constants, otherwise something might have been forgotten
        tp_constant_names = {name for name in dir(tracepoints) if not name.startswith('__')}
        tp_names = {getattr(tracepoints, name) for name in tp_constant_names}
        self.assertTrue(all(name.startswith('ros2:') for name in tp_names), tp_names)
        self.assertSetEqual(set(names.DEFAULT_EVENTS_ROS), tp_names)


if __name__ == '__main__':
    unittest.main()
