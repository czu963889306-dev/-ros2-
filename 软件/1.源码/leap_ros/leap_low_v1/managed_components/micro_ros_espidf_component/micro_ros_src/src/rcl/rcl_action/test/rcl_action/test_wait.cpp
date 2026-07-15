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

#include "rcl_action/action_client.h"
#include "rcl_action/action_server.h"
#include "rcl_action/wait.h"
#include "rcl_action/action_client_impl.h"
#include "rcl_action/action_server_impl.h"

#include "rcl/error_handling.h"
#include "rcl/rcl.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "test_msgs/action/fibonacci.h"

class TestActionClientWait : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rcl_ret_t ret;
    {
      rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
      ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
      {
        EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
      });
      this->context = rcl_get_zero_initialized_context();
      ret = rcl_init(0, nullptr, &init_options, &this->context);
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    }
    this->node = rcl_get_zero_initialized_node();
    rcl_node_options_t node_options = rcl_node_get_default_options();
    const char * node_name = "test_action_client_node";
    ret = rcl_node_init(&this->node, node_name, "", &this->context, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_FALSE(rcl_error_is_set()) << rcl_get_error_string().str;

    const char * action_name = "test_action_client_name";
    const rosidl_action_type_support_t * action_typesupport =
      ROSIDL_GET_ACTION_TYPE_SUPPORT(test_msgs, Fibonacci);
    const rcl_action_client_options_t action_client_options =
      rcl_action_client_get_default_options();

    action_client = rcl_action_get_zero_initialized_client();
    ret = rcl_action_client_init(
      &this->action_client, &this->node, action_typesupport,
      action_name, &action_client_options);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_FALSE(rcl_error_is_set()) << rcl_get_error_string().str;
  }

  void TearDown() override
  {
    rcl_ret_t fini_ret = rcl_action_client_fini(&action_client, &this->node);
    EXPECT_EQ(RCL_RET_OK, fini_ret) << rcl_get_error_string().str;
    rcl_ret_t ret = rcl_node_fini(&this->node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_shutdown(&this->context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(&this->context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  rcl_context_t context;
  rcl_node_t node;
  rcl_action_client_t action_client;
};

class TestActionServerWait : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rcl_ret_t ret;
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    ret = rcl_init_options_init(&init_options, allocator);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
    });
    context = rcl_get_zero_initialized_context();
    ret = rcl_init(0, nullptr, &init_options, &context);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    this->node = rcl_get_zero_initialized_node();
    rcl_node_options_t node_options = rcl_node_get_default_options();
    ret = rcl_node_init(&this->node, "test_action_server_node", "", &context, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_clock_init(RCL_ROS_TIME, &this->clock, &allocator);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    const rosidl_action_type_support_t * ts = ROSIDL_GET_ACTION_TYPE_SUPPORT(
      test_msgs, Fibonacci);
    const rcl_action_server_options_t options = rcl_action_server_get_default_options();
    const char * action_name = "test_action_server_name";
    this->action_server = rcl_action_get_zero_initialized_server();
    ret = rcl_action_server_init(
      &this->action_server, &this->node, &this->clock, ts, action_name, &options);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_FALSE(rcl_error_is_set()) << rcl_get_error_string().str;
  }

  void TearDown() override
  {
    // Finalize
    rcl_ret_t ret = rcl_action_server_fini(&this->action_server, &this->node);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_clock_fini(&this->clock);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_node_fini(&this->node);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_shutdown(&context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(&this->context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void init_test_uuid0(uint8_t * uuid)
  {
    for (uint8_t i = 0; i < UUID_SIZE; ++i) {
      uuid[i] = i;
    }
  }

  void init_test_uuid1(uint8_t * uuid)
  {
    for (uint8_t i = 0; i < UUID_SIZE; ++i) {
      uuid[i] = 15 - i;
    }
  }

  rcl_action_server_t action_server;
  rcl_context_t context;
  rcl_node_t node;
  rcl_clock_t clock;
};  // class TestActionServer

TEST_F(TestActionClientWait, test_wait_set_add_action_client) {
  // Check wait_set is null
  size_t client_index = 42;
  size_t subscription_index = 42;
  rcl_ret_t ret = rcl_action_wait_set_add_action_client(
    nullptr, &action_client, &client_index, &subscription_index);
  EXPECT_EQ(RCL_RET_WAIT_SET_INVALID, ret);
  EXPECT_EQ(42u, client_index);
  EXPECT_EQ(42u, subscription_index);
  rcl_reset_error();

  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  });

  // Check action client is null
  ret = rcl_action_wait_set_add_action_client(
    &wait_set, nullptr, &client_index, &subscription_index);
  EXPECT_EQ(RCL_RET_ACTION_CLIENT_INVALID, ret);
  EXPECT_EQ(42u, client_index);
  EXPECT_EQ(42u, subscription_index);
  rcl_reset_error();

  // Failed to add goal client
  wait_set = rcl_get_zero_initialized_wait_set();
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 0, 0, 0, &this->context, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret);

  ret = rcl_action_wait_set_add_action_client(
    &wait_set, &action_client, &client_index, &subscription_index);
  EXPECT_EQ(RCL_RET_WAIT_SET_FULL, ret);
  EXPECT_EQ(42u, client_index);
  EXPECT_EQ(42u, subscription_index);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Failed to add cancel client
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 1, 0, 0, &this->context, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret);

  ret = rcl_action_wait_set_add_action_client(
    &wait_set, &action_client, &client_index, &subscription_index);
  EXPECT_EQ(RCL_RET_WAIT_SET_FULL, ret);
  EXPECT_EQ(42u, client_index);
  EXPECT_EQ(42u, subscription_index);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();


  // Failed to add result client
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 2, 0, 0, &this->context, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret);

  ret = rcl_action_wait_set_add_action_client(
    &wait_set, &action_client, &client_index, &subscription_index);
  EXPECT_EQ(RCL_RET_WAIT_SET_FULL, ret);
  EXPECT_EQ(42u, client_index);
  EXPECT_EQ(42u, subscription_index);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Failed to add feedback subscription
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 3, 0, 0, &this->context, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret);

  ret = rcl_action_wait_set_add_action_client(
    &wait_set, &action_client, &client_index, &subscription_index);
  EXPECT_EQ(RCL_RET_WAIT_SET_FULL, ret);
  EXPECT_EQ(42u, client_index);
  EXPECT_EQ(42u, subscription_index);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Failed to add status subscription
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 1, 0, 0, 3, 0, 0, &this->context, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret);

  ret = rcl_action_wait_set_add_action_client(
    &wait_set, &action_client, &client_index, &subscription_index);
  EXPECT_EQ(RCL_RET_WAIT_SET_FULL, ret);
  EXPECT_EQ(42u, client_index);
  EXPECT_EQ(42u, subscription_index);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Typical case
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 2, 0, 0, 3, 0, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_client(
    &wait_set, &action_client, &client_index, &subscription_index);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(0u, client_index);
  EXPECT_EQ(0u, subscription_index);

  // Should work fine, but doesn't increment counts
  // wait_set = rcl_get_zero_initialized_wait_set();
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 2, 0, 0, 3, 0, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ret = rcl_action_wait_set_add_action_client(&wait_set, &action_client, nullptr, nullptr);
  EXPECT_EQ(RCL_RET_OK, ret);
}

