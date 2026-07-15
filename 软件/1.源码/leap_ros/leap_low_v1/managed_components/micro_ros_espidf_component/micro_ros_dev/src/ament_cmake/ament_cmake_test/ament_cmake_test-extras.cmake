# Copyright 2014 Open Source Robotics Foundation, Inc.
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

# copied from ament_cmake_test/ament_cmake_test-extras.cmake

enable_testing()
# same option as in the CTest module
option(BUILD_TESTING "Build the testing tree." ON)

# option()
set(
  AMENT_TEST_RESULTS_DIR "${CMAKE_BINARY_DIR}/test_results"
  CACHE PATH "The path where test results are generated"
)

if(BUILD_TESTING)
  # configure ctest not to truncate the dashboard summary
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake"
    "set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 0)\n"
    "set(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 0)\n")
  # create dart configuration from template
  site_name(SITE)
  configure_file(
    ${CMAKE_ROOT}/Modules/DartConfiguration.tcl.in
    ${PROJECT_BINARY_DIR}/CTestConfiguration.ini)
endif()

find_package(ament_cmake_core QUIET REQUIRED)

include("${ament_cmake_test_DIR}/ament_add_test.cmake")
include("${ament_cmake_test_DIR}/ament_add_test_label.cmake")
