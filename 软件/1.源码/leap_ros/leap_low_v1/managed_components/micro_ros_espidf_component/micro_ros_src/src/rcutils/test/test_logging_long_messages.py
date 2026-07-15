# Copyright 2017 Open Source Robotics Foundation, Inc.
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

import os
import unittest

from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch.actions import SetEnvironmentVariable

import launch_testing
import launch_testing.actions
import launch_testing.asserts
import launch_testing.markers


@launch_testing.markers.keep_alive
def generate_test_description():
    launch_description = LaunchDescription()
    # Set the output format to a "verbose" format that is expected by the executable output
    launch_description.add_action(
        SetEnvironmentVariable(
            name='RCUTILS_CONSOLE_OUTPUT_FORMAT',
            value='[{severity}] [{name}]: {message} '
                  '({function_name}() at {file_name}:{line_number})'
        )
    )
    executable = os.path.join(os.getcwd(), 'test_logging_long_messages')
    if os.name == 'nt':
        executable += '.exe'
    process_name = 'test_logging_long_messages'
    launch_description.add_action(
        ExecuteProcess(cmd=[executable], name=process_name, output='screen')
    )

    launch_description.add_action(
        launch_testing.actions.ReadyToTest()
    )
    return launch_description, {'process_name': process_name}


class TestLoggingLongMessages(unittest.TestCase):

    def test_logging_output(self, proc_output, process_name):
        """Test executable output against expectation."""
        proc_output.assertWaitFor(
            expected_output=launch_testing.tools.expected_output_from_file(
                path=os.path.join(os.path.dirname(__file__), process_name)
            ), process=process_name, timeout=10
        )