TEST_F(TestActionServerWait, test_wait_set_add_action_server) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  });
  rcl_ret_t ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 0, 0, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  size_t service_index = 42;
  ret = rcl_action_wait_set_add_action_server(nullptr, &this->action_server, &service_index);
  EXPECT_EQ(ret, RCL_RET_WAIT_SET_INVALID);
  EXPECT_EQ(service_index, 42u);
  rcl_reset_error();

  ret = rcl_action_wait_set_add_action_server(&wait_set, nullptr, &service_index);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID);
  EXPECT_EQ(service_index, 42u);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Check adding goal service fails
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 0, 0, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_server(&wait_set, &this->action_server, &service_index);
  EXPECT_EQ(ret, RCL_RET_WAIT_SET_FULL) << rcl_get_error_string().str;
  EXPECT_EQ(service_index, 42u);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Check adding cancel service fails
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 0, 1, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_server(&wait_set, &this->action_server, &service_index);
  EXPECT_EQ(ret, RCL_RET_WAIT_SET_FULL) << rcl_get_error_string().str;
  EXPECT_EQ(service_index, 42u);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Check adding result service fails
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 0, 2, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_server(&wait_set, &this->action_server, &service_index);
  EXPECT_EQ(ret, RCL_RET_WAIT_SET_FULL) << rcl_get_error_string().str;
  EXPECT_EQ(service_index, 42u);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Check adding expire timer fails
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 0, 0, 3, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_server(&wait_set, &this->action_server, &service_index);
  EXPECT_EQ(ret, RCL_RET_WAIT_SET_FULL) << rcl_get_error_string().str;
  EXPECT_EQ(service_index, 42u);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Check everything is good
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 1, 0, 3, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_server(&wait_set, &this->action_server, &service_index);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(service_index, 0u);

  // Everything should be ok without a valid service index.
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  ret = rcl_wait_set_init(
    &wait_set, 0, 0, 1, 0, 3, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  ret = rcl_action_wait_set_add_action_server(&wait_set, &this->action_server, nullptr);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(rcl_error_is_set()) << rcl_get_error_string().str;
}


