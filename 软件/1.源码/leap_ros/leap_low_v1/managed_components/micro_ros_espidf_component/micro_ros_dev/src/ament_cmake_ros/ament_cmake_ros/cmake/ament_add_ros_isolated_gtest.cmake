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

#
# Add a gtest that runs with ROS_DOMAIN_ID isolation.
# If no ROS_DOMAIN_ID is set, automatically select one not in use by another
# isolated test before running the target test.
# This will prevent tests running in parallel from interfering with
# one-another.
#
# This behavior can be disabled for debugging in two ways:
# 1) Creating an environment variable called DISABLE_ROS_ISOLATION
# 2) Setting a ROS_DOMAIN_ID environment variable.
#    This will cause the tests to use that ROS_DOMAIN_ID.
#
# Parameters are the same as ament_add_gtest

function(ament_add_ros_isolated_gtest target)

  set(RUNNER "RUNNER" "${ament_cmake_ros_DIR}/run_test_isolated.py")

  ament_add_gtest(
    "${target}"
    RUNNER "${RUNNER}"
    ${ARGN}
  )

endfunction()
