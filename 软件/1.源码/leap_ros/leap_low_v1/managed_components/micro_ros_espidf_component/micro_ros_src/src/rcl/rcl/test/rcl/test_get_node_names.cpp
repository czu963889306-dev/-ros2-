// Copyright 2017 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <tuple>
#include <string>
#include <set>
#include <utility>

#include "rcutils/types.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/graph.h"
#include "rcl/rcl.h"
#include "rmw/rmw.h"

#include "rcl/error_handling.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using namespace std::chrono_literals;

class CLASSNAME (TestGetNodeNames, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {}

  void TearDown()
  {}
};

TEST_F(CLASSNAME(TestGetNodeNames, RMW_IMPLEMENTATION), test_rcl_get_node_names) {
  rcl_ret_t ret;
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context)) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context)) << rcl_get_error_string().str;
  });
  std::multiset<std::pair<std::string, std::string>> expected_nodes, discovered_nodes;

  auto node1_ptr = new rcl_node_t;
  *node1_ptr = rcl_get_zero_initialized_node();
  const char * node1_name = "node1";
  const char * node1_namespace = "/";
  rcl_node_options_t node1_options = rcl_node_get_default_options();
  ret = rcl_node_init(node1_ptr, node1_name, node1_namespace, &context, &node1_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(std::make_pair(std::string(node1_name), std::string(node1_namespace)));

  auto node2_ptr = new rcl_node_t;
  *node2_ptr = rcl_get_zero_initialized_node();
  const char * node2_name = "node2";
  const char * node2_namespace = "/";
  rcl_node_options_t node2_options = rcl_node_get_default_options();
  ret = rcl_node_init(node2_ptr, node2_name, node2_namespace, &context, &node2_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(std::make_pair(std::string(node2_name), std::string(node2_namespace)));

  auto node3_ptr = new rcl_node_t;
  *node3_ptr = rcl_get_zero_initialized_node();
  const char * node3_name = "node3";
  const char * node3_namespace = "/ns";
  rcl_node_options_t node3_options = rcl_node_get_default_options();
  ret = rcl_node_init(node3_ptr, node3_name, node3_namespace, &context, &node3_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(std::make_pair(std::string(node3_name), std::string(node3_namespace)));

  auto node4_ptr = new rcl_node_t;
  *node4_ptr = rcl_get_zero_initialized_node();
  const char * node4_name = "node2";
  const char * node4_namespace = "/ns/ns";
  rcl_node_options_t node4_options = rcl_node_get_default_options();
  ret = rcl_node_init(node4_ptr, node4_name, node4_namespace, &context, &node4_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(std::make_pair(std::string(node4_name), std::string(node4_namespace)));

  auto node5_ptr = new rcl_node_t;
  *node5_ptr = rcl_get_zero_initialized_node();
  const char * node5_name = "node1";
  const char * node5_namespace = "/";
  rcl_node_options_t node5_options = rcl_node_get_default_options();
  ret = rcl_node_init(node5_ptr, node5_name, node5_namespace, &context, &node5_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(std::make_pair(std::string(node5_name), std::string(node5_namespace)));

  std::this_thread::sleep_for(1s);

  rcutils_string_array_t node_names = rcutils_get_zero_initialized_string_array();
  rcutils_string_array_t node_namespaces = rcutils_get_zero_initialized_string_array();
  ret = rcl_get_node_names(node1_ptr, node1_options.allocator, &node_names, &node_namespaces);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcl_get_error_string().str;

  std::stringstream ss;
  ss << "[test_rcl_get_node_names]: Found node names:" << std::endl;
  for (size_t i = 0; i < node_names.size; ++i) {
    ss << node_names.data[i] << std::endl;
  }
  EXPECT_EQ(node_names.size, node_namespaces.size) << ss.str();

  for (size_t i = 0; i < node_names.size; ++i) {
    discovered_nodes.insert(
      std::make_pair(
        std::string(node_names.data[i]),
        std::string(node_namespaces.data[i])));
  }
  EXPECT_EQ(discovered_nodes, expected_nodes);

  ret = rcutils_string_array_fini(&node_names);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_string_array_fini(&node_namespaces);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcl_node_fini(node1_ptr);
  delete node1_ptr;
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_node_fini(node2_ptr);
  delete node2_ptr;
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_node_fini(node3_ptr);
  delete node3_ptr;
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_node_fini(node4_ptr);
  delete node4_ptr;
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_node_fini(node5_ptr);
  delete node5_ptr;
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(
  CLASSNAME(TestGetNodeNames, RMW_IMPLEMENTATION), test_rcl_get_node_names_with_enclave)
{
  rcl_ret_t ret;
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_context_t context = rcl_get_zero_initialized_context();
  const char * enclave_name = "/enclave";
  const char * argv[] = {"--ros-args", "--enclave", enclave_name};
  ret = rcl_init(3, argv, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context)) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context)) << rcl_get_error_string().str;
  });
  std::multiset<std::tuple<std::string, std::string, std::string>>
  expected_nodes, discovered_nodes;

  rcl_node_t node1 = rcl_get_zero_initialized_node();
  const char * node1_name = "node1";
  const char * node1_namespace = "/";
  rcl_node_options_t node1_options = rcl_node_get_default_options();
  ret = rcl_node_init(&node1, node1_name, node1_namespace, &context, &node1_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(
    std::make_tuple(
      std::string(node1_name),
      std::string(node1_namespace),
      std::string(enclave_name)));

  rcl_node_t node2 = rcl_get_zero_initialized_node();
  const char * node2_name = "node2";
  const char * node2_namespace = "/";
  rcl_node_options_t node2_options = rcl_node_get_default_options();
  ret = rcl_node_init(&node2, node2_name, node2_namespace, &context, &node2_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(
    std::make_tuple(
      std::string(node2_name),
      std::string(node2_namespace),
      std::string(enclave_name)));

  rcl_node_t node3 = rcl_get_zero_initialized_node();
  const char * node3_name = "node3";
  const char * node3_namespace = "/ns";
  rcl_node_options_t node3_options = rcl_node_get_default_options();
  ret = rcl_node_init(&node3, node3_name, node3_namespace, &context, &node3_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(
    std::make_tuple(
      std::string(node3_name),
      std::string(node3_namespace),
      std::string(enclave_name)));

  rcl_node_t node4 = rcl_get_zero_initialized_node();
  const char * node4_name = "node2";
  const char * node4_namespace = "/ns/ns";
  rcl_node_options_t node4_options = rcl_node_get_default_options();
  ret = rcl_node_init(&node4, node4_name, node4_namespace, &context, &node4_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(
    std::make_tuple(
      std::string(node4_name),
      std::string(node4_namespace),
      std::string(enclave_name)));

  rcl_node_t node5 = rcl_get_zero_initialized_node();
  const char * node5_name = "node1";
  const char * node5_namespace = "/";
  rcl_node_options_t node5_options = rcl_node_get_default_options();
  ret = rcl_node_init(&node5, node5_name, node5_namespace, &context, &node5_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  expected_nodes.insert(
    std::make_tuple(
      std::string(node5_name),
      std::string(node5_namespace),
      std::string(enclave_name)));

  std::this_thread::sleep_for(1s);

  rcutils_string_array_t node_names = rcutils_get_zero_initialized_string_array();
  rcutils_string_array_t node_namespaces = rcutils_get_zero_initialized_string_array();
  rcutils_string_array_t enclaves = rcutils_get_zero_initialized_string_array();
  ret = rcl_get_node_names_with_enclaves(
    &node1, node1_options.allocator, &node_names, &node_namespaces, &enclaves);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcl_get_error_string().str;

  std::stringstream ss;
  ss << "[test_rcl_get_node_names]: Found node names:" << std::endl;
  for (size_t i = 0; i < node_names.size; ++i) {
    ss << node_names.data[i] << std::endl;
  }
  EXPECT_EQ(node_names.size, node_namespaces.size) << ss.str();

  for (size_t i = 0; i < node_names.size; ++i) {
    discovered_nodes.insert(
      std::make_tuple(
        std::string(node_names.data[i]),
        std::string(node_namespaces.data[i]),
        std::string(enclaves.data[i])));
  }
  EXPECT_EQ(discovered_nodes, expected_nodes);

  ret = rcutils_string_array_fini(&node_names);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_string_array_fini(&node_namespaces);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_string_array_fini(&enclaves);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcl_node_fini(&node1);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_node_fini(&node2);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_node_fini(&node3);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_node_fini(&node4);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_node_fini(&node5);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}
