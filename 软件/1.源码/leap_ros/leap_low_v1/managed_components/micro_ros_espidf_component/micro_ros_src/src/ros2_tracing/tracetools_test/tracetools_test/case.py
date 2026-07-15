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

"""Module for a tracing-specific unittest.TestCase extension."""

import os
import time
from typing import Any
from typing import List
from typing import Optional
from typing import Type
from typing import Union
import unittest

from launch import Action
from tracetools_read import DictEvent
from tracetools_read import get_event_name
from tracetools_read import get_event_timestamp
from tracetools_read import get_field
from tracetools_read import get_procname
from tracetools_read.trace import get_trace_events

from .utils import cleanup_trace
from .utils import get_event_names
from .utils import run_and_trace


class TraceTestCase(unittest.TestCase):
    """
    TestCase extension for tests on a trace.

    Sets up tracing, traces given nodes, and provides
    the resulting events for an extending class to test on.
    It also does some basic checks on the resulting trace.

    If the TRACETOOLS_TEST_DEBUG environment variable is set (and
    non-empty), the resulting trace will not be removed after the test.

    The assert*() functions are specifically in mixedCase to match the unittest functions.
    """

    ENV_VAR_DEBUG = 'TRACETOOLS_TEST_DEBUG'

    def __init__(
        self,
        *args,
        session_name_prefix: str,
        events_ros: List[str],
        package: str,
        nodes: List[str],
        base_path: str = '/tmp',
        events_kernel: List[str] = [],
        additional_actions: Union[List[Action], Action] = [],
    ) -> None:
        """Create a TraceTestCase."""
        super().__init__(methodName=args[0])
        self._base_path = base_path
        self._session_name_prefix = session_name_prefix
        self._events_ros = events_ros
        self._events_kernel = events_kernel
        self._package = package
        self._nodes = nodes
        self._additional_actions = additional_actions

    def setUp(self):
        # Get timestamp before trace (ns)
        timestamp_before = int(time.time() * 1000000000.0)

        exit_code, full_path = run_and_trace(
            self._base_path,
            self._session_name_prefix,
            self._events_ros,
            self._events_kernel,
            self._package,
            self._nodes,
            self._additional_actions,
        )

        print(f'TRACE DIRECTORY: {full_path}')
        self._exit_code = exit_code
        self._full_path = full_path

        # Check that setUp() ran fine
        self.assertEqual(self._exit_code, 0)

        # Read events once
        self._events = get_trace_events(self._full_path)
        self._event_names = get_event_names(self._events)
        self.assertGreater(len(self._events), 0, 'no events found in trace')

        # Check the timestamp of the first event
        self.assertEventAfterTimestamp(self._events[0], timestamp_before)

        # Check that the enabled events are present
        ros = set(self._events_ros) if self._events_ros is not None else set()
        kernel = set(self._events_kernel) if self._events_kernel is not None else set()
        all_event_names = ros | kernel
        self.assertSetEqual(all_event_names, set(self._event_names))

        # Check that the launched nodes are present as processes
        self.assertProcessNamesExist(self._nodes)

    def tearDown(self):
        if not os.environ.get(self.ENV_VAR_DEBUG, None):
            cleanup_trace(self._full_path)

    def assertEventsSet(  # noqa: N802
        self,
        event_names: List[str],
    ) -> None:
        """
        Compare given event names to trace events names as sets.

        :param event_names: the list of event names to compare to (as a set)
        """
        self.assertSetEqual(set(self._event_names), set(event_names), 'wrong events')

    def assertProcessNamesExist(  # noqa: N802
        self,
        names: List[str],
    ) -> None:
        """
        Check that the given processes exist.

        :param names: the node names to look for
        """
        procnames = [get_procname(e) for e in self._events]
        for name in names:
            # Procnames have a max length of 15
            name_trimmed = name[:15]
            self.assertTrue(name_trimmed in procnames, 'node name not found in tracepoints')

    def assertFieldType(  # noqa: N802
        self,
        event: DictEvent,
        field_names: Union[str, List[str]],
        field_type: Type,
    ) -> None:
        """
        Check that the type of a field value corresponds to the given type.

        :param event: the event
        :param field_names: the name(s) of field(s) to check
        :param field_type: the expected field value type
        """
        if not isinstance(field_names, list):
            field_names = [field_names]
        for field_name in field_names:
            field_value = self.get_field(event, field_name)
            self.assertIsInstance(
                field_value,
                field_type,
                f'expected {field_name} field type {field_type.__name__}, '
                f'got {type(field_value).__name__}')

    def assertValidHandle(  # noqa: N802
        self,
        event: DictEvent,
        handle_field_names: Union[str, List[str]],
    ) -> None:
        """
        Check that the handle associated to a field name is valid.

        :param event: the event which has a handle field
        :param handle_field_names: the handle field name(s) to check
        """
        self.assertValidPointer(
            event,
            handle_field_names,
        )

    def assertValidPointer(  # noqa: N802
        self,
        event: DictEvent,
        pointer_field_names: Union[str, List[str]],
    ) -> None:
        """
        Check that the pointer associated to a field name is valid.

        :param event: the event which has a pointer field
        :param pointer_field_names: the pointer field name(s) to check
        """
        if not isinstance(pointer_field_names, list):
            pointer_field_names = [pointer_field_names]
        for field_name in pointer_field_names:
            pointer_value = self.get_field(event, field_name)
            self.assertIsInstance(pointer_value, int, 'pointer value not int')
            self.assertGreater(pointer_value, 0, f'invalid pointer value: {field_name}')

    def assertValidArray(  # noqa: N802
        self,
        event: DictEvent,
        array_field_names: Union[str, List[str]],
        array_type: Optional[Type] = None,
    ) -> None:
        """
        Check that the array associated to a field name is valid.

        Optionally check the type of its elements.

        :param event: the event which has has an array field
        :param array_field_names: the aray field name(s) to check
        :param array_type: the expected type of elements in the array
        """
        if not isinstance(array_field_names, list):
            array_field_names = [array_field_names]
        for field_name in array_field_names:
            array_value = self.get_field(event, field_name)
            self.assertIsInstance(
                array_value,
                list,
                f'{field_name} value not array: {array_value}')
            if array_type and len(array_value) > 0:
                self.assertIsInstance(
                    array_value[0],
                    array_type,
                    f'{field_name} array element not {array_type.__name__}: {array_value}')

    def assertValidQueueDepth(  # noqa: N802
        self,
        event: DictEvent,
        queue_depth_field_name: str = 'queue_depth',
    ) -> None:
        """
        Check that the queue depth value is valid.

        :param event: the event with the queue depth field
        :param queue_depth_field_name: the field name for queue depth
        """
        queue_depth_value = self.get_field(event, queue_depth_field_name)
        self.assertIsInstance(queue_depth_value, int, 'invalid queue depth type')
        self.assertGreater(queue_depth_value, 0, 'invalid queue depth')

    def assertStringFieldNotEmpty(  # noqa: N802
        self,
        event: DictEvent,
        string_field_name: str,
    ) -> None:
        """
        Check that a string field is not empty.

        :param event: the event which has a string field
        :param string_field_name: the field name of the string field
        """
        string_field = self.get_field(event, string_field_name)
        self.assertGreater(len(string_field), 0, 'empty string')

    def assertEventAfterTimestamp(  # noqa: N802
        self,
        event: DictEvent,
        timestamp: int,
    ) -> None:
        """
        Check that the event happens after the given timestamp.

        :param event: the event to check
        :param timestamp: the reference timestamp
        """
        self.assertGreater(get_event_timestamp(event), timestamp, 'event not after timestamp')

    def assertEventOrder(  # noqa: N802
        self,
        events: List[DictEvent],
    ) -> None:
        """
        Check that the first event was generated before the second event.

        :param events: the events in the expected order
        """
        self.assertTrue(self.are_events_ordered(events), 'unexpected events order')

    def assertNumEventsEqual(  # noqa: N802
        self,
        events: List[DictEvent],
        expected_number: int,
        msg: str = 'wrong number of events',
    ) -> None:
        """
        Check number of events.

        :param events: the events to check
        :param expected_number: the expected number of events
        :param msg: the message to display on failure
        """
        self.assertEqual(len(events), expected_number, msg)

    def assertNumEventsGreaterEqual(  # noqa: N802
        self,
        events: List[DictEvent],
        min_expected_number: int,
        msg: str = 'wrong number of events',
    ) -> None:
        """
        Check that the number of events is greater of equal.

        :param events: the events to check
        :param min_expected_number: the minimum expected number of events
        :param msg: the message to display on failure
        """
        self.assertGreaterEqual(len(events), min_expected_number, msg)

    def assertMatchingField(  # noqa: N802
        self,
        initial_event: DictEvent,
        field_name: str,
        matching_event_name: str = None,
        events: List[DictEvent] = None,
        check_order: bool = True,
    ) -> None:
        """
        Check that the value of a field for a given event has a matching event that follows.

        :param initial_event: the first event, which is the origin of the common field value
        :param field_name: the name of the common field to check
        :param matching_event_name: the name of the event to check (or `None` to check all)
        :param events: the events to check (or `None` to check all in trace)
        :param check_order: whether to check that the matching event comes after the initial event
        """
        if events is None:
            events = self._events
        if matching_event_name is not None:
            events = self.get_events_with_name(matching_event_name, events)
        field_value = self.get_field(initial_event, field_name)

        # Get events with that handle
        matches = self.get_events_with_field_value(
            field_name,
            field_value,
            events)
        # Check that there is at least one
        self.assertGreaterEqual(
            len(matches),
            1,
            f'no corresponding {field_name}')
        if check_order:
            # Check order
            # Since matching pairs might repeat, we need to check
            # that there is at least one match that comes after
            matches_ordered = [e for e in matches if self.are_events_ordered([initial_event, e])]
            self.assertGreaterEqual(
                len(matches_ordered),
                1,
                'matching field event not after initial event')

    def assertFieldEquals(  # noqa: N802
        self,
        event: DictEvent,
        field_name: str,
        value: Any,
        msg: str = 'wrong field value',
    ) -> None:
        """
        Check the value of a field.

        :param event: the event
        :param field_name: the name of the field to check
        :param value: to value to compare the field value to
        :param msg: the message to display on failure
        """
        actual_value = self.get_field(event, field_name)
        self.assertEqual(actual_value, value, msg)

    def get_field(
        self,
        event: DictEvent,
        field_name: str,
    ) -> Any:
        """
        Get field value; will fail test if not found.

        :param event: the event from which to get the value
        :param field_name: the field name
        :return: the value
        """
        try:
            value = get_field(event, field_name, default=None, raise_if_not_found=True)
        except AttributeError as e:
            # Explicitly failing here
            self.fail(str(e))
        else:
            return value

    def get_procname(
        self,
        event: DictEvent,
    ) -> str:
        """
        Get procname.

        :param event: the event
        :return: the procname of the event
        """
        return get_procname(event)

    def get_events_with_name(
        self,
        event_name: str,
        events: List[DictEvent] = None,
    ) -> List[DictEvent]:
        """
        Get all events with the given name.

        :param event_name: the event name
        :param events: the events to check (or `None` to check all events)
        :return: the list of events with the given name
        """
        if events is None:
            events = self._events
        return [e for e in events if get_event_name(e) == event_name]

    def get_events_with_procname(
        self,
        procname: str,
        events: List[DictEvent] = None,
    ) -> List[DictEvent]:
        """
        Get all events with the given procname.

        Note: the given procname value will be truncated to the same max length as the procname
        field.

        :param procname: the procname
        :param events: the events to check (or `None` to check all events)
        :return: the events with the given procname
        """
        if events is None:
            events = self._events
        return [e for e in events if self.get_procname(e) == procname[:15]]

    def get_events_with_field_value(
        self,
        field_name: str,
        field_values: Any,
        events: List[DictEvent] = None,
    ) -> List[DictEvent]:
        """
        Get all events with the given field:value.

        :param field_name: the name of the field to check
        :param field_values: the value(s) of the field to check
        :param events: the events to check (or `None` to check all events)
        :return: the events with the given field:value pair
        """
        if not isinstance(field_values, list):
            field_values = [field_values]
        if events is None:
            events = self._events
        return [e for e in events if get_field(e, field_name, None) in field_values]

    def get_events_with_field_not_value(
        self,
        field_name: str,
        field_values: Any,
        events: List[DictEvent] = None,
    ) -> List[DictEvent]:
        """
        Get all events with the given field but not the value.

        :param field_name: the name of the field to check
        :param field_values: the value(s) of the field to check
        :param events: the events to check (or `None` to check all events)
        :return: the events with the given field:value pair
        """
        if not isinstance(field_values, list):
            field_values = [field_values]
        if events is None:
            events = self._events
        return [e for e in events if get_field(e, field_name, None) not in field_values]

    def are_events_ordered(
        self,
        events: List[DictEvent],
    ) -> bool:
        """
        Check that the events were generated in the expected order.

        :param events: the events in the expected order
        """
        orders = [
            get_event_timestamp(events[i]) < get_event_timestamp(events[i + 1])
            for i in range(len(events) - 1)
        ]
        return all(orders)
