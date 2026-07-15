# Copyright 2016 Open Source Robotics Foundation, Inc.
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

if(rcl_add_custom_launch_test_INCLUDED)
  return()
endif()

find_package(launch_testing_ament_cmake REQUIRED)
set(rcl_add_custom_launch_test_INCLUDED TRUE)

macro(rcl_add_custom_launch_test test_name executable1 executable2)
  set(TEST_NAME "${test_name}")
  set(TEST_EXECUTABLE1 "$<TARGET_FILE:${executable1}${target_suffix}>")
  set(TEST_EXECUTABLE1_NAME "${executable1}")
  set(TEST_EXECUTABLE2 "$<TARGET_FILE:${executable2}${target_suffix}>")
  set(TEST_EXECUTABLE2_NAME "${executable2}")
  configure_file(
    rcl/test_two_executables.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/${test_name}${target_suffix}.py.configure
    @ONLY
  )
  file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test/${test_name}${target_suffix}_$<CONFIG>.py"
    INPUT "${CMAKE_CURRENT_BINARY_DIR}/${test_name}${target_suffix}.py.configure"
  )
  add_launch_test(
    "${CMAKE_CURRENT_BINARY_DIR}/test/${test_name}${target_suffix}_$<CONFIG>.py"
    TARGET ${test_name}${target_suffix}
    ${ARGN}
  )
  if(TEST ${test_name}${target_suffix})
    set_tests_properties(${test_name}${target_suffix} PROPERTIES DEPENDS "${executable1}${target_suffix} ${executable2}${target_suffix}")
  endif()
endmacro()
