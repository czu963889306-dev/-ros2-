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

import unittest

from tracetools_test.case import TraceTestCase
from tracetools_trace.tools import tracepoints as tp


VERSION_REGEX = r'^[0-9]+\.[0-9]+\.[0-9]+$'


class TestNode(TraceTestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
            session_name_prefix='session-test-node-creation',
            events_ros=[
                tp.rcl_init,
                tp.rcl_node_init,
            ],
            package='test_tracetools',
            nodes=['test_publisher'],
        )

    def test_all(self):
        # Check events as set
        self.assertEventsSet(self._events_ros)

        # Check fields
        rcl_init_events = self.get_events_with_name(tp.rcl_init)
        for event in rcl_init_events:
            self.assertValidHandle(event, 'context_handle')
            # TODO actually compare to version fetched from the tracetools package?
            version_field = self.get_field(event, 'version')
            self.assertRegex(version_field, VERSION_REGEX, 'invalid version number')

        rcl_node_init_events = self.get_events_with_name(tp.rcl_node_init)
        for event in rcl_node_init_events:
            self.assertValidHandle(event, ['node_handle', 'rmw_handle'])
            self.assertStringFieldNotEmpty(event, 'node_name')
            self.assertStringFieldNotEmpty(event, 'namespace')

        # Check that the launched nodes have a corresponding rcl_node_init event
        node_name_fields = [self.get_field(e, 'node_name') for e in rcl_node_init_events]
        for node_name in self._nodes:
            self.assertTrue(
                node_name in node_name_fields,
                f'cannot find node_init event for node name: {node_name} ({node_name_fields})',
            )


if __name__ == '__main__':
    unittest.main()
