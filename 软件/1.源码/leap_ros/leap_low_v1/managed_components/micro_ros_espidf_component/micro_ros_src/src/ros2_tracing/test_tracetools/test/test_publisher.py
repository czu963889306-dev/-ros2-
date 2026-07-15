# Copyright 2019 Robert Bosch GmbH
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


class TestPublisher(TraceTestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
            session_name_prefix='session-test-publisher',
            events_ros=[
                tp.rcl_node_init,
                tp.rmw_publisher_init,
                tp.rcl_publisher_init,
                tp.rclcpp_publish,
                tp.rcl_publish,
                tp.rmw_publish,
            ],
            package='test_tracetools',
            nodes=['test_publisher'],
        )

    def test_all(self):
        # Check events as set
        self.assertEventsSet(self._events_ros)

        # Check fields
        rmw_pub_init_events = self.get_events_with_name(tp.rmw_publisher_init)
        for event in rmw_pub_init_events:
            self.assertValidHandle(event, ['rmw_publisher_handle'])
            self.assertValidArray(event, 'gid', int)
        pub_init_events = self.get_events_with_name(tp.rcl_publisher_init)
        for event in pub_init_events:
            self.assertValidHandle(
                event,
                ['publisher_handle', 'node_handle', 'rmw_publisher_handle'],
            )
            self.assertValidQueueDepth(event, 'queue_depth')
            self.assertStringFieldNotEmpty(event, 'topic_name')
        rmw_publish_events = self.get_events_with_name(tp.rmw_publish)
        for event in rmw_publish_events:
            # Message is a pointer (aka a handle)
            self.assertValidHandle(
                event,
                'message',
            )
        rcl_publish_events = self.get_events_with_name(tp.rcl_publish)
        for event in rcl_publish_events:
            # Message is a pointer (aka a handle)
            self.assertValidHandle(
                event,
                ['publisher_handle', 'message'],
            )
        rclcpp_publish_events = self.get_events_with_name(tp.rclcpp_publish)
        for event in rclcpp_publish_events:
            # Message is a pointer (aka a handle)
            self.assertValidHandle(
                event,
                'message',
            )

        # Check that the test topic name exists
        test_pub_init_events = self.get_events_with_procname('test_publisher', pub_init_events)
        test_pub_init_topic_events = self.get_events_with_field_value(
            'topic_name',
            '/the_topic',
            test_pub_init_events,
        )
        self.assertNumEventsEqual(
            test_pub_init_topic_events,
            1,
            'none or more than 1 rcl_pub_init even for test topic',
        )

        # Check queue_depth value
        test_pub_init_topic_event = test_pub_init_topic_events[0]
        self.assertFieldEquals(
            test_pub_init_topic_event,
            'queue_depth',
            10,
            'pub_init event does not have expected queue depth value',
        )

        # Check that the node handle matches with the node_init event
        node_init_events = self.get_events_with_name(tp.rcl_node_init)
        test_pub_node_init_events = self.get_events_with_procname(
            'test_publisher',
            node_init_events,
        )
        self.assertNumEventsEqual(
            test_pub_node_init_events,
            1,
            'none or more than 1 node_init event',
        )
        test_pub_node_init_event = test_pub_node_init_events[0]
        self.assertMatchingField(
            test_pub_node_init_event,
            'node_handle',
            None,
            test_pub_init_events,
        )

        # Get rmw_publisher_handle of publisher and find corresponding rmw pub init event
        rmw_publisher_handle = self.get_field(test_pub_init_topic_event, 'rmw_publisher_handle')
        rmw_pub_init_events = self.get_events_with_field_value(
            'rmw_publisher_handle',
            rmw_publisher_handle,
            rmw_pub_init_events,
        )
        self.assertNumEventsEqual(
            rmw_pub_init_events,
            1,
            'none or more than 1 rmw_pub_init event for test topic',
        )
        rmw_pub_init_event = rmw_pub_init_events[0]

        # Check publisher creation events order (rmw then rcl)
        self.assertEventOrder([rmw_pub_init_event, test_pub_init_topic_event])

        # Check publish events
        # Get publisher handle from rcl_publisher_init event
        publisher_handle = self.get_field(test_pub_init_topic_event, 'publisher_handle')
        # And find pointer of published message using the corresponding rcl_publish event
        rcl_publish_topic_events = self.get_events_with_field_value(
            'publisher_handle',
            publisher_handle,
            rcl_publish_events,
        )
        self.assertNumEventsEqual(
            rcl_publish_topic_events,
            1,
            'none or more than 1 rcl_publish event for test topic',
        )
        rcl_publish_topic_event = rcl_publish_topic_events[0]
        pub_message = self.get_field(rcl_publish_topic_event, 'message')
        # Find corresponding rclcpp/rmw_publish event
        rclcpp_publish_topic_events = self.get_events_with_field_value(
            'message',
            pub_message,
            rclcpp_publish_events,
        )
        rmw_publish_topic_events = self.get_events_with_field_value(
            'message',
            pub_message,
            rmw_publish_events,
        )
        self.assertNumEventsEqual(
            rclcpp_publish_topic_events,
            1,
            'none or more than 1 rclcpp_publish event for test topic',
        )
        self.assertNumEventsEqual(
            rmw_publish_topic_events,
            1,
            'none or more than 1 rmw_publish event for test topic',
        )
        rclcpp_publish_topic_event = rclcpp_publish_topic_events[0]
        rmw_publish_topic_event = rmw_publish_topic_events[0]
        # Check the order
        self.assertEventOrder(
            [rclcpp_publish_topic_event, rcl_publish_topic_event, rmw_publish_topic_event])


if __name__ == '__main__':
    unittest.main()
