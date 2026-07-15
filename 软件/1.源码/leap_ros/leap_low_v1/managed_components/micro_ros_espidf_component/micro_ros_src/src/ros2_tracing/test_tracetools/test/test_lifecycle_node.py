# Copyright 2020 Christophe Bedard
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


class TestLifecycleNode(TraceTestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
            session_name_prefix='session-test-lifecycle-node',
            events_ros=[
                tp.rcl_node_init,
                tp.rcl_lifecycle_state_machine_init,
                tp.rcl_lifecycle_transition,
            ],
            package='test_tracetools',
            nodes=['test_lifecycle_node', 'test_lifecycle_client'],
        )

    def test_all(self):
        # Check events as set
        self.assertEventsSet(self._events_ros)

        # Check that the lifecycle node has an init event just like a normal node
        rcl_node_init_events = self.get_events_with_name(tp.rcl_node_init)
        lifecycle_node_matches = self.get_events_with_field_value(
            'node_name',
            'test_lifecycle_node',
            rcl_node_init_events,
        )
        self.assertEqual(
            len(lifecycle_node_matches),
            1,
            'no node init event for lifecycle node',
        )
        lifecycle_node_handle = self.get_field(lifecycle_node_matches[0], 'node_handle')

        # Check the state machine init event
        rcl_lifecycle_state_machine_init_events = self.get_events_with_name(
            tp.rcl_lifecycle_state_machine_init,
        )
        self.assertEqual(
            len(rcl_lifecycle_state_machine_init_events),
            1,
            'more than one state machine init event',
        )
        # Make sure the node handle matches the one from the node init event
        rcl_lifecycle_state_machine_init_event = rcl_lifecycle_state_machine_init_events[0]
        node_handle = self.get_field(rcl_lifecycle_state_machine_init_event, 'node_handle')
        self.assertEqual(
            node_handle,
            lifecycle_node_handle,
            'node init and state machine init node handles do not match',
        )
        state_machine_handle = self.get_field(
            rcl_lifecycle_state_machine_init_event,
            'state_machine',
        )

        # This list of states corresponds to the states that the test_lifecycle_node goes through
        # following test_lifecycle_client's requests
        # We do not expect any other lifecycle node
        expected_lifecycle_states = [
            'unconfigured',
            'configuring',
            'inactive',
            'activating',
            'active',
            'deactivating',
            'inactive',
            'activating',
            'active',
            'deactivating',
            'inactive',
            'cleaningup',
            'unconfigured',
            'shuttingdown',
        ]
        expected_lifecycle_state_transitions = [
            (expected_lifecycle_states[i], expected_lifecycle_states[i + 1])
            for i in range(len(expected_lifecycle_states) - 1)
        ]
        # Check transitions
        rcl_lifecycle_transition_events = self.get_events_with_name(
            tp.rcl_lifecycle_transition)
        lifecycle_state_transitions = [
            (self.get_field(event, 'start_label'), self.get_field(event, 'goal_label'))
            for event in rcl_lifecycle_transition_events
        ]
        self.assertListEqual(
            expected_lifecycle_state_transitions,
            lifecycle_state_transitions,
            'transitions not valid',
        )
        # Make sure all state machine handle values match the one from the init event
        self.assertTrue(
            all(
                self.get_field(event, 'state_machine') == state_machine_handle
                for event in rcl_lifecycle_transition_events
            ),
            'state machine handle does not match',
        )


if __name__ == '__main__':
    unittest.main()
