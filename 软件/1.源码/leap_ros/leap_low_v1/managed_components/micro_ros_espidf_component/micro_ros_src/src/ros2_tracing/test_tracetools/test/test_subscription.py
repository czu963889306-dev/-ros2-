# Copyright 2019 Robert Bosch GmbH
# Copyright 2019 Apex.AI, Inc.
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


class TestSubscription(TraceTestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
            session_name_prefix='session-test-subscription',
            events_ros=[
                tp.rcl_node_init,
                tp.rmw_subscription_init,
                tp.rcl_subscription_init,
                tp.rclcpp_subscription_init,
                tp.rclcpp_subscription_callback_added,
                tp.rclcpp_executor_execute,
                tp.rmw_take,
                tp.rcl_take,
                tp.rclcpp_take,
                tp.callback_start,
                tp.callback_end,
            ],
            package='test_tracetools',
            nodes=['test_ping', 'test_pong'],
        )

    def test_all(self):
        # Check events as set
        self.assertEventsSet(self._events_ros)

        # Check fields
        rmw_sub_init_events = self.get_events_with_name(tp.rmw_subscription_init)
        rcl_sub_init_events = self.get_events_with_name(tp.rcl_subscription_init)
        rclcpp_sub_init_events = self.get_events_with_name(tp.rclcpp_subscription_init)
        callback_added_events = self.get_events_with_name(
            tp.rclcpp_subscription_callback_added,
        )
        execute_events = self.get_events_with_name(tp.rclcpp_executor_execute)
        rmw_take_events = self.get_events_with_name(tp.rmw_take)
        rcl_take_events = self.get_events_with_name(tp.rcl_take)
        rclcpp_take_events = self.get_events_with_name(tp.rclcpp_take)
        start_events = self.get_events_with_name(tp.callback_start)
        end_events = self.get_events_with_name(tp.callback_end)

        for event in rmw_sub_init_events:
            self.assertValidHandle(event, ['rmw_subscription_handle'])
            self.assertValidArray(event, 'gid', int)
        for event in rcl_sub_init_events:
            self.assertValidHandle(
                event,
                ['subscription_handle', 'node_handle', 'rmw_subscription_handle'],
            )
            self.assertValidQueueDepth(event, 'queue_depth')
            self.assertStringFieldNotEmpty(event, 'topic_name')
        for event in rclcpp_sub_init_events:
            self.assertValidHandle(
                event,
                ['subscription_handle', 'subscription'],
            )
        for event in callback_added_events:
            self.assertValidHandle(event, ['subscription', 'callback'])
        for event in rmw_take_events:
            self.assertValidHandle(event, ['rmw_subscription_handle'])
            self.assertValidPointer(event, ['message'])
            self.assertFieldType(event, ['source_timestamp', 'taken'], int)
        for event in rcl_take_events:
            self.assertValidPointer(event, ['message'])
        for event in rclcpp_take_events:
            self.assertValidPointer(event, ['message'])
        for event in start_events:
            self.assertValidHandle(event, 'callback')
            is_intra_process_value = self.get_field(event, 'is_intra_process')
            self.assertIsInstance(is_intra_process_value, int, 'is_intra_process not int')
            self.assertTrue(
                is_intra_process_value in [0, 1],
                f'invalid value for is_intra_process: {is_intra_process_value}',
            )
        for event in end_events:
            self.assertValidHandle(event, 'callback')

        # Check that the pong test topic name exists
        # Note: using the ping node
        test_rcl_sub_init_events = self.get_events_with_field_value(
            'topic_name',
            '/pong',
            rcl_sub_init_events,
        )
        self.assertNumEventsEqual(test_rcl_sub_init_events, 1, 'cannot find test topic name')
        test_rcl_sub_init_event = test_rcl_sub_init_events[0]

        # Check queue_depth value
        self.assertFieldEquals(
            test_rcl_sub_init_event,
            'queue_depth',
            10,
            'sub_init event does not have expected queue depth value',
        )

        # Check that the node handle matches the node_init event
        node_init_events = self.get_events_with_name(tp.rcl_node_init)
        test_sub_node_init_events = self.get_events_with_procname(
            'test_ping',
            node_init_events,
        )
        self.assertNumEventsEqual(
            test_sub_node_init_events,
            1,
            'none or more than 1 node_init event',
        )
        test_sub_node_init_event = test_sub_node_init_events[0]
        self.assertMatchingField(
            test_sub_node_init_event,
            'node_handle',
            tp.rcl_subscription_init,
            rcl_sub_init_events,
        )

        # Check that subscription handle matches between rcl_sub_init and rclcpp_sub_init
        subscription_handle = self.get_field(test_rcl_sub_init_event, 'subscription_handle')
        rclcpp_sub_init_matching_events = self.get_events_with_field_value(
            'subscription_handle',
            subscription_handle,
            rclcpp_sub_init_events,
        )
        # Should only have 1 rclcpp_sub_init event, since intra-process is not enabled
        self.assertNumEventsEqual(
            rclcpp_sub_init_matching_events,
            1,
            'none or more than 1 rclcpp_sub_init event for topic',
        )
        # Check that the rmw subscription handle matches between rmw_sub_init and rcl_sub_init
        rmw_subscription_handle = self.get_field(
            test_rcl_sub_init_event, 'rmw_subscription_handle')
        rmw_sub_init_events = self.get_events_with_field_value(
            'rmw_subscription_handle',
            rmw_subscription_handle,
            rmw_sub_init_events,
        )
        self.assertNumEventsEqual(
            rmw_sub_init_events,
            1,
            'none or more than 1 rmw_sub_init event for test topic',
        )
        rmw_sub_init_event = rmw_sub_init_events[0]

        # Check that subscription pointer matches between rclcpp_sub_init and sub_callback_added
        rclcpp_sub_init_matching_event = rclcpp_sub_init_matching_events[0]
        subscription_pointer = self.get_field(rclcpp_sub_init_matching_event, 'subscription')
        callback_added_matching_events = self.get_events_with_field_value(
            'subscription',
            subscription_pointer,
            callback_added_events,
        )
        self.assertNumEventsEqual(
            callback_added_matching_events,
            1,
            'none or more than 1 rclcpp_sub_callback_added event for topic',
        )
        callback_added_matching_event = callback_added_matching_events[0]

        # Check susbcription creation events order
        self.assertEventOrder([
            rmw_sub_init_event,
            test_rcl_sub_init_event,
            rclcpp_sub_init_matching_event,
            callback_added_matching_event,
        ])

        # Get executor_execute and *_take events, there should only be one message received
        test_execute_events = self.get_events_with_field_value(
            'handle',
            subscription_handle,
            execute_events,
        )
        self.assertNumEventsEqual(
            test_execute_events,
            1,
            'none or more than 1 executor_execute event for topic sub',
        )
        test_execute_event = test_execute_events[0]
        test_rmw_take_events = self.get_events_with_field_value(
            'rmw_subscription_handle',
            rmw_subscription_handle,
            rmw_take_events,
        )
        self.assertNumEventsEqual(
            test_execute_events,
            1,
            'none or more than 1 executor_execute event for topic sub',
        )
        test_rmw_take_event = test_rmw_take_events[0]
        test_taken_msg = self.get_field(test_rmw_take_event, 'message')
        self.assertFieldEquals(test_rmw_take_event, 'taken', 1, 'test message not taken')
        test_rcl_take_events = self.get_events_with_field_value(
            'message',
            test_taken_msg,
            rcl_take_events,
        )
        self.assertNumEventsEqual(
            test_rcl_take_events,
            1,
            'none or more than 1 rcl_take event for topic sub',
        )
        test_rcl_take_event = test_rcl_take_events[0]
        test_rclcpp_take_events = self.get_events_with_field_value(
            'message',
            test_taken_msg,
            rclcpp_take_events,
        )
        self.assertNumEventsEqual(
            test_rcl_take_events,
            1,
            'none or more than 1 rclcpp_take event for topic sub',
        )
        test_rclcpp_take_event = test_rclcpp_take_events[0]

        # Check that each start:end pair has a common callback handle
        ping_events = self.get_events_with_procname('test_ping')
        pong_events = self.get_events_with_procname('test_pong')
        ping_events_start = self.get_events_with_name(tp.callback_start, ping_events)
        pong_events_start = self.get_events_with_name(tp.callback_start, pong_events)
        for ping_start in ping_events_start:
            self.assertMatchingField(
                ping_start,
                'callback',
                tp.callback_end,
                ping_events,
            )
        for pong_start in pong_events_start:
            self.assertMatchingField(
                pong_start,
                'callback',
                tp.callback_end,
                pong_events,
            )

        # Check that callback pointer matches between sub_callback_added and callback_start/end
        # There is only one callback for /pong topic in ping node
        callback_added_matching_event = callback_added_matching_events[0]
        callback_pointer = self.get_field(callback_added_matching_event, 'callback')
        callback_start_matching_events = self.get_events_with_field_value(
            'callback',
            callback_pointer,
            ping_events_start,
        )
        self.assertNumEventsEqual(
            callback_start_matching_events,
            1,
            'none or more than 1 callback_start event for topic callback',
        )
        callback_start_matching_event = callback_start_matching_events[0]
        ping_events_end = self.get_events_with_name(tp.callback_end, ping_events)
        callback_end_matching_events = self.get_events_with_field_value(
            'callback',
            callback_pointer,
            ping_events_end,
        )
        self.assertNumEventsEqual(
            callback_end_matching_events,
            1,
            'none or more than 1 callback_end event for topic callback',
        )
        callback_end_matching_event = callback_end_matching_events[0]

        # Check execute+take+callback order
        self.assertEventOrder([
            test_execute_event,
            test_rmw_take_event,
            test_rcl_take_event,
            test_rclcpp_take_event,
            callback_start_matching_event,
            callback_end_matching_event,
        ])


if __name__ == '__main__':
    unittest.main()
