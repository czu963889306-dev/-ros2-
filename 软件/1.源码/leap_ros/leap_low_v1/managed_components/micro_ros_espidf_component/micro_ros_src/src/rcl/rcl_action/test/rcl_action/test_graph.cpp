// Copyright 2019 Open Source Robotics Foundation, Inc.
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
#include <string>
#include <thread>

#include "rcl_action/action_client.h"
#include "rcl_action/action_server.h"
#include "rcl_action/graph.h"

#include "rcl/error_handling.h"
#include "rcl/rcl.h"
#include "rcutils/testing/fault_injection.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "test_msgs/action/fibonacci.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

void * bad_malloc(size_t, void *)
{
  return NULL;
}

class CLASSNAME (TestActionGraphFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_allocator_t zero_allocator;
  rcl_context_t old_context;
  rcl_context_t context;
  rcl_node_t old_node;
  rcl_node_t node;
  rcl_node_t zero_node;
  const char * test_graph_node_name = "test_action_graph_node";
  const char * test_graph_old_node_name = "test_action_graph_old_node_name";
  const char * test_graph_unknown_node_name = "test_action_graph_unknown_node_name";

  void SetUp()
  {
    rcl_ret_t ret;
    this->zero_node = rcl_get_zero_initialized_node();
    this->zero_allocator = static_cast<rcl_allocator_t>(rcutils_get_zero_initialized_allocator());

    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    ret = rcl_init_options_init(&init_options, this->allocator);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
    });
    this->old_context = rcl_get_zero_initialized_context();
    ret = rcl_init(0, nullptr, &init_options, &this->old_context);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    this->old_node = rcl_get_zero_initialized_node();
    rcl_node_options_t node_options = rcl_node_get_default_options();
    ret = rcl_node_init(
      &this->old_node, this->test_graph_old_node_name, "", &this->old_context, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_shutdown(&this->old_context);  // after this, the old_node should be invalid
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    this->context = rcl_get_zero_initialized_context();
    ret = rcl_init(0, nullptr, &init_options, &this->context);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    this->node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&this->node, test_graph_node_name, "", &this->context, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown()
  {
    rcl_ret_t ret;
    ret = rcl_node_fini(&this->old_node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_node_fini(&this->node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_shutdown(&this->context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(&this->context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(&this->old_context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
};

TEST_F(
  CLASSNAME(TestActionGraphFixture, RMW_IMPLEMENTATION),
  test_action_get_client_names_and_types_by_node)
{
  rcl_ret_t ret;
  rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();

  // Invalid node
  ret = rcl_action_get_client_names_and_types_by_node(
    nullptr, &this->allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->zero_node, &this->allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->old_node, &this->allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Invalid allocator
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->node, nullptr, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->node, &this->zero_allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Invalid node name
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->node, &this->allocator, "_!test_this_is_not_a_valid_name", "", &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID_NAME, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Non-existent node
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->node, &this->allocator, this->test_graph_unknown_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_NODE_NAME_NON_EXISTENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Invalid names and types
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->node, &this->allocator, this->test_graph_node_name, "", nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Failing allocator
  rcl_allocator_t bad_allocator = rcl_get_default_allocator();
  bad_allocator.allocate = bad_malloc;
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->node, &bad_allocator, this->test_graph_node_name, "", nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Valid call
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->node, &this->allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(
  CLASSNAME(TestActionGraphFixture, RMW_IMPLEMENTATION),
  test_action_get_server_names_and_types_by_node)
{
  rcl_ret_t ret;
  rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();

  // Invalid node
  ret = rcl_action_get_server_names_and_types_by_node(
    nullptr, &this->allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->zero_node, &this->allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->old_node, &this->allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Invalid allocator
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->node, nullptr, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->node, &this->zero_allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Invalid node name
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->node, &this->allocator, "_!test_this_is_not_a_valid_name", "", &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID_NAME, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Non-existent node
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->node, &this->allocator, this->test_graph_unknown_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_NODE_NAME_NON_EXISTENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Invalid names and types
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->node, &this->allocator, this->test_graph_node_name, "", nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Valid call
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->node, &this->allocator, this->test_graph_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(
  CLASSNAME(TestActionGraphFixture, RMW_IMPLEMENTATION),
  test_action_get_names_and_types)
{
  rcl_ret_t ret;
  rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();

  // Invalid node
  ret = rcl_action_get_names_and_types(nullptr, &this->allocator, &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_names_and_types(&this->zero_node, &this->allocator, &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_names_and_types(&this->old_node, &this->allocator, &nat);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Invalid allocator
  ret = rcl_action_get_names_and_types(&this->node, nullptr, &nat);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_action_get_names_and_types(&this->node, &this->zero_allocator, &nat);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Invalid names and types
  ret = rcl_action_get_names_and_types(&this->node, &this->allocator, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  // Valid call
  ret = rcl_action_get_names_and_types(&this->node, &this->allocator, &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

/**
 * Type define a get actions function.
 */
typedef std::function<
    rcl_ret_t(const rcl_node_t *, rcl_names_and_types_t *)
> GetActionsFunc;

/**
 * Extend the TestActionGraphFixture with a multi-node fixture for node discovery and node-graph
 * perspective.
 */
class TestActionGraphMultiNodeFixture : public CLASSNAME(TestActionGraphFixture, RMW_IMPLEMENTATION)
{
public:
  const char * remote_node_name = "remote_graph_node";
  const char * action_name = "/test_action_info_functions__";
  rcl_node_t remote_node;
  rcl_context_t remote_context;
  GetActionsFunc action_func, clients_by_node_func, servers_by_node_func;

  void SetUp() override
  {
    CLASSNAME(TestActionGraphFixture, RMW_IMPLEMENTATION) ::SetUp();

    rcl_ret_t ret;
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) <<
        rcl_get_error_string().str;
    });

    this->remote_node = rcl_get_zero_initialized_node();
    rcl_node_options_t node_options = rcl_node_get_default_options();

    this->remote_context = rcl_get_zero_initialized_context();
    ret = rcl_init(0, nullptr, &init_options, &this->remote_context);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_node_init(
      &this->remote_node, this->remote_node_name, "", &this->remote_context, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    action_func = std::bind(
      rcl_action_get_names_and_types,
      std::placeholders::_1,
      &this->allocator,
      std::placeholders::_2);
    clients_by_node_func = std::bind(
      rcl_action_get_client_names_and_types_by_node,
      std::placeholders::_1,
      &this->allocator,
      this->remote_node_name,
      "",
      std::placeholders::_2);
    servers_by_node_func = std::bind(
      rcl_action_get_server_names_and_types_by_node,
      std::placeholders::_1,
      &this->allocator,
      this->remote_node_name,
      "",
      std::placeholders::_2);
    WaitForAllNodesAlive();
  }

  void TearDown() override
  {
    CLASSNAME(TestActionGraphFixture, RMW_IMPLEMENTATION) ::TearDown();

    rcl_ret_t ret;
    ret = rcl_node_fini(&this->remote_node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_shutdown(&this->remote_context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(&this->remote_context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void WaitForAllNodesAlive()
  {
    rcl_ret_t ret;
    rcutils_string_array_t node_names = rcutils_get_zero_initialized_string_array();
    rcutils_string_array_t node_namespaces = rcutils_get_zero_initialized_string_array();
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      ret = rcutils_string_array_fini(&node_names);
      ASSERT_EQ(RCUTILS_RET_OK, ret);
      ret = rcutils_string_array_fini(&node_namespaces);
      ASSERT_EQ(RCUTILS_RET_OK, ret);
    });
    // Wait for all 3 nodes to be discovered: remote_node, old_node, node
    size_t attempts = 0u;
    size_t max_attempts = 4u;
    while (node_names.size < 3u) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      ret = rcl_get_node_names(&this->remote_node, allocator, &node_names, &node_namespaces);
      ++attempts;
      ASSERT_LE(attempts, max_attempts) << "Unable to attain all required nodes";
      if (node_names.size < 3u) {
        ret = rcutils_string_array_fini(&node_names);
        ASSERT_EQ(RCUTILS_RET_OK, ret);
        ret = rcutils_string_array_fini(&node_namespaces);
        ASSERT_EQ(RCUTILS_RET_OK, ret);
        node_names = rcutils_get_zero_initialized_string_array();
        node_namespaces = rcutils_get_zero_initialized_string_array();
      }
    }
  }

  void WaitForActionCount(
    GetActionsFunc func,
    size_t expected_count,
    std::chrono::milliseconds duration)
  {
    auto start_time = std::chrono::system_clock::now();
    auto curr_time = start_time;

    rcl_ret_t ret;
    while ((curr_time - start_time) < duration) {
      rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();
      ret = func(&this->node, &nat);
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      size_t action_count = nat.names.size;
      ret = rcl_names_and_types_fini(&nat);
      EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      if (action_count == expected_count) {
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      curr_time = std::chrono::system_clock::now();
    }
  }
};

// Note, this test could be affected by other communication on the same ROS domain
TEST_F(TestActionGraphMultiNodeFixture, test_action_get_names_and_types) {
  rcl_ret_t ret;
  // Create an action client
  rcl_action_client_t action_client = rcl_action_get_zero_initialized_client();
  const rosidl_action_type_support_t * action_typesupport =
    ROSIDL_GET_ACTION_TYPE_SUPPORT(test_msgs, Fibonacci);
  const char * client_action_name = "/test_action_get_names_and_types_client_action_name";
  rcl_action_client_options_t action_client_options = rcl_action_client_get_default_options();
  ret = rcl_action_client_init(
    &action_client,
    &this->remote_node,
    action_typesupport,
    client_action_name,
    &action_client_options);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_action_client_fini(&action_client, &this->remote_node)) <<
      rcl_get_error_string().str;
  });

  WaitForActionCount(action_func, 1u, std::chrono::seconds(1));

  // Check that there is exactly one action name
  rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();
  ret = action_func(&this->node, &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_EQ(nat.names.size, 1u);
  EXPECT_STREQ(nat.names.data[0], client_action_name);
  ASSERT_EQ(nat.types[0].size, 1u);
  EXPECT_STREQ(nat.types[0].data[0], "test_msgs/action/Fibonacci");

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Create an action server
  rcl_action_server_t action_server = rcl_action_get_zero_initialized_server();
  rcl_clock_t clock;
  ret = rcl_clock_init(RCL_STEADY_TIME, &clock, &this->allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_clock_fini(&clock)) << rcl_get_error_string().str;
  });
  const char * server_action_name = "/test_action_get_names_and_types_server_action_name";
  rcl_action_server_options_t action_server_options = rcl_action_server_get_default_options();
  ret = rcl_action_server_init(
    &action_server,
    &this->remote_node,
    &clock,
    action_typesupport,
    server_action_name,
    &action_server_options);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_action_server_fini(&action_server, &this->remote_node)) <<
      rcl_get_error_string().str;
  });

  WaitForActionCount(action_func, 2u, std::chrono::seconds(1));

  ret = action_func(&this->node, &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_EQ(nat.names.size, 2u);
  EXPECT_STREQ(nat.names.data[0], client_action_name);
  EXPECT_STREQ(nat.names.data[1], server_action_name);
  ASSERT_EQ(nat.types[0].size, 1u);
  EXPECT_STREQ(nat.types[0].data[0], "test_msgs/action/Fibonacci");
  ASSERT_EQ(nat.types[1].size, 1u);
  EXPECT_STREQ(nat.types[1].data[0], "test_msgs/action/Fibonacci");

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

// Note, this test could be affected by other communication on the same ROS domain
TEST_F(TestActionGraphMultiNodeFixture, test_action_get_server_names_and_types_by_node) {
  rcl_ret_t ret;
  // Create an action client
  rcl_action_client_t action_client = rcl_action_get_zero_initialized_client();
  const rosidl_action_type_support_t * action_typesupport =
    ROSIDL_GET_ACTION_TYPE_SUPPORT(test_msgs, Fibonacci);
  rcl_action_client_options_t action_client_options = rcl_action_client_get_default_options();
  ret = rcl_action_client_init(
    &action_client,
    &this->remote_node,
    action_typesupport,
    this->action_name,
    &action_client_options);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_action_client_fini(&action_client, &this->remote_node)) <<
      rcl_get_error_string().str;
  });
  // Check that there are no action servers
  rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();
  ret = rcl_action_get_server_names_and_types_by_node(
    &this->node, &this->allocator, this->remote_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_EQ(nat.names.size, 0u);

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Create an action server
  rcl_action_server_t action_server = rcl_action_get_zero_initialized_server();
  rcl_clock_t clock;
  ret = rcl_clock_init(RCL_STEADY_TIME, &clock, &this->allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_clock_fini(&clock)) << rcl_get_error_string().str;
  });
  rcl_action_server_options_t action_server_options = rcl_action_server_get_default_options();
  ret = rcl_action_server_init(
    &action_server,
    &this->remote_node,
    &clock,
    action_typesupport,
    this->action_name,
    &action_server_options);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_action_server_fini(&action_server, &this->remote_node)) <<
      rcl_get_error_string().str;
  });

  WaitForActionCount(servers_by_node_func, 1u, std::chrono::seconds(1));
  ret = servers_by_node_func(&this->node, &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_EQ(nat.names.size, 1u);
  EXPECT_STREQ(nat.names.data[0], this->action_name);
  ASSERT_EQ(nat.types[0].size, 1u);
  EXPECT_STREQ(nat.types[0].data[0], "test_msgs/action/Fibonacci");

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

// Note, this test could be affected by other communication on the same ROS domain
TEST_F(TestActionGraphMultiNodeFixture, test_action_get_client_names_and_types_by_node) {
  rcl_ret_t ret;
  const rosidl_action_type_support_t * action_typesupport =
    ROSIDL_GET_ACTION_TYPE_SUPPORT(test_msgs, Fibonacci);
  // Create an action server
  rcl_action_server_t action_server = rcl_action_get_zero_initialized_server();
  rcl_clock_t clock;
  ret = rcl_clock_init(RCL_STEADY_TIME, &clock, &this->allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_clock_fini(&clock)) << rcl_get_error_string().str;
  });
  rcl_action_server_options_t action_server_options = rcl_action_server_get_default_options();
  ret = rcl_action_server_init(
    &action_server,
    &this->remote_node,
    &clock,
    action_typesupport,
    this->action_name,
    &action_server_options);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_action_server_fini(&action_server, &this->remote_node)) <<
      rcl_get_error_string().str;
  });

  // Check that there are no action clients
  rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();
  ret = rcl_action_get_client_names_and_types_by_node(
    &this->node, &this->allocator, this->remote_node_name, "", &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_EQ(nat.names.size, 0u);

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Create an action client
  rcl_action_client_t action_client = rcl_action_get_zero_initialized_client();
  rcl_action_client_options_t action_client_options = rcl_action_client_get_default_options();
  ret = rcl_action_client_init(
    &action_client,
    &this->remote_node,
    action_typesupport,
    this->action_name,
    &action_client_options);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_action_client_fini(&action_client, &this->remote_node)) <<
      rcl_get_error_string().str;
  });

  WaitForActionCount(clients_by_node_func, 1u, std::chrono::seconds(1));
  ret = clients_by_node_func(&this->node, &nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_EQ(nat.names.size, 1u);
  EXPECT_STREQ(nat.names.data[0], this->action_name);
  ASSERT_EQ(nat.types[0].size, 1u);
  EXPECT_STREQ(nat.types[0].data[0], "test_msgs/action/Fibonacci");

  ret = rcl_names_and_types_fini(&nat);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(TestActionGraphMultiNodeFixture, get_names_and_types_maybe_fail)
{
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();
    rcl_ret_t ret = rcl_action_get_names_and_types(&this->node, &this->allocator, &nat);
    if (RCL_RET_OK == ret) {
      ret = rcl_names_and_types_fini(&nat);
      if (RCL_RET_OK != ret) {
        EXPECT_TRUE(rcutils_error_is_set());
        rcutils_reset_error();
        EXPECT_EQ(RCL_RET_OK, rcl_names_and_types_fini(&nat));
      }
    }
  });
}

