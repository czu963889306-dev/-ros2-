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

"""Tests for the trace directory logic."""

import os
import pathlib

from tracetools_trace.tools.path import get_tracing_directory


def test_get_trace_directory():
    os.environ.pop('ROS_TRACE_DIR', None)
    os.environ.pop('ROS_HOME', None)
    home = pathlib.Path.home()
    assert str(home)

    # Default case without ROS_TRACE_DIR or ROS_HOME being set (but with HOME)
    default_dir = str(home / '.ros/tracing')
    assert get_tracing_directory() == default_dir

    # Use $ROS_TRACE_DIR if it is set
    my_trace_dir_raw = '/my/ROS_TRACE_DIR'
    my_trace_dir = str(pathlib.Path(my_trace_dir_raw))
    os.environ['ROS_TRACE_DIR'] = my_trace_dir
    assert get_tracing_directory() == my_trace_dir
    # Make sure it converts path separators when necessary
    os.environ['ROS_TRACE_DIR'] = my_trace_dir_raw
    assert get_tracing_directory() == my_trace_dir
    # Setting ROS_HOME won't change anything since ROS_TRACE_DIR is used first
    os.environ['ROS_HOME'] = '/this/wont/be/used'
    assert get_tracing_directory() == my_trace_dir
    os.environ.pop('ROS_HOME', None)
    # Empty is considered unset
    os.environ['ROS_TRACE_DIR'] = ''
    assert get_tracing_directory() == default_dir
    # Make sure '~' is expanded to the home directory
    os.environ['ROS_TRACE_DIR'] = '~/tracedir'
    assert get_tracing_directory() == str(home / 'tracedir')

    os.environ.pop('ROS_TRACE_DIR', None)

    # Without ROS_TRACE_DIR, use $ROS_HOME/tracing
    fake_ros_home = home / '.fakeroshome'
    fake_ros_home_trace_dir = str(fake_ros_home / 'tracing')
    os.environ['ROS_HOME'] = str(fake_ros_home)
    assert get_tracing_directory() == fake_ros_home_trace_dir
    # Make sure it converts path separators when necessary
    my_ros_home_raw = '/my/ros/home'
    my_ros_home_trace_dir = str(pathlib.Path(my_ros_home_raw) / 'tracing')
    os.environ['ROS_HOME'] = my_ros_home_raw
    assert get_tracing_directory() == my_ros_home_trace_dir
    # Empty is considered unset
    os.environ['ROS_HOME'] = ''
    assert get_tracing_directory() == default_dir
    # Make sure '~' is expanded to the home directory
    os.environ['ROS_HOME'] = '~/.fakeroshome'
    assert get_tracing_directory() == fake_ros_home_trace_dir

    os.environ.pop('ROS_HOME', None)
