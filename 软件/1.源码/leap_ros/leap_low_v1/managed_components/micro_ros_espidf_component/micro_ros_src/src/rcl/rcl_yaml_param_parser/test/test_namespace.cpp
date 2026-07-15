// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include <yaml.h>

#include <string>
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl_yaml_param_parser/parser.h"
#include "../src/impl/namespace.h"
#include "rcutils/allocator.h"
#include "rcutils/strdup.h"

TEST(TestNamespace, add_name_to_ns) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  namespace_tracker_t ns_tracker;
  ns_tracker.node_ns = nullptr;
  ns_tracker.parameter_ns = nullptr;
  ns_tracker.num_node_ns = 0;
  ns_tracker.num_parameter_ns = 0;

  rcutils_ret_t ret = add_name_to_ns(&ns_tracker, nullptr, NS_TYPE_NODE, allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, ns_tracker.node_ns);

  ret = add_name_to_ns(&ns_tracker, "node1", NS_TYPE_NODE, allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("node1", ns_tracker.node_ns);
  EXPECT_EQ(1u, ns_tracker.num_node_ns);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(ns_tracker.node_ns, allocator.state);
  });

  ret = add_name_to_ns(&ns_tracker, "node2", NS_TYPE_NODE, allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("node1/node2", ns_tracker.node_ns);
  EXPECT_EQ(2u, ns_tracker.num_node_ns);

  ret = add_name_to_ns(&ns_tracker, "param1", NS_TYPE_PARAM, allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("param1", ns_tracker.parameter_ns);
  EXPECT_EQ(1u, ns_tracker.num_parameter_ns);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(ns_tracker.parameter_ns, allocator.state);
  });

  ret = add_name_to_ns(&ns_tracker, "param2", NS_TYPE_PARAM, allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("param1.param2", ns_tracker.parameter_ns);
  EXPECT_EQ(2u, ns_tracker.num_parameter_ns);
}

TEST(TestNamespace, replace_ns) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  namespace_tracker_t ns_tracker;
  ns_tracker.node_ns = rcutils_strdup("initial_node1/initial_node2", allocator);
  ASSERT_STREQ("initial_node1/initial_node2", ns_tracker.node_ns);
  ns_tracker.parameter_ns = rcutils_strdup("initial_param1.initial_param2", allocator);
  ASSERT_STREQ("initial_param1.initial_param2", ns_tracker.parameter_ns);
  ns_tracker.num_node_ns = 2;
  ns_tracker.num_parameter_ns = 2;

  char * expected_ns = rcutils_strdup("new_ns1/new_ns2/new_ns3", allocator);
  ASSERT_STREQ("new_ns1/new_ns2/new_ns3", expected_ns);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(ns_tracker.node_ns, allocator.state);
    allocator.deallocate(ns_tracker.parameter_ns, allocator.state);
    allocator.deallocate(expected_ns, allocator.state);
  });

  rcutils_ret_t ret =
    replace_ns(&ns_tracker, expected_ns, 3, NS_TYPE_NODE, allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ(expected_ns, ns_tracker.node_ns);
  EXPECT_EQ(3u, ns_tracker.num_node_ns);

  char * expected_param_ns =
    rcutils_strdup("new_param1.new_param2.new_param3", allocator);
  ASSERT_STREQ("new_param1.new_param2.new_param3", expected_param_ns);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(expected_param_ns, allocator.state);
  });

  ret = replace_ns(&ns_tracker, expected_param_ns, 3, NS_TYPE_PARAM, allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ(expected_param_ns, ns_tracker.parameter_ns);
  EXPECT_EQ(3u, ns_tracker.num_parameter_ns);
}

TEST(TestNamespace, replace_ns_maybe_fail) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  namespace_tracker_t ns_tracker;
  ns_tracker.node_ns = rcutils_strdup("node1/node2", allocator);
  ASSERT_STREQ("node1/node2", ns_tracker.node_ns);
  ns_tracker.parameter_ns = rcutils_strdup("param1.param2", allocator);
  ASSERT_STREQ("param1.param2", ns_tracker.parameter_ns);
  ns_tracker.num_node_ns = 2;
  ns_tracker.num_parameter_ns = 2;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(ns_tracker.node_ns, allocator.state);
    allocator.deallocate(ns_tracker.parameter_ns, allocator.state);
  });

  char * expected_ns = rcutils_strdup("new_ns1/new_ns2/new_ns3", allocator);
  ASSERT_STREQ("new_ns1/new_ns2/new_ns3", expected_ns);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(expected_ns, allocator.state);
  });

  char * expected_param_ns =
    rcutils_strdup("new_param1.new_param2.new_param3", allocator);
  ASSERT_STREQ("new_param1.new_param2.new_param3", expected_param_ns);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(expected_param_ns, allocator.state);
  });

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcutils_ret_t ret =
    replace_ns(&ns_tracker, expected_ns, 3, NS_TYPE_NODE, allocator);
    if (RCUTILS_RET_OK != ret) {
      EXPECT_EQ(nullptr, ns_tracker.node_ns);
      rcutils_reset_error();
    } else {
      EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
      EXPECT_STREQ(expected_ns, ns_tracker.node_ns);
      EXPECT_EQ(3u, ns_tracker.num_node_ns);
    }

    ret = replace_ns(&ns_tracker, expected_param_ns, 3, NS_TYPE_PARAM, allocator);
    if (RCUTILS_RET_OK != ret) {
      EXPECT_EQ(nullptr, ns_tracker.parameter_ns);
      rcutils_reset_error();
    } else {
      EXPECT_STREQ(expected_param_ns, ns_tracker.parameter_ns);
      EXPECT_EQ(3u, ns_tracker.num_parameter_ns);
    }
  });
}
