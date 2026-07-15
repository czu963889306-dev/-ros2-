// Copyright 2018 Open Source Robotics Foundation, Inc.
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
#include <thread>
#include <vector>

#include "action_msgs/srv/cancel_goal.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcl_action/action_server.h"
#include "rcl_action/action_server_impl.h"

#include "rcl/error_handling.h"
#include "rcl/rcl.h"

#include "test_msgs/action/fibonacci.h"

void * bad_malloc(size_t, void *)
{
  return nullptr;
}

void * bad_realloc(void *, size_t, void *)
{
  return nullptr;
}

void * bad_calloc(size_t, size_t, void *)
{
  printf("Returning null ptr\n");
  return nullptr;
}

TEST(TestActionServerInitFini, test_action_server_init_fini)
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret;
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_node_options_t node_options = rcl_node_get_default_options();
  ret = rcl_node_init(&node, "test_action_server_node", "", &context, &node_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_clock_t clock;
  ret = rcl_clock_init(RCL_STEADY_TIME, &clock, &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  const rosidl_action_type_support_t * ts = ROSIDL_GET_ACTION_TYPE_SUPPORT(test_msgs, Fibonacci);
  const rcl_action_server_options_t options = rcl_action_server_get_default_options();
  const char * action_name = "test_action_server_name";
  rcl_action_server_t action_server = rcl_action_get_zero_initialized_server();

  // Initialize with a null action server
  ret = rcl_action_server_init(nullptr, &node, &clock, ts, action_name, &options);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with a null node
  ret = rcl_action_server_init(&action_server, nullptr, &clock, ts, action_name, &options);
  EXPECT_EQ(ret, RCL_RET_NODE_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with an invalid node
  rcl_node_t invalid_node = rcl_get_zero_initialized_node();
  ret = rcl_action_server_init(&action_server, &invalid_node, &clock, ts, action_name, &options);
  EXPECT_EQ(ret, RCL_RET_NODE_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with a null clock
  ret = rcl_action_server_init(&action_server, &node, nullptr, ts, action_name, &options);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with an invalid clock
  rcl_clock_t invalid_clock;
  invalid_clock.get_now = nullptr;
  ret = rcl_action_server_init(&action_server, &node, &invalid_clock, ts, action_name, &options);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with a null typesupport
  ret = rcl_action_server_init(&action_server, &node, &clock, nullptr, action_name, &options);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with a null name
  ret = rcl_action_server_init(&action_server, &node, &clock, ts, nullptr, &options);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with an empty name
  const char * empty_action_name = "";
  ret = rcl_action_server_init(&action_server, &node, &clock, ts, empty_action_name, &options);
  EXPECT_EQ(ret, RCL_RET_ACTION_NAME_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with an invalid name
  const char * invalid_action_name = "42";
  ret = rcl_action_server_init(&action_server, &node, &clock, ts, invalid_action_name, &options);
  EXPECT_EQ(ret, RCL_RET_ACTION_NAME_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with a null options
  ret = rcl_action_server_init(&action_server, &node, &clock, ts, action_name, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with an invalid timeout
  rcl_action_server_options_t bad_options = rcl_action_server_get_default_options();
  bad_options.result_timeout.nanoseconds = -1;
  ret = rcl_action_server_init(&action_server, &node, &clock, ts, action_name, &bad_options);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Initialize with valid arguments
  ret = rcl_action_server_init(&action_server, &node, &clock, ts, action_name, &options);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Try to initialize again
  ret = rcl_action_server_init(&action_server, &node, &clock, ts, action_name, &options);
  EXPECT_EQ(ret, RCL_RET_ALREADY_INIT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Finalize with null action server
  ret = rcl_action_server_fini(nullptr, &node);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Finalize with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_server_fini(&invalid_action_server, &node);
  // Nothing happens
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Finalize with null node
  ret = rcl_action_server_fini(&action_server, nullptr);
  EXPECT_EQ(ret, RCL_RET_NODE_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Finalize with valid arguments
  ret = rcl_action_server_fini(&action_server, &node);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Finalize clock
  ret = rcl_clock_fini(&clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Finalize init_options
  ret = rcl_init_options_fini(&init_options);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Finalize node
  ret = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Shutdown node
  ret = rcl_shutdown(&context);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Finalize context
  ret = rcl_context_fini(&context);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

class TestActionServer : public ::testing::Test
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

TEST_F(TestActionServer, test_action_server_is_valid)
{
  // Check with null pointer
  bool is_valid = rcl_action_server_is_valid(nullptr);
  EXPECT_FALSE(is_valid) << rcl_get_error_string().str;
  rcl_reset_error();

  // Check with uninitialized action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  is_valid = rcl_action_server_is_valid(&invalid_action_server);
  EXPECT_FALSE(is_valid) << rcl_get_error_string().str;
  rcl_reset_error();

  // Check valid action server
  is_valid = rcl_action_server_is_valid(&this->action_server);
  EXPECT_TRUE(is_valid) << rcl_get_error_string().str;

  rcl_service_impl_t * tmp_service = this->action_server.impl->goal_service.impl;
  this->action_server.impl->goal_service.impl = nullptr;
  is_valid = rcl_action_server_is_valid(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->goal_service.impl = tmp_service;

  tmp_service = this->action_server.impl->cancel_service.impl;
  this->action_server.impl->cancel_service.impl = nullptr;
  is_valid = rcl_action_server_is_valid(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->cancel_service.impl = tmp_service;

  tmp_service = this->action_server.impl->result_service.impl;
  this->action_server.impl->result_service.impl = nullptr;
  is_valid = rcl_action_server_is_valid(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->result_service.impl = tmp_service;

  rcl_publisher_impl_t * tmp_publisher = this->action_server.impl->feedback_publisher.impl;
  this->action_server.impl->feedback_publisher.impl = nullptr;
  is_valid = rcl_action_server_is_valid(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->feedback_publisher.impl = tmp_publisher;

  tmp_publisher = this->action_server.impl->status_publisher.impl;
  this->action_server.impl->status_publisher.impl = nullptr;
  is_valid = rcl_action_server_is_valid(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->status_publisher.impl = tmp_publisher;
}

TEST_F(TestActionServer, test_action_server_is_valid_except_context)
{
  // Check with null pointer
  bool is_valid = rcl_action_server_is_valid_except_context(nullptr);
  EXPECT_FALSE(is_valid) << rcl_get_error_string().str;
  rcl_reset_error();

  // Check with uninitialized action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  is_valid = rcl_action_server_is_valid_except_context(&invalid_action_server);
  EXPECT_FALSE(is_valid) << rcl_get_error_string().str;
  rcl_reset_error();

  // Check valid action server
  is_valid = rcl_action_server_is_valid_except_context(&this->action_server);
  EXPECT_TRUE(is_valid) << rcl_get_error_string().str;

  rcl_service_impl_t * tmp_service = this->action_server.impl->goal_service.impl;
  this->action_server.impl->goal_service.impl = nullptr;
  is_valid = rcl_action_server_is_valid_except_context(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->goal_service.impl = tmp_service;

  tmp_service = this->action_server.impl->cancel_service.impl;
  this->action_server.impl->cancel_service.impl = nullptr;
  is_valid = rcl_action_server_is_valid_except_context(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->cancel_service.impl = tmp_service;

  tmp_service = this->action_server.impl->result_service.impl;
  this->action_server.impl->result_service.impl = nullptr;
  is_valid = rcl_action_server_is_valid_except_context(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->result_service.impl = tmp_service;

  rcl_publisher_impl_t * tmp_publisher = this->action_server.impl->feedback_publisher.impl;
  this->action_server.impl->feedback_publisher.impl = nullptr;
  is_valid = rcl_action_server_is_valid_except_context(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->feedback_publisher.impl = tmp_publisher;

  tmp_publisher = this->action_server.impl->status_publisher.impl;
  this->action_server.impl->status_publisher.impl = nullptr;
  is_valid = rcl_action_server_is_valid_except_context(&this->action_server);
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  this->action_server.impl->status_publisher.impl = tmp_publisher;
}

TEST_F(TestActionServer, test_action_accept_new_goal)
{
  // Initialize a goal info
  rcl_action_goal_info_t goal_info_in = rcl_action_get_zero_initialized_goal_info();
  init_test_uuid0(goal_info_in.goal_id.uuid);

  // Accept goal with a null action server
  rcl_action_goal_handle_t * goal_handle = rcl_action_accept_new_goal(nullptr, &goal_info_in);
  EXPECT_EQ(goal_handle, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Accept goal with null goal info
  goal_handle = rcl_action_accept_new_goal(&this->action_server, nullptr);
  EXPECT_EQ(goal_handle, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Accept goal with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  goal_handle = rcl_action_accept_new_goal(&invalid_action_server, &goal_info_in);
  EXPECT_EQ(goal_handle, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Check failing allocation of goal_handle
  this->action_server.impl->options.allocator.allocate = bad_malloc;
  goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  EXPECT_EQ(goal_handle, nullptr);
  this->action_server.impl->options.allocator.allocate =
    rcl_get_default_allocator().allocate;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Check failing reallocate of goal_handles
  this->action_server.impl->options.allocator.reallocate = bad_realloc;
  goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  EXPECT_EQ(goal_handle, nullptr);
  this->action_server.impl->options.allocator.reallocate =
    rcl_get_default_allocator().reallocate;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  std::vector<rcl_action_goal_handle_t> handles;

  // Accept with valid arguments
  goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  EXPECT_NE(goal_handle, nullptr) << rcl_get_error_string().str;
  handles.push_back(*goal_handle);
  rcl_action_goal_info_t goal_info_out = rcl_action_get_zero_initialized_goal_info();
  rcl_ret_t ret = rcl_action_goal_handle_get_info(goal_handle, &goal_info_out);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_TRUE(uuidcmp(goal_info_out.goal_id.uuid, goal_info_in.goal_id.uuid));
  size_t num_goals = 0u;
  rcl_action_goal_handle_t ** goal_handle_array = {nullptr};

  // Check invalid action server
  ret = rcl_action_server_get_goal_handles(nullptr, &goal_handle_array, &num_goals);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  ret = rcl_action_server_get_goal_handles(&this->action_server, &goal_handle_array, &num_goals);
  ASSERT_EQ(ret, RCL_RET_OK);
  EXPECT_EQ(num_goals, 1u);
  EXPECT_NE(goal_handle_array, nullptr) << rcl_get_error_string().str;
  EXPECT_NE(goal_handle_array[0], nullptr) << rcl_get_error_string().str;

  // Accept with the same goal ID
  goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  EXPECT_EQ(goal_handle, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Accept a different goal
  goal_info_in = rcl_action_get_zero_initialized_goal_info();
  init_test_uuid1(goal_info_in.goal_id.uuid);
  goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  EXPECT_NE(goal_handle, nullptr) << rcl_get_error_string().str;
  handles.push_back(*goal_handle);
  ret = rcl_action_goal_handle_get_info(goal_handle, &goal_info_out);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_TRUE(uuidcmp(goal_info_out.goal_id.uuid, goal_info_in.goal_id.uuid));
  ret = rcl_action_server_get_goal_handles(&this->action_server, &goal_handle_array, &num_goals);
  ASSERT_EQ(ret, RCL_RET_OK);
  EXPECT_EQ(num_goals, 2u);
  EXPECT_NE(goal_handle_array, nullptr) << rcl_get_error_string().str;
  EXPECT_NE(goal_handle_array[0], nullptr) << rcl_get_error_string().str;
  EXPECT_NE(goal_handle_array[1], nullptr) << rcl_get_error_string().str;

  for (auto & handle : handles) {
    EXPECT_EQ(RCL_RET_OK, rcl_action_goal_handle_fini(&handle));
    EXPECT_FALSE(rcl_error_is_set()) << rcl_get_error_string().str;
  }
}

TEST_F(TestActionServer, test_action_server_goal_exists) {
  rcl_action_goal_info_t goal_info_out = rcl_action_get_zero_initialized_goal_info();
  EXPECT_FALSE(rcl_action_server_goal_exists(nullptr, &goal_info_out));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_FALSE(rcl_action_server_goal_exists(&this->action_server, nullptr));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize a goal info
  rcl_action_goal_info_t goal_info_in = rcl_action_get_zero_initialized_goal_info();
  init_test_uuid0(goal_info_in.goal_id.uuid);

  // Add new goal
  rcl_action_goal_handle_t * goal_handle =
    rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  EXPECT_NE(goal_handle, nullptr) << rcl_get_error_string().str;

  // Check exists
  EXPECT_TRUE(rcl_action_server_goal_exists(&this->action_server, &goal_info_in));

  rcl_action_goal_info_t different_goal = rcl_action_get_zero_initialized_goal_info();
  init_test_uuid1(goal_info_in.goal_id.uuid);

  // Check doesn't exist
  EXPECT_FALSE(rcl_action_server_goal_exists(&this->action_server, &different_goal));
  EXPECT_FALSE(rcl_error_is_set()) << rcl_get_error_string().str;

  // Check corrupted goal_handles
  rcl_get_default_allocator().deallocate(goal_handle, rcl_get_default_allocator().state);
  this->action_server.impl->goal_handles[this->action_server.impl->num_goal_handles - 1] = nullptr;
  EXPECT_FALSE(rcl_action_server_goal_exists(&this->action_server, &different_goal));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Reset for teardown
  this->action_server.impl->num_goal_handles--;
}

TEST_F(TestActionServer, test_action_server_notify_goal_done) {
  // Invalid action server
  EXPECT_EQ(RCL_RET_ACTION_SERVER_INVALID, rcl_action_notify_goal_done(nullptr));
  rcl_reset_error();

  // No goals yet, should be ok
  EXPECT_EQ(RCL_RET_OK, rcl_action_notify_goal_done(&action_server));

  rcl_action_goal_info_t goal_info_in = rcl_action_get_zero_initialized_goal_info();
  init_test_uuid0(goal_info_in.goal_id.uuid);

  // Add new goal
  rcl_action_goal_handle_t * goal_handle =
    rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  EXPECT_NE(goal_handle, nullptr) << rcl_get_error_string().str;

  // One goal, should be able to notify
  EXPECT_EQ(RCL_RET_OK, rcl_action_notify_goal_done(&action_server));

  // Invalid goal handle
  rcl_get_default_allocator().deallocate(goal_handle, rcl_get_default_allocator().state);
  this->action_server.impl->goal_handles[this->action_server.impl->num_goal_handles - 1] = nullptr;
  EXPECT_EQ(RCL_RET_ERROR, rcl_action_notify_goal_done(&action_server));
  rcl_reset_error();

  // Reset for teardown
  this->action_server.impl->num_goal_handles--;
}

TEST_F(TestActionServer, test_action_clear_expired_goals)
{
  const size_t capacity = 1u;
  rcl_action_goal_info_t expired_goals[1u];
  size_t num_expired = 42u;
  // Clear expired goals with null action server
  rcl_ret_t ret = rcl_action_expire_goals(nullptr, expired_goals, capacity, &num_expired);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Clear with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_expire_goals(&invalid_action_server, expired_goals, capacity, &num_expired);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Clear with invalid arguments
  ret = rcl_action_expire_goals(&this->action_server, nullptr, capacity, &num_expired);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  EXPECT_EQ(num_expired, 42u);
  rcl_reset_error();

  // Clear with invalid arguments
  ret = rcl_action_expire_goals(&this->action_server, expired_goals, 0u, &num_expired);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  EXPECT_EQ(num_expired, 42u);
  rcl_reset_error();

  // Clear with invalid arguments
  ret = rcl_action_expire_goals(&this->action_server, expired_goals, capacity, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  EXPECT_EQ(num_expired, 42u);
  rcl_reset_error();

  // Clear with valid arguments
  ret = rcl_action_expire_goals(&this->action_server, expired_goals, capacity, &num_expired);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(num_expired, 0u);

  // Clear with valid arguments (optional num_expired)
  ret = rcl_action_expire_goals(&this->action_server, nullptr, 0u, nullptr);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  std::vector<rcl_action_goal_handle_t> handles;

  // Test with goals that actually expire
  // Set ROS time
  ASSERT_EQ(RCL_RET_OK, rcl_enable_ros_time_override(&this->clock));
  ASSERT_EQ(RCL_RET_OK, rcl_set_ros_time_override(&this->clock, RCUTILS_S_TO_NS(1)));

  // Accept a goal to create a new handle
  rcl_action_goal_info_t goal_info_in = rcl_action_get_zero_initialized_goal_info();
  init_test_uuid1(goal_info_in.goal_id.uuid);
  rcl_action_goal_handle_t * goal_handle =
    rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  ASSERT_NE(goal_handle, nullptr) << rcl_get_error_string().str;
  handles.push_back(*goal_handle);

  // Transition executing to aborted
  ASSERT_EQ(RCL_RET_OK, rcl_action_update_goal_state(goal_handle, GOAL_EVENT_EXECUTE));
  ASSERT_EQ(RCL_RET_OK, rcl_action_update_goal_state(goal_handle, GOAL_EVENT_ABORT));

  // Set time to something far in the future
  ASSERT_EQ(RCL_RET_OK, rcl_set_ros_time_override(&this->clock, RCUTILS_S_TO_NS(99999)));

  // Clear with valid arguments
  ret = rcl_action_expire_goals(&this->action_server, expired_goals, capacity, &num_expired);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(num_expired, 1u);
  EXPECT_TRUE(uuidcmp(expired_goals[0].goal_id.uuid, goal_info_in.goal_id.uuid));

  for (auto & handle : handles) {
    EXPECT_EQ(RCL_RET_OK, rcl_action_goal_handle_fini(&handle));
  }
}

TEST_F(TestActionServer, test_action_process_cancel_request)
{
  rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
  rcl_action_cancel_response_t cancel_response = rcl_action_get_zero_initialized_cancel_response();

  // Process cancel request with null action server
  rcl_ret_t ret = rcl_action_process_cancel_request(nullptr, &cancel_request, &cancel_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID);
  rcl_reset_error();

  // Process cancel request with null request message
  ret = rcl_action_process_cancel_request(&this->action_server, nullptr, &cancel_response);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  // Process cancel request with null response message
  ret = rcl_action_process_cancel_request(&this->action_server, &cancel_request, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  this->action_server.impl->options.allocator.allocate = bad_malloc;
  // Process cancel request with bad allocator
  ret = rcl_action_process_cancel_request(&this->action_server, &cancel_request, &cancel_response);
  EXPECT_EQ(ret, RCL_RET_BAD_ALLOC);
  rcl_reset_error();
  this->action_server.impl->options.allocator = rcl_get_default_allocator();

  // Process cancel request with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_process_cancel_request(
    &invalid_action_server, &cancel_request, &cancel_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID);
  rcl_reset_error();

  // Process cancel request with valid arguments (but no goals to cancel)
  ret = rcl_action_process_cancel_request(
    &this->action_server, &cancel_request, &cancel_response);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  // A zero request means "cancel all goals", which succeeds if there's nothing to cancel
  EXPECT_EQ(cancel_response.msg.return_code, action_msgs__srv__CancelGoal_Response__ERROR_NONE);

  // Number of goals is not 0, but goal handle is null, for case with request_nanosec == 0
  size_t num_goal_handles = 1u;
  rcl_allocator_t allocator = this->action_server.impl->options.allocator;
  this->action_server.impl->num_goal_handles = num_goal_handles;
  this->action_server.impl->goal_handles = reinterpret_cast<rcl_action_goal_handle_t **>(
    allocator.zero_allocate(num_goal_handles, sizeof(rcl_action_goal_handle_t *), allocator.state));
  ret = rcl_action_process_cancel_request(
    &this->action_server, &cancel_request, &cancel_response);
  EXPECT_EQ(ret, RCL_RET_ERROR);
  rcl_reset_error();

  // Number of goals is not 0, but goal handle is null, for case with request_nanosec > 0
  cancel_request.goal_info.stamp.nanosec = 1;
  ret = rcl_action_process_cancel_request(
    &this->action_server, &cancel_request, &cancel_response);
  EXPECT_EQ(ret, RCL_RET_ERROR);
  rcl_reset_error();
}

TEST_F(TestActionServer, test_action_server_get_goal_status_array)
{
  rcl_action_goal_status_array_t status_array =
    rcl_action_get_zero_initialized_goal_status_array();
  // Get with null action server
  rcl_ret_t ret = rcl_action_get_goal_status_array(nullptr, &status_array);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID);
  rcl_reset_error();

  // Get with null status array
  ret = rcl_action_get_goal_status_array(&this->action_server, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  // Get with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_get_goal_status_array(&invalid_action_server, nullptr);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID);
  rcl_reset_error();

  // Check goal_status_array_init fails
  this->action_server.impl->num_goal_handles = 1u;
  this->action_server.impl->options.allocator.zero_allocate = bad_calloc;
  ret = rcl_action_get_goal_status_array(&this->action_server, &status_array);
  EXPECT_EQ(ret, RCL_RET_BAD_ALLOC);
  rcl_reset_error();
  this->action_server.impl->options.allocator = rcl_get_default_allocator();

  // Check status_message is already inited
  this->action_server.impl->num_goal_handles = 1u;
  status_array.msg.status_list.size = 1;
  ret = rcl_action_get_goal_status_array(&this->action_server, &status_array);
  EXPECT_EQ(ret, RCL_RET_ERROR);
  rcl_reset_error();
  status_array.msg.status_list.size = 0;
  this->action_server.impl->num_goal_handles = 0u;

  // Get with valid arguments (but not goals being tracked)
  ret = rcl_action_get_goal_status_array(&this->action_server, &status_array);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(status_array.msg.status_list.data, nullptr);
  EXPECT_EQ(status_array.msg.status_list.size, 0u);
  ret = rcl_action_goal_status_array_fini(&status_array);
  EXPECT_EQ(ret, RCL_RET_OK);

  std::vector<rcl_action_goal_handle_t> handles;

  // Add a goal before getting the status array
  rcl_action_goal_info_t goal_info_in = rcl_action_get_zero_initialized_goal_info();
  init_test_uuid0(goal_info_in.goal_id.uuid);
  rcl_action_goal_handle_t * goal_handle;
  goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
  ASSERT_NE(goal_handle, nullptr) << rcl_get_error_string().str;
  handles.push_back(*goal_handle);
  ret = rcl_action_get_goal_status_array(&this->action_server, &status_array);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_NE(status_array.msg.status_list.data, nullptr);
  EXPECT_EQ(status_array.msg.status_list.size, 1u);
  rcl_action_goal_info_t * goal_info_out = &status_array.msg.status_list.data[0].goal_info;

  // Passing this array by pointer can confuse scan-build into thinking the pointer is
  // not checked for null. Passing by reference works fine
  const auto & goal_info_out_uuid = goal_info_out->goal_id.uuid;
  EXPECT_TRUE(uuidcmp(goal_info_out_uuid, goal_info_in.goal_id.uuid));
  ret = rcl_action_goal_status_array_fini(&status_array);
  EXPECT_EQ(ret, RCL_RET_OK);

  // Add nine more goals
  for (int i = 1; i < 10; ++i) {
    for (int j = 0; j < UUID_SIZE; ++j) {
      goal_info_in.goal_id.uuid[j] = static_cast<uint8_t>(i + j);
    }
    goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
    ASSERT_NE(goal_handle, nullptr) << rcl_get_error_string().str;
    handles.push_back(*goal_handle);
  }
  ret = rcl_action_get_goal_status_array(&this->action_server, &status_array);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_NE(status_array.msg.status_list.data, nullptr);
  ASSERT_EQ(status_array.msg.status_list.size, 10u);
  for (int i = 0; i < 10; ++i) {
    goal_info_out = &status_array.msg.status_list.data[i].goal_info;
    for (int j = 0; j < UUID_SIZE; ++j) {
      EXPECT_EQ(goal_info_out->goal_id.uuid[j], i + j);
    }
  }
  ret = rcl_action_goal_status_array_fini(&status_array);
  ASSERT_EQ(ret, RCL_RET_OK);
  for (auto & handle : handles) {
    EXPECT_EQ(RCL_RET_OK, rcl_action_goal_handle_fini(&handle));
  }
}

TEST_F(TestActionServer, test_action_server_get_action_name)
{
  // Get action_name for a null action server
  const char * action_name = rcl_action_server_get_action_name(nullptr);
  EXPECT_EQ(action_name, nullptr);
  rcl_reset_error();

  // Get action_name for an invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  action_name = rcl_action_server_get_action_name(&invalid_action_server);
  EXPECT_EQ(action_name, nullptr);
  rcl_reset_error();

  // Get action_name for a valid action server
  action_name = rcl_action_server_get_action_name(&this->action_server);
  ASSERT_NE(action_name, nullptr) << rcl_get_error_string().str;
  EXPECT_STREQ(action_name, "test_action_server_name");
}

TEST_F(TestActionServer, test_action_server_get_options)
{
  // Get options for a null action server
  const rcl_action_server_options_t * options = rcl_action_server_get_options(nullptr);
  EXPECT_EQ(options, nullptr);
  rcl_reset_error();

  // Get options for an invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  options = rcl_action_server_get_options(&invalid_action_server);
  EXPECT_EQ(options, nullptr);
  rcl_reset_error();

  // Get options for a valid action server
  options = rcl_action_server_get_options(&this->action_server);
  EXPECT_NE(options, nullptr) << rcl_get_error_string().str;
}

class TestActionServerCancelPolicy : public TestActionServer
{
protected:
  void SetUp() override
  {
    TestActionServer::SetUp();
    // Add several goals
    rcl_action_goal_info_t goal_info_in = rcl_action_get_zero_initialized_goal_info();
    rcl_action_goal_handle_t * goal_handle;
    rcl_ret_t ret;
    for (int i = 0; i < NUM_GOALS; ++i) {
      for (int j = 0; j < UUID_SIZE; ++j) {
        goal_info_in.goal_id.uuid[j] = static_cast<uint8_t>(i + j);
      }
      goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info_in);
      ASSERT_NE(goal_handle, nullptr) << rcl_get_error_string().str;
      handles.push_back(*goal_handle);
      goal_infos_out[i] = rcl_action_get_zero_initialized_goal_info();
      ret = rcl_action_goal_handle_get_info(goal_handle, &goal_infos_out[i]);
      ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
      // Sleep so goals have different acceptance times
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  void TearDown() override
  {
    for (auto & handle : handles) {
      EXPECT_EQ(RCL_RET_OK, rcl_action_goal_handle_fini(&handle));
    }
    TestActionServer::TearDown();
  }

  static const int NUM_GOALS = 10;
  rcl_action_goal_info_t goal_infos_out[NUM_GOALS];
  std::vector<rcl_action_goal_handle_t> handles;
};

TEST_F(TestActionServerCancelPolicy, test_action_process_cancel_request_all_goals)
{
  // Request to cancel all goals
  rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
  cancel_request.goal_info.stamp.sec = 0;
  cancel_request.goal_info.stamp.nanosec = 0u;
  rcl_action_cancel_response_t cancel_response = rcl_action_get_zero_initialized_cancel_response();
  rcl_ret_t ret = rcl_action_process_cancel_request(
    &this->action_server, &cancel_request, &cancel_response);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_NE(cancel_response.msg.goals_canceling.data, nullptr);
  ASSERT_EQ(cancel_response.msg.goals_canceling.size, (size_t)NUM_GOALS);
  EXPECT_EQ(cancel_response.msg.return_code, action_msgs__srv__CancelGoal_Response__ERROR_NONE);
  rcl_action_goal_info_t * goal_info_out;
  for (int i = 0; i < NUM_GOALS; ++i) {
    goal_info_out = &cancel_response.msg.goals_canceling.data[i];
    for (int j = 0; j < UUID_SIZE; ++j) {
      EXPECT_EQ(goal_info_out->goal_id.uuid[j], static_cast<uint8_t>(i + j));
    }
  }
  EXPECT_EQ(RCL_RET_OK, rcl_action_cancel_response_fini(&cancel_response));
}

TEST_F(TestActionServerCancelPolicy, test_action_process_cancel_request_single_goal)
{
  {
    // Request to cancel a specific goal
    rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
    init_test_uuid0(cancel_request.goal_info.goal_id.uuid);
    rcl_action_cancel_response_t cancel_response =
      rcl_action_get_zero_initialized_cancel_response();
    rcl_ret_t ret = rcl_action_process_cancel_request(
      &this->action_server, &cancel_request, &cancel_response);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_NE(cancel_response.msg.goals_canceling.data, nullptr);
    ASSERT_EQ(cancel_response.msg.goals_canceling.size, 1u);
    EXPECT_EQ(cancel_response.msg.return_code, action_msgs__srv__CancelGoal_Response__ERROR_NONE);
    rcl_action_goal_info_t * goal_info = &cancel_response.msg.goals_canceling.data[0];

    // Passing this array by pointer can confuse scan-build into thinking the pointer is
    // not checked for null. Passing by reference works fine
    const auto & goal_info_uuid = goal_info->goal_id.uuid;
    EXPECT_TRUE(uuidcmp(goal_info_uuid, cancel_request.goal_info.goal_id.uuid));
    EXPECT_EQ(RCL_RET_OK, rcl_action_cancel_response_fini(&cancel_response));
  }
  {
    // Request to cancel an invalid goal
    rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
    init_test_uuid1(cancel_request.goal_info.goal_id.uuid);
    rcl_action_cancel_response_t cancel_response =
      rcl_action_get_zero_initialized_cancel_response();
    rcl_ret_t ret = rcl_action_process_cancel_request(
      &this->action_server, &cancel_request, &cancel_response);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
    EXPECT_EQ(cancel_response.msg.goals_canceling.size, 0u);
    EXPECT_EQ(
      cancel_response.msg.return_code,
      action_msgs__srv__CancelGoal_Response__ERROR_UNKNOWN_GOAL_ID);
    EXPECT_EQ(RCL_RET_OK, rcl_action_cancel_response_fini(&cancel_response));
  }
  {
    // Request to cancel a terminated goal
    // First, transition a goal handle to a terminal state
    rcl_ret_t ret = rcl_action_update_goal_state(&this->handles[3], GOAL_EVENT_EXECUTE);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_action_update_goal_state(&this->handles[3], GOAL_EVENT_SUCCEED);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    // Attempt to cancel the terminated goal
    rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
    cancel_request.goal_info.goal_id = this->goal_infos_out[3].goal_id;
    rcl_action_cancel_response_t cancel_response =
      rcl_action_get_zero_initialized_cancel_response();
    ret = rcl_action_process_cancel_request(
      &this->action_server, &cancel_request, &cancel_response);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
    EXPECT_EQ(cancel_response.msg.goals_canceling.size, 0u);
    EXPECT_EQ(
      cancel_response.msg.return_code,
      action_msgs__srv__CancelGoal_Response__ERROR_GOAL_TERMINATED);
    EXPECT_EQ(RCL_RET_OK, rcl_action_cancel_response_fini(&cancel_response));
  }
}

TEST_F(TestActionServerCancelPolicy, test_action_process_cancel_request_by_time)
{
  // Request to cancel all goals at and before a specific time
  const size_t time_index = 7;
  rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
  cancel_request.goal_info = this->goal_infos_out[time_index];
  rcl_action_cancel_response_t cancel_response = rcl_action_get_zero_initialized_cancel_response();
  rcl_ret_t ret = rcl_action_process_cancel_request(
    &this->action_server, &cancel_request, &cancel_response);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_NE(cancel_response.msg.goals_canceling.data, nullptr);
  ASSERT_EQ(cancel_response.msg.goals_canceling.size, time_index + 1);  // goals at indices [0, 7]
  EXPECT_EQ(cancel_response.msg.return_code, action_msgs__srv__CancelGoal_Response__ERROR_NONE);
  rcl_action_goal_info_t * goal_info_out;
  for (size_t i = 0; i < cancel_response.msg.goals_canceling.size; ++i) {
    goal_info_out = &cancel_response.msg.goals_canceling.data[i];
    for (size_t j = 0; j < UUID_SIZE; ++j) {
      EXPECT_EQ(goal_info_out->goal_id.uuid[j], static_cast<uint8_t>(i + j));
    }
  }
  EXPECT_EQ(RCL_RET_OK, rcl_action_cancel_response_fini(&cancel_response));
}

TEST_F(TestActionServerCancelPolicy, test_action_process_cancel_request_by_time_and_id)
{
  // Request to cancel a specific goal by ID and all goals at and before a specific time
  const size_t goal_index = 9;
  const size_t time_index = 2;
  rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
  cancel_request.goal_info = this->goal_infos_out[time_index];
  for (int i = 0; i < UUID_SIZE; ++i) {
    cancel_request.goal_info.goal_id.uuid[i] = static_cast<uint8_t>(i + goal_index);
  }
  rcl_action_cancel_response_t cancel_response = rcl_action_get_zero_initialized_cancel_response();
  rcl_ret_t ret = rcl_action_process_cancel_request(
    &this->action_server, &cancel_request, &cancel_response);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_NE(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.return_code, action_msgs__srv__CancelGoal_Response__ERROR_NONE);
  const size_t num_goals_canceling = cancel_response.msg.goals_canceling.size;
  ASSERT_EQ(num_goals_canceling, time_index + 2);  // goals at indices [0, 2] and 8
  rcl_action_goal_info_t * goal_info_out;
  for (size_t i = 0; i < num_goals_canceling - 1; ++i) {
    goal_info_out = &cancel_response.msg.goals_canceling.data[i];
    for (size_t j = 0; j < UUID_SIZE; ++j) {
      EXPECT_EQ(goal_info_out->goal_id.uuid[j], static_cast<uint8_t>(i + j));
    }
  }
  goal_info_out = &cancel_response.msg.goals_canceling.data[num_goals_canceling - 1];
  EXPECT_TRUE(uuidcmp(goal_info_out->goal_id.uuid, cancel_request.goal_info.goal_id.uuid));
  EXPECT_EQ(RCL_RET_OK, rcl_action_cancel_response_fini(&cancel_response));
}

TEST_F(TestActionServer, action_server_init_fini_maybe_fail)
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options));
  });

  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_node_options_t node_options = rcl_node_get_default_options();
  ret = rcl_node_init(&node, "test_action_server_node", "", &context, &node_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node));
    EXPECT_EQ(RCL_RET_OK, rcl_node_options_fini(&node_options));
  });

  rcl_clock_t clock;
  ret = rcl_clock_init(RCL_STEADY_TIME, &clock, &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  const rosidl_action_type_support_t * ts = ROSIDL_GET_ACTION_TYPE_SUPPORT(test_msgs, Fibonacci);
  const rcl_action_server_options_t options = rcl_action_server_get_default_options();
  constexpr char action_name[] = "test_action_server_name";

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_action_server_t action_server = rcl_action_get_zero_initialized_server();
    rcl_ret_t ret = rcl_action_server_init(
      &action_server, &node, &clock, ts, action_name, &options);

    if (RCL_RET_OK == ret) {
      ret = rcl_action_server_fini(&action_server, &node);
    }
  });
}

TEST_F(TestActionServerCancelPolicy, test_action_process_cancel_request_maybe_fail)
{
  // Request to cancel all goals
  rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
  cancel_request.goal_info.stamp.sec = 0;
  cancel_request.goal_info.stamp.nanosec = 0u;
  rcl_action_cancel_response_t cancel_response = rcl_action_get_zero_initialized_cancel_response();

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_ret_t ret = rcl_action_process_cancel_request(
      &this->action_server, &cancel_request, &cancel_response);
    // Regardless of return, fini should be able to succeed
    (void)ret;
    EXPECT_EQ(RCL_RET_OK, rcl_action_cancel_response_fini(&cancel_response));
  });
}

TEST_F(TestActionServerCancelPolicy, test_action_expire_goals_maybe_fail)
{
  const size_t capacity = 10u;
  rcl_action_goal_info_t expired_goals[10u];
  size_t num_expired = 42u;

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_ret_t ret = rcl_action_expire_goals(
      &this->action_server, expired_goals, capacity, &num_expired);
    if (RCL_RET_OK != ret) {
      rcl_reset_error();
    }
  });
}
