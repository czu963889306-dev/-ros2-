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

"""Module with functions for reading traces."""

import os
from typing import Iterable
from typing import List

import babeltrace

from . import DictEvent


def is_trace_directory(path: str) -> bool:
    """
    Check recursively if a path is a trace directory.

    :param path: the path to check
    :return: `True` if it is a trace directory, `False` otherwise
    """
    path = os.path.expanduser(path)
    if not os.path.isdir(path):
        return False
    tc = babeltrace.TraceCollection()
    # Could still return an empty dict even if it is not a trace directory (recursively)
    traces = tc.add_traces_recursive(path, 'ctf')
    return traces is not None and len(traces) > 0


def get_trace_ctf_events(trace_directory: str) -> Iterable[babeltrace.babeltrace.Event]:
    """
    Get the events of a trace.

    :param trace_directory: the path to the main/top trace directory
    :return: events iterable
    """
    tc = babeltrace.TraceCollection()
    tc.add_traces_recursive(trace_directory, 'ctf')
    return tc.events


def get_trace_events(trace_directory: str) -> List[DictEvent]:
    """
    Get the events of a trace.

    :param trace_directory: the path to the main/top trace directory
    :return: events
    """
    events: List[DictEvent] = [
        event_to_dict(event) for event in get_trace_ctf_events(trace_directory)
    ]
    return events


# List of ignored CTF fields
_IGNORED_FIELDS = [
    'content_size',
    'events_discarded',
    'id',
    'packet_size',
    'packet_seq_num',
    'stream_id',
    'stream_instance_id',
    'timestamp_end',
    'timestamp_begin',
    'magic',
    'uuid',
    'v',
]
_DISCARD = 'events_discarded'


def event_to_dict(event: babeltrace.babeltrace.Event) -> DictEvent:
    """
    Convert name, timestamp, and all other keys except those in IGNORED_FIELDS into a dictionary.

    :param event: the event to convert
    :return: the event as a dictionary
    """
    if hasattr(event, _DISCARD) and event[_DISCARD] > 0:
        print(event[_DISCARD])
    meta = {'_name': event.name, '_timestamp': event.timestamp}
    data = {key: event[key] for key in event.keys() if key not in _IGNORED_FIELDS}
    return {**meta, **data}
