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

"""Module with trace-reading utilities."""

from typing import Any
from typing import Dict


DictEvent = Dict[str, Any]


def get_field(
    event: DictEvent,
    field_name: str,
    default: Any = None,
    raise_if_not_found: bool = True,
) -> Any:
    """
    Get value of a field from an event.

    Can return a custom default value if not found. Will raise `AttributeError` by default if not
    found, but it can be suppressed. These two options cannot be used together.

    :param event: the event
    :param field_name: the name of the field
    :param default: the value to use if not found
    :param raise_if_not_found: whether to raise an error the field is not found
    :return: `None` (or default value) if not found
    """
    field_value = event.get(field_name, default)
    # If enabled, raise exception as soon as possible to avoid headaches
    if raise_if_not_found and field_value is None:
        raise AttributeError(f"event field '{field_name}' not found for event: {event}")
    return field_value


def get_event_name(event: DictEvent) -> str:
    return event['_name']


def get_event_timestamp(event: DictEvent) -> int:
    return event['_timestamp']


def get_procname(event: DictEvent) -> str:
    return event['procname']
