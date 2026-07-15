# Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

cmake_minimum_required(VERSION 3.5)

find_package(ament_cmake_gtest REQUIRED)

project(rmw_microxrcedds_tests LANGUAGES CXX)

macro(rmw_test TEST_NAME TEST_SOURCE)
  ament_add_gtest(${TEST_NAME}
    ${TEST_SOURCE}
    test_utils.cpp)

  target_link_libraries(${TEST_NAME}
    microcdr
    microxrcedds_client
    rmw_microxrcedds)

  include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/../src)
  include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/../test)

  target_include_directories(${TEST_NAME}
    PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
      ${PROJECT_SOURCE_DIR}/src
      $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>)

  set_target_properties(${TEST_NAME} PROPERTIES
    CXX_STANDARD
      14
    CXX_STANDARD_REQUIRED
      YES)

  ament_target_dependencies(${TEST_NAME} rmw)
endmacro()

rmw_test(test-node        test_node.cpp)
rmw_test(test-publisher   test_publisher.cpp)
rmw_test(test-service     test_service.cpp)
rmw_test(test-client      test_client.cpp)
rmw_test(test-subscriber  test_subscription.cpp)
rmw_test(test-pubsub      test_pubsub.cpp)
rmw_test(test-reqres      test_reqres.cpp)
rmw_test(test-topic       test_topic.cpp)
rmw_test(test-rmw         test_rmw.cpp)
rmw_test(test-sizes       test_sizes.cpp)
rmw_test(test-guardcond   test_guard_condition.cpp)