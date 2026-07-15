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


class TestIntra(TraceTestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
            session_name_prefix='session-test-intra',
            events_ros=[
                tp.rcl_subscription_init,
                tp.rclcpp_subscription_init,
                tp.rclcpp_subscription_callback_added,
                tp.callback_start,
                tp.callback_end,
            ],
            package='test_tracetools',
            nodes=['test_intra'],
        )

    def test_all(self):
        # Check events as set
        self.assertEventsSet(self._events_ros)

        print('EVENTS: ', self._events)

        # Check rcl_subscription_init events
        rcl_sub_init_events = self.get_events_with_name(tp.rcl_subscription_init)
        rcl_sub_init_topic_events = self.get_events_with_field_value(
            'topic_name',
            '/the_topic',
            rcl_sub_init_events,
        )
        # Only 1 for our given topic
        self.assertNumEventsEqual(
            rcl_sub_init_topic_events,
            1,
            'none or more than 1 rcl_sub_init event for the topic',
        )

        # Get subscription handle
        rcl_sub_init_topic_event = rcl_sub_init_topic_events[0]
        sub_handle_intra = self.get_field(rcl_sub_init_topic_event, 'subscription_handle')

        # Check rclcpp_subscription_init events
        rclcpp_sub_init_events = self.get_events_with_name(tp.rclcpp_subscription_init)
        rclcpp_sub_init_topic_events = self.get_events_with_field_value(
            'subscription_handle',
            sub_handle_intra,
            rclcpp_sub_init_events,
        )
        # Should have 2 events for the given subscription handle
        # (Subscription and SubscriptionIntraProcess)
        self.assertNumEventsEqual(
            rclcpp_sub_init_topic_events,
            2,
            'number of rclcpp_sub_init events for the topic not equal to 2',
        )

        # Get the 2 subscription pointers
        events = rclcpp_sub_init_topic_events
        subscription_pointers = [self.get_field(e, 'subscription') for e in events]

        # Get the corresponding callback pointers
        rclcpp_sub_callback_added_events = self.get_events_with_name(
            tp.rclcpp_subscription_callback_added,
        )
        rclcpp_sub_callback_added_topic_events = self.get_events_with_field_value(
            'subscription',
            subscription_pointers,
            rclcpp_sub_callback_added_events,
        )
        events = rclcpp_sub_callback_added_topic_events
        callback_pointers = [self.get_field(e, 'callback') for e in events]

        # Get corresponding callback start/end pairs
        start_events = self.get_events_with_name(tp.callback_start)
        end_events = self.get_events_with_name(tp.callback_end)
        # Should have at least one start:end pair
        self.assertNumEventsGreaterEqual(
            start_events,
            1,
            'does not have at least 1 callback start event')
        self.assertNumEventsGreaterEqual(
            end_events,
            1,
            'does not have at least 1 callback end event')
        start_events_topic = self.get_events_with_field_value(
            'callback',
            callback_pointers,
            start_events)
        end_events_topic = self.get_events_with_field_value(
            'callback',
            callback_pointers,
            end_events)
        self.assertNumEventsGreaterEqual(
            start_events_topic,
            1,
            'no callback_start event found for topic')
        self.assertNumEventsGreaterEqual(
            end_events_topic,
            1,
            'no callback_end found event for topic')

        # Check for is_intra_process field value of 1/true
        start_events_intra = self.get_events_with_field_value(
            'is_intra_process',
            1,
            start_events_topic,
        )
        # Should have one event
        self.assertNumEventsEqual(
            start_events_intra,
            1,
            'none or more than 1 callback_start event for intra-process topic',
        )
        # As a sanity check, make sure its callback pointer has a corresponding callback_end event
        callback_pointer_intra = self.get_field(start_events_intra[0], 'callback')
        end_events_intra = self.get_events_with_field_value(
            'callback',
            callback_pointer_intra,
            end_events_topic,
        )
        self.assertNumEventsEqual(
            end_events_intra,
            1,
            'none or more than 1 callback_end event for intra-process topic',
        )


if __name__ == '__main__':
    unittest.main()