TEST_F(TestActionClientWait, test_client_wait_set_get_num_entities) {
  const char * action_name = "test_action_client_name";
  const rosidl_action_type_support_t * action_typesupport =
    ROSIDL_GET_ACTION_TYPE_SUPPORT(test_msgs, Fibonacci);
  const rcl_action_client_options_t action_client_options =
    rcl_action_client_get_default_options();

  rcl_action_client_t action_client = rcl_action_get_zero_initialized_client();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t fini_ret = rcl_action_client_fini(&action_client, &this->node);
    EXPECT_EQ(RCL_RET_OK, fini_ret) << rcl_get_error_string().str;
  });

  rcl_ret_t ret = rcl_action_client_init(
    &action_client, &this->node, action_typesupport,
    action_name, &action_client_options);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  rcl_reset_error();

  size_t num_subscriptions = 0;
  size_t num_guard_conditions = 0;
  size_t num_timers = 0;
  size_t num_clients = 0;
  size_t num_services = 0;

  ret = rcl_action_client_wait_set_get_num_entities(
    nullptr, &num_subscriptions, &num_guard_conditions, &num_timers, &num_clients, &num_services);
  EXPECT_EQ(RCL_RET_ACTION_CLIENT_INVALID, ret);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_num_entities(
    &action_client, nullptr, &num_guard_conditions, &num_timers, &num_clients, &num_services);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_num_entities(
    &action_client, &num_subscriptions, nullptr, &num_timers, &num_clients, &num_services);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_num_entities(
    &action_client,
    &num_subscriptions,
    &num_guard_conditions,
    nullptr,
    &num_clients,
    &num_services);

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_num_entities(
    &action_client,
    &num_subscriptions,
    &num_guard_conditions,
    &num_clients,
    nullptr,
    &num_services);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_num_entities(
    &action_client,
    &num_subscriptions,
    &num_guard_conditions,
    &num_clients,
    &num_clients,
    nullptr);

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_num_entities(
    &action_client,
    &num_subscriptions,
    &num_guard_conditions,
    &num_clients,
    &num_clients,
    &num_services);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(num_subscriptions, 2u);
  EXPECT_EQ(num_guard_conditions, 0u);
  EXPECT_EQ(num_timers, 0u);
  EXPECT_EQ(num_clients, 3u);
  EXPECT_EQ(num_services, 0u);
}

TEST_F(TestActionServerWait, test_server_wait_set_get_num_entities) {
  size_t num_subscriptions = 0;
  size_t num_guard_conditions = 0;
  size_t num_timers = 0;
  size_t num_clients = 0;
  size_t num_services = 0;

  rcl_ret_t ret = rcl_action_server_wait_set_get_num_entities(
    nullptr, &num_subscriptions, &num_guard_conditions, &num_timers, &num_clients, &num_services);
  EXPECT_EQ(RCL_RET_ACTION_SERVER_INVALID, ret);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_num_entities(
    &this->action_server,
    nullptr,
    &num_guard_conditions,
    &num_timers,
    &num_clients,
    &num_services);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_num_entities(
    &this->action_server,
    &num_subscriptions,
    nullptr,
    &num_timers,
    &num_clients,
    &num_services);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_num_entities(
    &this->action_server,
    &num_subscriptions,
    &num_guard_conditions,
    nullptr,
    &num_clients,
    &num_services);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_num_entities(
    &this->action_server,
    &num_subscriptions,
    &num_guard_conditions,
    &num_timers,
    nullptr,
    &num_services);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_num_entities(
    &this->action_server,
    &num_subscriptions,
    &num_guard_conditions,
    &num_timers,
    &num_clients,
    nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_num_entities(
    &this->action_server,
    &num_subscriptions,
    &num_guard_conditions,
    &num_timers,
    &num_clients,
    &num_services);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(num_subscriptions, 0u);
  EXPECT_EQ(num_guard_conditions, 0u);
  EXPECT_EQ(num_timers, 1u);
  EXPECT_EQ(num_clients, 0u);
  EXPECT_EQ(num_services, 3u);
}

TEST_F(TestActionClientWait, test_client_wait_set_get_entities_ready) {
  const char * action_name = "test_action_client_name";
  const rosidl_action_type_support_t * action_typesupport =
    ROSIDL_GET_ACTION_TYPE_SUPPORT(test_msgs, Fibonacci);
  const rcl_action_client_options_t action_client_options =
    rcl_action_client_get_default_options();

  rcl_action_client_t action_client = rcl_action_get_zero_initialized_client();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t fini_ret = rcl_action_client_fini(&action_client, &this->node);
    EXPECT_EQ(RCL_RET_OK, fini_ret) << rcl_get_error_string().str;
  });

  rcl_ret_t ret = rcl_action_client_init(
    &action_client, &this->node, action_typesupport,
    action_name, &action_client_options);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  rcl_reset_error();

  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  });
  ret = rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 1, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  bool is_feedback_ready = false;
  bool is_status_ready = false;
  bool is_goal_response_ready = false;
  bool is_cancel_response_ready = false;
  bool is_result_response_ready = false;

  // Check valid arguments
  ret = rcl_action_client_wait_set_get_entities_ready(
    nullptr,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_WAIT_SET_INVALID);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    nullptr,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    nullptr,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    nullptr,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    nullptr,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    nullptr,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  // Even though they should be different subscriptions and clients, we can mock the correct
  // behavior by assigning all three clients to the same index, and both subscriptions as well
  wait_set.size_of_subscriptions = 1;
  wait_set.size_of_clients = 1;

  // Check wait indices are out of bounds
  action_client.impl->wait_set_feedback_subscription_index = 10;
  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_ERROR);
  rcl_reset_error();
  action_client.impl->wait_set_feedback_subscription_index = 0;

  action_client.impl->wait_set_status_subscription_index = 10;
  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_ERROR);
  rcl_reset_error();
  action_client.impl->wait_set_status_subscription_index = 0;

  action_client.impl->wait_set_goal_client_index = 10;
  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_ERROR);
  rcl_reset_error();
  action_client.impl->wait_set_goal_client_index = 0;

  action_client.impl->wait_set_cancel_client_index = 10;
  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_ERROR);
  rcl_reset_error();
  action_client.impl->wait_set_cancel_client_index = 0;

  action_client.impl->wait_set_result_client_index = 10;
  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_ERROR);
  rcl_reset_error();
  action_client.impl->wait_set_result_client_index = 0;

  ret = rcl_action_client_wait_set_get_entities_ready(
    &wait_set,
    &action_client,
    &is_feedback_ready,
    &is_status_ready,
    &is_goal_response_ready,
    &is_cancel_response_ready,
    &is_result_response_ready);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_FALSE(is_feedback_ready);
  EXPECT_FALSE(is_status_ready);
  EXPECT_FALSE(is_goal_response_ready);
  EXPECT_FALSE(is_cancel_response_ready);
  EXPECT_FALSE(is_result_response_ready);
}

