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


class TestService(TraceTestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
            session_name_prefix='session-test-service',
            events_ros=[
                tp.rcl_node_init,
                tp.rcl_service_init,
                tp.rclcpp_service_callback_added,
                tp.rclcpp_callback_register,
                tp.callback_start,
                tp.callback_end,
            ],
            package='test_tracetools',
            nodes=['test_service_ping', 'test_service_pong'],
        )

    def test_all(self):
        # Check events as set
        self.assertEventsSet(self._events_ros)

        # Check fields
        srv_init_events = self.get_events_with_name(tp.rcl_service_init)
        callback_added_events = self.get_events_with_name(tp.rclcpp_service_callback_added)
        callback_register_events = self.get_events_with_name(tp.rclcpp_callback_register)
        start_events = self.get_events_with_name(tp.callback_start)
        end_events = self.get_events_with_name(tp.callback_end)

        for event in srv_init_events:
            self.assertValidHandle(event, ['service_handle', 'node_handle', 'rmw_service_handle'])
            self.assertStringFieldNotEmpty(event, 'service_name')
        for event in callback_added_events:
            self.assertValidHandle(event, ['service_handle', 'callback'])
        for event in callback_register_events:
            self.assertValidPointer(event, 'callback')
            self.assertStringFieldNotEmpty(event, 'symbol')
        for event in start_events:
            self.assertValidHandle(event, 'callback')
            # Should not be 1 for services (yet)
            self.assertFieldEquals(
                event,
                'is_intra_process',
                0,
                'invalid value for is_intra_process',
            )
        for event in end_events:
            self.assertValidHandle(event, 'callback')

        # Check that the test services names exists
        ping_node_srv_init_events = self.get_events_with_procname(
            'test_service_ping',
            srv_init_events,
        )
        ping_node_test_srv_init_events = self.get_events_with_field_value(
            'service_name',
            '/pong',
            ping_node_srv_init_events,
        )
        self.assertEqual(
            len(ping_node_test_srv_init_events),
            1,
            'none or more than 1 /pong service under the test_service_pong node',
        )

        pong_node_srv_init_events = self.get_events_with_procname(
            'test_service_pong',
            srv_init_events,
        )
        pong_node_test_srv_init_events = self.get_events_with_field_value(
            'service_name',
            '/ping',
            pong_node_srv_init_events,
        )
        self.assertGreaterEqual(
            len(pong_node_test_srv_init_events),
            1,
            'cannot find test service name',
        )

        # Check that the service init events have a matching node handle (with node_init events)
        node_init_events = self.get_events_with_name(tp.rcl_node_init)

        ping_node_test_srv_init_event = ping_node_test_srv_init_events[0]
        self.assertMatchingField(
            ping_node_test_srv_init_event,
            'node_handle',
            None,
            node_init_events,
            False,
        )

        pong_node_test_srv_init_event = pong_node_test_srv_init_events[0]
        self.assertMatchingField(
            pong_node_test_srv_init_event,
            'node_handle',
            None,
            node_init_events,
            False,
        )

        # Check that there are matching rclcpp_service_callback_added events
        ping_node_test_service_handle = self.get_field(
            ping_node_test_srv_init_event,
            'service_handle',
        )
        pong_node_test_service_handle = self.get_field(
            pong_node_test_srv_init_event,
            'service_handle',
        )

        ping_node_srv_callback_added_events = self.get_events_with_procname(
            'test_service_ping',
            callback_added_events,
        )
        ping_node_test_srv_callback_added_events = self.get_events_with_field_value(
            'service_handle',
            ping_node_test_service_handle,
            ping_node_srv_callback_added_events,
        )
        self.assertEqual(
            len(ping_node_test_srv_callback_added_events),
            1,
            'none or more than 1 matching callback_added events for the test_service_ping node',
        )

        pong_node_srv_callback_added_events = self.get_events_with_procname(
            'test_service_pong',
            callback_added_events,
        )
        pong_node_test_srv_callback_added_events = self.get_events_with_field_value(
            'service_handle',
            pong_node_test_service_handle,
            pong_node_srv_callback_added_events,
        )
        self.assertEqual(
            len(pong_node_test_srv_callback_added_events),
            1,
            'none or more than 1 matching callback_added events for the test_service_pong node',
        )

        # Check that there are matching rclcpp_callback_register events
        ping_node_test_callback_ref = self.get_field(
            ping_node_test_srv_callback_added_events[0],
            'callback',
        )
        pong_node_test_callback_ref = self.get_field(
            pong_node_test_srv_callback_added_events[0],
            'callback',
        )

        ping_node_callback_register_events = self.get_events_with_procname(
            'test_service_ping',
            callback_register_events,
        )
        ping_node_test_callback_register_events = self.get_events_with_field_value(
            'callback',
            ping_node_test_callback_ref,
            ping_node_callback_register_events,
        )
        self.assertEqual(
            len(ping_node_test_callback_register_events),
            1,
            'none or more than 1 matching callback_register events for the test_service_ping node',
        )

        pong_node_callback_register_events = self.get_events_with_procname(
            'test_service_pong',
            callback_register_events,
        )
        pong_node_test_callback_register_events = self.get_events_with_field_value(
            'callback',
            pong_node_test_callback_ref,
            pong_node_callback_register_events,
        )
        self.assertEqual(
            len(pong_node_test_callback_register_events),
            1,
            'none or more than 1 matching callback_register events for the test_service_pong node',
        )

        # Check that there are corresponding callback_start/stop pairs
        ping_node_test_callback_start_events = self.get_events_with_field_value(
            'callback',
            ping_node_test_callback_ref,
            start_events,
        )
        self.assertEqual(
            len(ping_node_test_callback_start_events),
            1,
            'none or more than 1 matching callback_start events for the test_service_ping node',
        )

        pong_node_test_callback_start_events = self.get_events_with_field_value(
            'callback',
            pong_node_test_callback_ref,
            start_events,
        )
        self.assertEqual(
            len(pong_node_test_callback_start_events),
            1,
            'none or more than 1 matching callback_start events for the test_service_pong node',
        )

        ping_node_test_callback_end_events = self.get_events_with_field_value(
            'callback',
            ping_node_test_callback_ref,
            end_events,
        )
        self.assertEqual(
            len(ping_node_test_callback_end_events),
            1,
            'none or more than 1 matching callback_end events for the test_service_ping node',
        )

        pong_node_test_callback_end_events = self.get_events_with_field_value(
            'callback',
            pong_node_test_callback_ref,
            end_events,
        )
        self.assertEqual(
            len(pong_node_test_callback_end_events),
            1,
            'none or more than 1 matching callback_end events for the test_service_pong node',
        )


if __name__ == '__main__':
    unittest.main()
