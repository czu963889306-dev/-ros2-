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

"""Utils for tracetools_test that are not strictly test-related."""

import os
import shutil
from typing import List
from typing import Tuple
from typing import Union

from launch import Action
from launch import LaunchDescription
from launch import LaunchService
from launch_ros.actions import Node
from tracetools_launch.action import Trace
from tracetools_read import DictEvent
from tracetools_read import get_event_name
from tracetools_trace.tools.path import append_timestamp


def run_and_trace(
    base_path: str,
    session_name_prefix: str,
    ros_events: List[str],
    kernel_events: List[str],
    package_name: str,
    node_names: List[str],
    additional_actions: Union[List[Action], Action] = [],
) -> Tuple[int, str]:
    """
    Run a node while tracing.

    :param base_path: the base path where to put the trace directory
    :param session_name_prefix: the session name prefix for the trace directory
    :param ros_events: the list of ROS UST events to enable
    :param kernel_events: the list of kernel events to enable
    :param package_name: the name of the package to use
    :param node_names: the names of the nodes to execute
    :param additional_actions: the list of additional actions to prepend
    :return: exit code, full generated path
    """
    session_name = append_timestamp(session_name_prefix)
    full_path = os.path.join(base_path, session_name)
    if not isinstance(additional_actions, list):
        additional_actions = [additional_actions]

    launch_actions = additional_actions
    # Add trace action
    launch_actions.append(
        Trace(
            session_name=session_name,
            append_timestamp=False,
            base_path=base_path,
            events_ust=ros_events,
            events_kernel=kernel_events,
        )
    )
    # Add nodes
    for node_name in node_names:
        n = Node(
            package=package_name,
            executable=node_name,
            output='screen',
        )
        launch_actions.append(n)
    ld = LaunchDescription(launch_actions)
    ls = LaunchService()
    ls.include_launch_description(ld)

    exit_code = ls.run()

    return exit_code, full_path


def cleanup_trace(full_path: str) -> None:
    """
    Cleanup trace data.

    :param full_path: the full path to the main trace directory
    """
    shutil.rmtree(full_path)


def get_event_names(events: List[DictEvent]) -> List[str]:
    """
    Get a list of events names from a list of events.

    :param events: the events of the trace
    :return: the list of event names
    """
    return [get_event_name(e) for e in events]
