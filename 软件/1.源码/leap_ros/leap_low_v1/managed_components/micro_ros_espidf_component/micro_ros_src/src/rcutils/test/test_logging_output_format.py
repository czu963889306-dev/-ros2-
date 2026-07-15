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

import launch_testing
import launch_testing.actions
import launch_testing.asserts
import launch_testing.markers


@launch_testing.markers.keep_alive
def generate_test_description():
    processes_to_test = []

    launch_description = LaunchDescription()
    # Re-use the test_logging_long_messages test binary and modify the output format from an
    # environment variable.
    executable = os.path.join(os.getcwd(), 'test_logging_long_messages')
    if os.name == 'nt':
        executable += '.exe'
    env_long = dict(os.environ)
    # In this custom output, the long message is output twice, to test both dynamic allocation and
    # re-allocation.
    env_long['RCUTILS_CONSOLE_OUTPUT_FORMAT'] = \
        '[{{name}}].({severity}) output: {file_name}:{line_number} {message}, again: {message} ({function_name}()){'  # noqa
    name = 'test_logging_output_format_long'
    launch_description.add_action(ExecuteProcess(
        cmd=[executable], env=env_long, name=name, output='screen'
    ))
    processes_to_test.append(name)

    env_edge_cases = dict(os.environ)
    # This custom output is to check different edge cases of the output format string parsing.
    env_edge_cases['RCUTILS_CONSOLE_OUTPUT_FORMAT'] = '{}}].({unknown_token}) {{{{'
    name = 'test_logging_output_format_edge_cases'
    launch_description.add_action(ExecuteProcess(
        cmd=[executable], env=env_edge_cases, name=name, output='screen'
    ))
    processes_to_test.append(name)

    env_no_tokens = dict(os.environ)
    # This custom output is to check that there are no issues when no tokens are used.
    env_no_tokens['RCUTILS_CONSOLE_OUTPUT_FORMAT'] = 'no_tokens'
    name = 'test_logging_output_format_no_tokens'
    launch_description.add_action(ExecuteProcess(
        cmd=[executable], env=env_no_tokens, name=name, output='screen'
    ))
    processes_to_test.append(name)

    env_time_tokens = dict(os.environ)
    # This custom output is to check that time stamps work correctly
    env_time_tokens['RCUTILS_CONSOLE_OUTPUT_FORMAT'] = "'{time}' '{time_as_nanoseconds}'"
    name = 'test_logging_output_timestamps'
    launch_description.add_action(ExecuteProcess(
        cmd=[executable], env=env_time_tokens, name=name, output='screen'
    ))
    processes_to_test.append(name)

    launch_description.add_action(
        launch_testing.actions.ReadyToTest()
    )

    return launch_description, {'processes_to_test': processes_to_test}


class TestLoggingOutputFormat(unittest.TestCase):

    def test_logging_output(self, proc_info, proc_output, processes_to_test):
        for process_name in processes_to_test:
            proc_info.assertWaitForShutdown(process=process_name, timeout=10)


@launch_testing.post_shutdown_test()
class TestLoggingOutputFormatAfterShutdown(unittest.TestCase):

    def test_logging_output(self, proc_output, processes_to_test):
        """Test all executables output against expectations."""
        for process_name in processes_to_test:
            launch_testing.asserts.assertInStderr(
                proc_output,
                expected_output=launch_testing.tools.expected_output_from_file(
                    path=os.path.join(os.path.dirname(__file__), process_name)
                ),
                process=process_name
            )

    def test_processes_exit_codes(self, proc_info):
        """Test that all executables finished cleanly."""
        launch_testing.asserts.assertExitCodes(proc_info)