TEST_F(TestActionServerWait, test_server_wait_set_get_entities_ready) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;
  });

  bool is_goal_request_ready = false;
  bool is_cancel_request_ready = false;
  bool is_result_request_ready = false;
  bool is_goal_expired = false;

  rcl_ret_t ret = rcl_action_server_wait_set_get_entities_ready(
    nullptr,
    &this->action_server,
    &is_goal_request_ready,
    &is_cancel_request_ready,
    &is_result_request_ready,
    &is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_WAIT_SET_INVALID);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_entities_ready(
    &wait_set,
    nullptr,
    &is_goal_request_ready,
    &is_cancel_request_ready,
    &is_result_request_ready,
    &is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_entities_ready(
    &wait_set,
    &this->action_server,
    nullptr,
    &is_cancel_request_ready,
    &is_result_request_ready,
    &is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_entities_ready(
    &wait_set,
    &this->action_server,
    &is_goal_request_ready,
    nullptr,
    &is_result_request_ready,
    &is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  ret = rcl_action_server_wait_set_get_entities_ready(
    &wait_set,
    &this->action_server,
    &is_goal_request_ready,
    &is_cancel_request_ready,
    nullptr,
    &is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();


  ret = rcl_action_server_wait_set_get_entities_ready(
    &wait_set,
    &this->action_server,
    &is_goal_request_ready,
    &is_cancel_request_ready,
    &is_result_request_ready,
    nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);

  ret = rcl_wait_set_init(&wait_set, 0, 0, 1, 0, 3, 0, &this->context, rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  wait_set.services[0] = &this->action_server.impl->goal_service;
  this->action_server.impl->wait_set_goal_service_index = 0;
  wait_set.services[1] = &this->action_server.impl->cancel_service;
  this->action_server.impl->wait_set_cancel_service_index = 1;
  wait_set.services[2] = &this->action_server.impl->result_service;
  this->action_server.impl->wait_set_result_service_index = 2;
  wait_set.timers[0] = &this->action_server.impl->expire_timer;
  this->action_server.impl->wait_set_expire_timer_index = 0;
  ret = rcl_action_server_wait_set_get_entities_ready(
    &wait_set,
    &this->action_server,
    &is_goal_request_ready,
    &is_cancel_request_ready,
    &is_result_request_ready,
    &is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_TRUE(is_goal_request_ready);
  EXPECT_TRUE(is_cancel_request_ready);
  EXPECT_TRUE(is_result_request_ready);
  EXPECT_TRUE(is_goal_expired);
}
