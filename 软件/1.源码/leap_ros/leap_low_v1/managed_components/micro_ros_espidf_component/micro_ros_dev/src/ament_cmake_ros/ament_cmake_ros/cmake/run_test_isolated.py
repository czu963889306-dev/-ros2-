#!/usr/bin/env python3

# Copyright 2019 Apex.AI, Inc.
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

import contextlib
import os
import sys

import ament_cmake_test
import domain_coordinator


if __name__ == '__main__':
    # If ROS_DOMAIN_ID is already set, respect that domain ID and use it.
    # If ROS_DOMAIN_ID is not set, pick a ROS_DOMAIN_ID that's not being used
    # by another run_test_isolated process.
    # This is to allow tests to run in parallel and not have ROS cross-talk.
    # If the user needs to debug a test and they don't have ROS_DOMAIN_ID set in their environment
    # they can disable isolation by setting the DISABLE_ROS_ISOLATION environment variable.
    with contextlib.ExitStack() as stack:
        if 'ROS_DOMAIN_ID' not in os.environ and 'DISABLE_ROS_ISOLATION' not in os.environ:
            domain_id = stack.enter_context(domain_coordinator.domain_id())
            print('Running with ROS_DOMAIN_ID {}'.format(domain_id))
            os.environ['ROS_DOMAIN_ID'] = str(domain_id)
        sys.exit(ament_cmake_test.main())