TEST_F(TestActionGraphMultiNodeFixture, action_client_init_maybe_fail)
{
  RCUTILS_FAULT_INJECTION_TEST(
  {
    const rosidl_action_type_support_t * action_typesupport = ROSIDL_GET_ACTION_TYPE_SUPPORT(
      test_msgs, Fibonacci);

    rcl_action_client_t action_client = rcl_action_get_zero_initialized_client();
    rcl_action_client_options_t action_client_options = rcl_action_client_get_default_options();
    rcl_ret_t ret = rcl_action_client_init(
      &action_client,
      &this->remote_node,
      action_typesupport,
      this->action_name,
      &action_client_options);

    if (RCL_RET_OK == ret) {
      ret = rcl_action_client_fini(&action_client, &this->remote_node);
      if (RCL_RET_OK != ret) {
        // This isn't always set, but just in case reset anyway
        rcutils_reset_error();
      }
    }
  });
}

TEST_F(TestActionGraphMultiNodeFixture, rcl_get_client_names_and_types_by_node_maybe_fail)
{
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();
    rcl_ret_t ret = rcl_action_get_client_names_and_types_by_node(
      &this->node, &this->allocator, this->test_graph_node_name, "", &nat);
    if (RCL_RET_OK == ret) {
      ret = rcl_names_and_types_fini(&nat);
      if (ret != RCL_RET_OK) {
        EXPECT_TRUE(rcutils_error_is_set());
        rcutils_reset_error();
        EXPECT_EQ(RCL_RET_OK, rcl_names_and_types_fini(&nat));
      }
    }
  });
}

TEST_F(TestActionGraphMultiNodeFixture, rcl_get_server_names_and_types_by_node_maybe_fail)
{
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_names_and_types_t nat = rcl_get_zero_initialized_names_and_types();
    rcl_ret_t ret = rcl_action_get_server_names_and_types_by_node(
      &this->node, &this->allocator, this->test_graph_node_name, "", &nat);
    if (RCL_RET_OK == ret) {
      ret = rcl_names_and_types_fini(&nat);
      if (ret != RCL_RET_OK) {
        EXPECT_TRUE(rcutils_error_is_set());
        rcutils_reset_error();
        EXPECT_EQ(RCL_RET_OK, rcl_names_and_types_fini(&nat));
      }
    }
  });
}
