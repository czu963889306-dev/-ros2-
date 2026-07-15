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

from tracetools_test.case import TraceTestCase
from tracetools_trace.tools import tracepoints as tp


class TestExecutor(TraceTestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
            session_name_prefix='session-test-executor',
            events_ros=[
                tp.rclcpp_executor_get_next_ready,
                tp.rclcpp_executor_wait_for_work,
                tp.rclcpp_executor_execute,
            ],
            package='test_tracetools',
            nodes=['test_ping', 'test_pong'],
        )

    def test_all(self):
        # Check events as set
        self.assertEventsSet(self._events_ros)

        # Check fields
        get_next_ready_events = self.get_events_with_name(tp.rclcpp_executor_get_next_ready)
        wait_for_work_events = self.get_events_with_name(tp.rclcpp_executor_wait_for_work)
        execute_events = self.get_events_with_name(tp.rclcpp_executor_execute)
        for event in wait_for_work_events:
            self.assertFieldType(event, 'timeout', int)
        for event in execute_events:
            self.assertValidHandle(event, 'handle')

        # Check executor events for each node
        for node_name in self._nodes:
            node_get_next_ready = self.get_events_with_procname(node_name, get_next_ready_events)
            node_wait_for_work = self.get_events_with_procname(node_name, wait_for_work_events)
            node_execute_events = self.get_events_with_procname(node_name, execute_events)

            # Should have at least one event
            self.assertNumEventsGreaterEqual(node_get_next_ready, 1)
            self.assertNumEventsGreaterEqual(node_wait_for_work, 1)
            self.assertNumEventsGreaterEqual(node_execute_events, 1)

            # Simple order check
            # (this doesn't mean that these events will be one
            # after the other without any other event in between)
            self.assertEventOrder([
                node_get_next_ready[0],
                node_wait_for_work[0],
                node_execute_events[0],
            ])


if __name__ == '__main__':
    unittest.main()
