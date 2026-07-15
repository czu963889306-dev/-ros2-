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


class TestPubSub(TraceTestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
            session_name_prefix='session-test-pub-sub',
            events_ros=[
                tp.rmw_publisher_init,
                tp.rcl_publisher_init,
                tp.rmw_publish,
                tp.rcl_publish,
                tp.rclcpp_publish,
                tp.rmw_subscription_init,
                tp.rcl_subscription_init,
                tp.rclcpp_subscription_init,
                tp.rclcpp_subscription_callback_added,
                tp.callback_start,
                tp.callback_end,
            ],
            package='test_tracetools',
            nodes=['test_ping', 'test_pong'],
        )

    def test_all(self):
        # Check events as set
        self.assertEventsSet(self._events_ros)

        # Get publisher init events & publisher handles of test topics
        rmw_pub_init_events = self.get_events_with_name(tp.rmw_publisher_init)
        rmw_sub_init_events = self.get_events_with_name(tp.rmw_subscription_init)
        publisher_init_events = self.get_events_with_name(tp.rcl_publisher_init)
        ping_publisher_init_events = self.get_events_with_field_value(
            'topic_name',
            '/ping',
            publisher_init_events,
        )
        pong_publisher_init_events = self.get_events_with_field_value(
            'topic_name',
            '/pong',
            publisher_init_events,
        )
        self.assertNumEventsEqual(ping_publisher_init_events, 1)
        self.assertNumEventsEqual(pong_publisher_init_events, 1)
        ping_publisher_init_event = ping_publisher_init_events[0]
        pong_publisher_init_event = pong_publisher_init_events[0]
        ping_pub_handle = self.get_field(ping_publisher_init_event, 'publisher_handle')
        ping_rmw_pub_handle = self.get_field(ping_publisher_init_event, 'rmw_publisher_handle')
        pong_pub_handle = self.get_field(pong_publisher_init_event, 'publisher_handle')
        pong_rmw_pub_handle = self.get_field(pong_publisher_init_event, 'rmw_publisher_handle')

        # Find corresponding rmw_pub_init events
        ping_rmw_pub_init_events = self.get_events_with_field_value(
            'rmw_publisher_handle',
            ping_rmw_pub_handle,
            rmw_pub_init_events,
        )
        pong_rmw_pub_init_events = self.get_events_with_field_value(
            'rmw_publisher_handle',
            pong_rmw_pub_handle,
            rmw_pub_init_events,
        )
        self.assertNumEventsEqual(ping_rmw_pub_init_events, 1)
        self.assertNumEventsEqual(pong_rmw_pub_init_events, 1)
        ping_rmw_pub_init_event = ping_rmw_pub_init_events[0]
        pong_rmw_pub_init_event = pong_rmw_pub_init_events[0]

        # Check publisher init order (rmw then rcl)
        self.assertEventOrder([
            ping_rmw_pub_init_event,
            ping_publisher_init_event,
        ])
        self.assertEventOrder([
            pong_rmw_pub_init_event,
            pong_publisher_init_event,
        ])

        # Get corresponding rmw/rcl/rclcpp publish events for ping & pong
        rcl_publish_events = self.get_events_with_name(tp.rcl_publish)
        ping_rcl_pub_events = self.get_events_with_field_value(
            'publisher_handle',
            ping_pub_handle,
            rcl_publish_events,
        )
        pong_rcl_pub_events = self.get_events_with_field_value(
            'publisher_handle',
            pong_pub_handle,
            rcl_publish_events,
        )
        self.assertNumEventsEqual(ping_rcl_pub_events, 1)
        self.assertNumEventsEqual(pong_rcl_pub_events, 1)
        ping_rcl_pub_event = ping_rcl_pub_events[0]
        pong_rcl_pub_event = pong_rcl_pub_events[0]

        rclcpp_publish_events = self.get_events_with_name(tp.rclcpp_publish)
        rmw_publish_events = self.get_events_with_name(tp.rmw_publish)
        ping_pub_message = self.get_field(ping_rcl_pub_event, 'message')
        pong_pub_message = self.get_field(pong_rcl_pub_event, 'message')

        ping_rclcpp_pub_events = self.get_events_with_field_value(
            'message',
            ping_pub_message,
            rclcpp_publish_events,
        )
        pong_rclcpp_pub_events = self.get_events_with_field_value(
            'message',
            pong_pub_message,
            rclcpp_publish_events,
        )
        ping_rmw_pub_events = self.get_events_with_field_value(
            'message',
            ping_pub_message,
            rmw_publish_events,
        )
        pong_rmw_pub_events = self.get_events_with_field_value(
            'message',
            pong_pub_message,
            rmw_publish_events,
        )
        self.assertNumEventsEqual(ping_rclcpp_pub_events, 1)
        self.assertNumEventsEqual(pong_rclcpp_pub_events, 1)
        self.assertNumEventsEqual(ping_rmw_pub_events, 1)
        self.assertNumEventsEqual(pong_rmw_pub_events, 1)
        ping_rclcpp_pub_event = ping_rclcpp_pub_events[0]
        pong_rclcpp_pub_event = pong_rclcpp_pub_events[0]
        ping_rmw_pub_event = ping_rmw_pub_events[0]
        pong_rmw_pub_event = pong_rmw_pub_events[0]

        # Get subscription init events & subscription handles of test topics
        rcl_subscription_init_events = self.get_events_with_name(tp.rcl_subscription_init)
        ping_rcl_subscription_init_events = self.get_events_with_field_value(
            'topic_name',
            '/ping',
            rcl_subscription_init_events,
        )
        pong_rcl_subscription_init_events = self.get_events_with_field_value(
            'topic_name',
            '/pong',
            rcl_subscription_init_events,
        )
        self.assertNumEventsEqual(ping_rcl_subscription_init_events, 1)
        self.assertNumEventsEqual(pong_rcl_subscription_init_events, 1)
        ping_rcl_subscription_init_event = ping_rcl_subscription_init_events[0]
        pong_rcl_subscription_init_event = pong_rcl_subscription_init_events[0]
        ping_sub_handle = self.get_field(ping_rcl_subscription_init_event, 'subscription_handle')
        ping_rmw_sub_handle = self.get_field(
            ping_rcl_subscription_init_event, 'rmw_subscription_handle')
        pong_sub_handle = self.get_field(pong_rcl_subscription_init_event, 'subscription_handle')
        pong_rmw_sub_handle = self.get_field(
            pong_rcl_subscription_init_event, 'rmw_subscription_handle')

        # Find corresponding rmw_sub_init events
        ping_rmw_sub_init_events = self.get_events_with_field_value(
            'rmw_subscription_handle',
            ping_rmw_sub_handle,
            rmw_sub_init_events,
        )
        pong_rmw_sub_init_events = self.get_events_with_field_value(
            'rmw_subscription_handle',
            pong_rmw_sub_handle,
            rmw_sub_init_events,
        )
        self.assertNumEventsEqual(ping_rmw_sub_init_events, 1)
        self.assertNumEventsEqual(pong_rmw_sub_init_events, 1)
        ping_rmw_sub_init_event = ping_rmw_sub_init_events[0]
        pong_rmw_sub_init_event = pong_rmw_sub_init_events[0]

        # Get corresponding subscription objects
        rclcpp_subscription_init_events = self.get_events_with_name(
            tp.rclcpp_subscription_init,
        )
        ping_rclcpp_subscription_init_events = self.get_events_with_field_value(
            'subscription_handle',
            ping_sub_handle,
            rclcpp_subscription_init_events,
        )
        pong_rclcpp_subscription_init_events = self.get_events_with_field_value(
            'subscription_handle',
            pong_sub_handle,
            rclcpp_subscription_init_events,
        )
        self.assertNumEventsEqual(ping_rclcpp_subscription_init_events, 1)
        self.assertNumEventsEqual(pong_rclcpp_subscription_init_events, 1)
        ping_rclcpp_subscription_init_event = ping_rclcpp_subscription_init_events[0]
        pong_rclcpp_subscription_init_event = pong_rclcpp_subscription_init_events[0]
        ping_sub_object = self.get_field(ping_rclcpp_subscription_init_event, 'subscription')
        pong_sub_object = self.get_field(pong_rclcpp_subscription_init_event, 'subscription')

        # Get corresponding subscription callback objects
        rclcpp_subscription_callback_events = self.get_events_with_name(
            tp.rclcpp_subscription_callback_added,
        )
        ping_rclcpp_subscription_callback_events = self.get_events_with_field_value(
            'subscription',
            ping_sub_object,
            rclcpp_subscription_callback_events,
        )
        pong_rclcpp_subscription_callback_events = self.get_events_with_field_value(
            'subscription',
            pong_sub_object,
            rclcpp_subscription_callback_events,
        )
        self.assertNumEventsEqual(ping_rclcpp_subscription_callback_events, 1)
        self.assertNumEventsEqual(pong_rclcpp_subscription_callback_events, 1)
        ping_rclcpp_subscription_callback_event = ping_rclcpp_subscription_callback_events[0]
        pong_rclcpp_subscription_callback_event = pong_rclcpp_subscription_callback_events[0]
        ping_callback_object = self.get_field(ping_rclcpp_subscription_callback_event, 'callback')
        pong_callback_object = self.get_field(pong_rclcpp_subscription_callback_event, 'callback')

        # Check subscription init order
        self.assertEventOrder([
            ping_rmw_sub_init_event,
            ping_rcl_subscription_init_event,
            ping_rclcpp_subscription_init_event,
            ping_rclcpp_subscription_callback_event,
        ])
        self.assertEventOrder([
            pong_rmw_sub_init_event,
            pong_rcl_subscription_init_event,
            pong_rclcpp_subscription_init_event,
            pong_rclcpp_subscription_callback_event,
        ])

        # Get corresponding callback start/end events
        callback_start_events = self.get_events_with_name(tp.callback_start)
        callback_end_events = self.get_events_with_name(tp.callback_end)
        ping_callback_start_events = self.get_events_with_field_value(
            'callback',
            ping_callback_object,
            callback_start_events,
        )
        pong_callback_start_events = self.get_events_with_field_value(
            'callback',
            pong_callback_object,
            callback_start_events,
        )
        ping_callback_end_events = self.get_events_with_field_value(
            'callback',
            ping_callback_object,
            callback_end_events,
        )
        pong_callback_end_events = self.get_events_with_field_value(
            'callback',
            pong_callback_object,
            callback_end_events,
        )
        self.assertNumEventsEqual(ping_callback_start_events, 1)
        self.assertNumEventsEqual(pong_callback_start_events, 1)
        self.assertNumEventsEqual(ping_callback_end_events, 1)
        self.assertNumEventsEqual(pong_callback_end_events, 1)
        ping_callback_start_event = ping_callback_start_events[0]
        pong_callback_start_event = pong_callback_start_events[0]
        ping_callback_end_event = ping_callback_end_events[0]
        pong_callback_end_event = pong_callback_end_events[0]

        # Check pub/sub order:
        #   * /ping pub rclcpp_publish
        #   * /ping pub rcl_publish
        #   * /ping pub rmw_publish
        #   * /ping sub callback_start
        #   * /pong pub rclcpp_publish
        #   * /pong pub rcl_publish
        #   * /pong pub rmw_publish
        #   ...
        #   * /ping sub callback_end
        #   ... we shouldn't necessarily expect the /pong callback to start
        #       before the /ping callback has ended
        #   * /pong sub callback_start
        #   * /pong sub callback_end
        self.assertEventOrder([
            ping_rclcpp_pub_event,
            ping_rcl_pub_event,
            ping_rmw_pub_event,
            ping_callback_start_event,
            pong_rclcpp_pub_event,
            pong_rcl_pub_event,
            pong_rmw_pub_event,
            ping_callback_end_event,
        ])
        self.assertEventOrder([
            pong_rclcpp_pub_event,
            pong_rcl_pub_event,
            pong_rmw_pub_event,
            pong_callback_start_event,
            pong_callback_end_event,
        ])


if __name__ == '__main__':
    unittest.main()
