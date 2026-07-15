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

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcl_action/action_client.h"
#include "rcl_action/action_server.h"
#include "rcl_action/wait.h"

#include "rcl/error_handling.h"
#include "rcl/rcl.h"

#include "rosidl_runtime_c/primitives_sequence_functions.h"

#include "test_msgs/action/fibonacci.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestActionCommunication, RMW_IMPLEMENTATION) : public ::testing::Test
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
    ret = rcl_node_init(&this->node, "test_action_communication_node", "", &context, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_clock_init(RCL_STEADY_TIME, &this->clock, &allocator);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    const rosidl_action_type_support_t * ts = ROSIDL_GET_ACTION_TYPE_SUPPORT(
      test_msgs, Fibonacci);
    const char * action_name = "test_action_commmunication_name";
    const rcl_action_server_options_t server_options = rcl_action_server_get_default_options();
    this->action_server = rcl_action_get_zero_initialized_server();
    ret = rcl_action_server_init(
      &this->action_server, &this->node, &this->clock, ts, action_name, &server_options);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    const rcl_action_client_options_t client_options = rcl_action_client_get_default_options();
    this->action_client = rcl_action_get_zero_initialized_client();
    ret = rcl_action_client_init(
      &this->action_client, &this->node, ts, action_name, &client_options);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

    size_t num_subscriptions_server;
    size_t num_guard_conditions_server;
    size_t num_timers_server;
    size_t num_clients_server;
    size_t num_services_server;
    size_t num_subscriptions_client;
    size_t num_guard_conditions_client;
    size_t num_timers_client;
    size_t num_clients_client;
    size_t num_services_client;

    this->wait_set = rcl_get_zero_initialized_wait_set();
    ret = rcl_action_server_wait_set_get_num_entities(
      &this->action_server,
      &num_subscriptions_server,
      &num_guard_conditions_server,
      &num_timers_server,
      &num_clients_server,
      &num_services_server);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_action_client_wait_set_get_num_entities(
      &this->action_client,
      &num_subscriptions_client,
      &num_guard_conditions_client,
      &num_timers_client,
      &num_clients_client,
      &num_services_client);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_wait_set_init(
      &this->wait_set,
      num_subscriptions_server + num_subscriptions_client,
      num_guard_conditions_server + num_guard_conditions_client,
      num_timers_server + num_timers_client,
      num_clients_server + num_clients_client,
      num_services_server + num_services_client,
      0 /* num_events_server + num_events_client */,
      &context,
      rcl_get_default_allocator());
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

    // Make sure action client has discovered action server.
    bool is_available = false;
    for (int i = 0; i < 10; ++i) {
      ret = rcl_action_server_is_available(
        &this->node, &this->action_client, &is_available);
      EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      rcl_reset_error();
      if (is_available) {
        break;
      }
      using namespace std::literals::chrono_literals;
      std::this_thread::sleep_for(100ms);
    }
    ASSERT_TRUE(is_available);
  }

  void TearDown() override
  {
    // Finalize
    rcl_ret_t ret = rcl_action_server_fini(&this->action_server, &this->node);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_clock_fini(&this->clock);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_action_client_fini(&this->action_client, &this->node);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_node_fini(&this->node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_wait_set_fini(&this->wait_set);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_shutdown(&context);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
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

  rcl_action_client_t action_client;
  rcl_action_server_t action_server;
  rcl_context_t context;
  rcl_node_t node;
  rcl_clock_t clock;

  rcl_wait_set_t wait_set;

  bool is_goal_request_ready;
  bool is_cancel_request_ready;
  bool is_result_request_ready;
  bool is_goal_expired;

  bool is_feedback_ready;
  bool is_status_ready;
  bool is_goal_response_ready;
  bool is_cancel_response_ready;
  bool is_result_response_ready;
};  // class TestActionCommunication

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_valid_goal_comm)
{
  test_msgs__action__Fibonacci_SendGoal_Request outgoing_goal_request;
  test_msgs__action__Fibonacci_SendGoal_Request incoming_goal_request;
  test_msgs__action__Fibonacci_SendGoal_Response outgoing_goal_response;
  test_msgs__action__Fibonacci_SendGoal_Response incoming_goal_response;
  test_msgs__action__Fibonacci_SendGoal_Request__init(&outgoing_goal_request);
  test_msgs__action__Fibonacci_SendGoal_Request__init(&incoming_goal_request);
  test_msgs__action__Fibonacci_SendGoal_Response__init(&outgoing_goal_response);
  test_msgs__action__Fibonacci_SendGoal_Response__init(&incoming_goal_response);

  // Initialize goal request
  init_test_uuid0(outgoing_goal_request.goal_id.uuid);
  outgoing_goal_request.goal.order = 10;

  // Send goal request with valid arguments
  int64_t sequence_number;
  rcl_ret_t ret = rcl_action_send_goal_request(
    &this->action_client, &outgoing_goal_request, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_server(&this->wait_set, &this->action_server, NULL);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));

  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_server_wait_set_get_entities_ready(
    &this->wait_set,
    &this->action_server,
    &this->is_goal_request_ready,
    &this->is_cancel_request_ready,
    &this->is_result_request_ready,
    &this->is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_TRUE(this->is_goal_request_ready) << rcl_get_error_string().str;
  EXPECT_FALSE(this->is_cancel_request_ready) << rcl_get_error_string().str;
  EXPECT_FALSE(this->is_result_request_ready) << rcl_get_error_string().str;

  // Take goal request with valid arguments
  rmw_request_id_t request_header;
  ret = rcl_action_take_goal_request(&this->action_server, &request_header, &incoming_goal_request);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Check that the goal request was received correctly
  EXPECT_EQ(outgoing_goal_request.goal.order, incoming_goal_request.goal.order);
  EXPECT_TRUE(
    uuidcmp(
      outgoing_goal_request.goal_id.uuid,
      incoming_goal_request.goal_id.uuid));

  // Initialize goal response
  outgoing_goal_response.accepted = true;
  outgoing_goal_response.stamp.sec = 123;
  outgoing_goal_response.stamp.nanosec = 456789u;

  // Send goal response with valid arguments
  ret = rcl_action_send_goal_response(
    &this->action_server, &request_header, &outgoing_goal_response);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait_set_clear(&this->wait_set);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_client(
    &this->wait_set, &this->action_client, NULL, NULL);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_client_wait_set_get_entities_ready(
    &this->wait_set,
    &this->action_client,
    &this->is_feedback_ready,
    &this->is_status_ready,
    &this->is_goal_response_ready,
    &this->is_cancel_response_ready,
    &this->is_result_response_ready);

  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_TRUE(this->is_goal_response_ready);
  EXPECT_FALSE(this->is_cancel_response_ready);
  EXPECT_FALSE(this->is_feedback_ready);
  EXPECT_FALSE(this->is_status_ready);
  EXPECT_FALSE(this->is_result_response_ready);

  // Take goal response with valid arguments
  ret = rcl_action_take_goal_response(
    &this->action_client, &request_header, &incoming_goal_response);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Check that the goal response was received correctly
  EXPECT_EQ(outgoing_goal_response.accepted, incoming_goal_response.accepted);
  EXPECT_EQ(outgoing_goal_response.stamp.sec, incoming_goal_response.stamp.sec);
  EXPECT_EQ(outgoing_goal_response.stamp.nanosec, incoming_goal_response.stamp.nanosec);

  test_msgs__action__Fibonacci_SendGoal_Request__fini(&outgoing_goal_request);
  test_msgs__action__Fibonacci_SendGoal_Request__fini(&incoming_goal_request);
  test_msgs__action__Fibonacci_SendGoal_Response__fini(&incoming_goal_response);
  test_msgs__action__Fibonacci_SendGoal_Response__fini(&outgoing_goal_response);
}


TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_valid_cancel_comm)
{
  action_msgs__srv__CancelGoal_Request outgoing_cancel_request;
  action_msgs__srv__CancelGoal_Request incoming_cancel_request;
  action_msgs__srv__CancelGoal_Response outgoing_cancel_response;
  action_msgs__srv__CancelGoal_Response incoming_cancel_response;
  action_msgs__srv__CancelGoal_Request__init(&outgoing_cancel_request);
  action_msgs__srv__CancelGoal_Request__init(&incoming_cancel_request);
  action_msgs__srv__CancelGoal_Response__init(&outgoing_cancel_response);
  action_msgs__srv__CancelGoal_Response__init(&incoming_cancel_response);

  // Initialize cancel request
  init_test_uuid0(outgoing_cancel_request.goal_info.goal_id.uuid);
  outgoing_cancel_request.goal_info.stamp.sec = 321;
  outgoing_cancel_request.goal_info.stamp.nanosec = 987654u;

  // Send cancel request with valid arguments
  int64_t sequence_number = 1324;
  rcl_ret_t ret = rcl_action_send_cancel_request(
    &this->action_client, &outgoing_cancel_request, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_server(&this->wait_set, &this->action_server, NULL);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_server_wait_set_get_entities_ready(
    &this->wait_set,
    &this->action_server,
    &this->is_goal_request_ready,
    &this->is_cancel_request_ready,
    &this->is_result_request_ready,
    &this->is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_TRUE(this->is_cancel_request_ready);
  EXPECT_FALSE(this->is_goal_request_ready);
  EXPECT_FALSE(this->is_result_request_ready);

  // Take cancel request with valid arguments
  rmw_request_id_t request_header;
  ret = rcl_action_take_cancel_request(
    &this->action_server, &request_header, &incoming_cancel_request);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Check that the cancel request was received correctly
  EXPECT_TRUE(
    uuidcmp(
      outgoing_cancel_request.goal_info.goal_id.uuid,
      incoming_cancel_request.goal_info.goal_id.uuid));
  EXPECT_EQ(
    outgoing_cancel_request.goal_info.stamp.sec,
    incoming_cancel_request.goal_info.stamp.sec);
  EXPECT_EQ(
    outgoing_cancel_request.goal_info.stamp.nanosec,
    incoming_cancel_request.goal_info.stamp.nanosec);

  // Initialize cancel request
  ASSERT_TRUE(
    action_msgs__msg__GoalInfo__Sequence__init(
      &outgoing_cancel_response.goals_canceling, 2));
  init_test_uuid0(outgoing_cancel_response.goals_canceling.data[0].goal_id.uuid);
  outgoing_cancel_response.goals_canceling.data[0].stamp.sec = 102;
  outgoing_cancel_response.goals_canceling.data[0].stamp.nanosec = 9468u;
  init_test_uuid1(outgoing_cancel_response.goals_canceling.data[1].goal_id.uuid);
  outgoing_cancel_response.goals_canceling.data[1].stamp.sec = 867;
  outgoing_cancel_response.goals_canceling.data[1].stamp.nanosec = 6845u;

  // Send cancel response with valid arguments
  // rmw_request_id_t response_header;
  ret = rcl_action_send_cancel_response(
    &this->action_server, &request_header, &outgoing_cancel_response);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait_set_clear(&this->wait_set);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_client(
    &this->wait_set, &this->action_client, NULL, NULL);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_client_wait_set_get_entities_ready(
    &this->wait_set,
    &this->action_client,
    &this->is_feedback_ready,
    &this->is_status_ready,
    &this->is_goal_response_ready,
    &this->is_cancel_response_ready,
    &this->is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_TRUE(this->is_cancel_response_ready);
  EXPECT_FALSE(this->is_feedback_ready);
  EXPECT_FALSE(this->is_status_ready);
  EXPECT_FALSE(this->is_goal_response_ready);
  EXPECT_FALSE(this->is_result_response_ready);

  // Take cancel response with valid arguments
  ret = rcl_action_take_cancel_response(
    &this->action_client, &request_header, &incoming_cancel_response);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Check that the cancel response was received correctly
  ASSERT_EQ(
    outgoing_cancel_response.goals_canceling.size,
    incoming_cancel_response.goals_canceling.size);
  for (size_t i = 0; i < outgoing_cancel_response.goals_canceling.size; ++i) {
    const action_msgs__msg__GoalInfo * outgoing_goal_info =
      &outgoing_cancel_response.goals_canceling.data[i];
    const action_msgs__msg__GoalInfo * incoming_goal_info =
      &incoming_cancel_response.goals_canceling.data[i];
    EXPECT_TRUE(uuidcmp(outgoing_goal_info->goal_id.uuid, incoming_goal_info->goal_id.uuid));
    EXPECT_EQ(outgoing_goal_info->stamp.sec, incoming_goal_info->stamp.sec);
    EXPECT_EQ(outgoing_goal_info->stamp.nanosec, incoming_goal_info->stamp.nanosec);
  }

  action_msgs__srv__CancelGoal_Request__fini(&incoming_cancel_request);
  action_msgs__srv__CancelGoal_Request__fini(&outgoing_cancel_request);
  action_msgs__srv__CancelGoal_Response__fini(&incoming_cancel_response);
  action_msgs__srv__CancelGoal_Response__fini(&outgoing_cancel_response);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_valid_result_comm)
{
  test_msgs__action__Fibonacci_GetResult_Request outgoing_result_request;
  test_msgs__action__Fibonacci_GetResult_Request incoming_result_request;
  test_msgs__action__Fibonacci_GetResult_Response outgoing_result_response;
  test_msgs__action__Fibonacci_GetResult_Response incoming_result_response;
  test_msgs__action__Fibonacci_GetResult_Request__init(&outgoing_result_request);
  test_msgs__action__Fibonacci_GetResult_Request__init(&incoming_result_request);
  test_msgs__action__Fibonacci_GetResult_Response__init(&outgoing_result_response);
  test_msgs__action__Fibonacci_GetResult_Response__init(&incoming_result_response);

  // Initialize result request
  init_test_uuid0(outgoing_result_request.goal_id.uuid);

  // Send result request with valid arguments
  int64_t sequence_number;
  rcl_ret_t ret = rcl_action_send_result_request(
    &this->action_client, &outgoing_result_request, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_server(&this->wait_set, &this->action_server, NULL);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_server_wait_set_get_entities_ready(
    &this->wait_set,
    &this->action_server,
    &this->is_goal_request_ready,
    &this->is_cancel_request_ready,
    &this->is_result_request_ready,
    &this->is_goal_expired);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_TRUE(this->is_result_request_ready);
  EXPECT_FALSE(this->is_cancel_request_ready);
  EXPECT_FALSE(this->is_goal_request_ready);

  // Take result request with valid arguments
  rmw_request_id_t request_header;
  ret = rcl_action_take_result_request(
    &this->action_server, &request_header, &incoming_result_request);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Check that the result request was received correctly
  EXPECT_TRUE(
    uuidcmp(
      outgoing_result_request.goal_id.uuid,
      incoming_result_request.goal_id.uuid));

  // Initialize result response
  ASSERT_TRUE(
    rosidl_runtime_c__int32__Sequence__init(
      &outgoing_result_response.result.sequence, 4));
  outgoing_result_response.result.sequence.data[0] = 0;
  outgoing_result_response.result.sequence.data[1] = 1;
  outgoing_result_response.result.sequence.data[2] = 2;
  outgoing_result_response.result.sequence.data[3] = 6;
  outgoing_result_response.status =
    action_msgs__msg__GoalStatus__STATUS_SUCCEEDED;

  // Send result response with valid arguments
  ret = rcl_action_send_result_response(
    &this->action_server, &request_header, &outgoing_result_response);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait_set_clear(&this->wait_set);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_client(
    &this->wait_set, &this->action_client, NULL, NULL);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_client_wait_set_get_entities_ready(
    &this->wait_set,
    &this->action_client,
    &this->is_feedback_ready,
    &this->is_status_ready,
    &this->is_goal_response_ready,
    &this->is_cancel_response_ready,
    &this->is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_TRUE(this->is_result_response_ready);
  EXPECT_FALSE(this->is_cancel_response_ready);
  EXPECT_FALSE(this->is_feedback_ready);
  EXPECT_FALSE(this->is_status_ready);
  EXPECT_FALSE(this->is_goal_response_ready);

  // Take result response with valid arguments
  ret = rcl_action_take_result_response(
    &this->action_client, &request_header, &incoming_result_response);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Check that the result response was received correctly
  EXPECT_EQ(outgoing_result_response.status, incoming_result_response.status);
  ASSERT_EQ(
    outgoing_result_response.result.sequence.size,
    incoming_result_response.result.sequence.size);
  EXPECT_TRUE(
    !memcmp(
      outgoing_result_response.result.sequence.data,
      incoming_result_response.result.sequence.data,
      outgoing_result_response.result.sequence.size));

  test_msgs__action__Fibonacci_GetResult_Request__fini(&incoming_result_request);
  test_msgs__action__Fibonacci_GetResult_Request__fini(&outgoing_result_request);
  test_msgs__action__Fibonacci_GetResult_Response__fini(&incoming_result_response);
  test_msgs__action__Fibonacci_GetResult_Response__fini(&outgoing_result_response);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_valid_status_comm)
{
  action_msgs__msg__GoalStatusArray incoming_status_array;
  action_msgs__msg__GoalStatusArray__init(&incoming_status_array);

  // Using rcl_action_goal_status_array_t in lieu of a message instance works
  // because these tests make use of C type support
  rcl_action_goal_status_array_t status_array =
    rcl_action_get_zero_initialized_goal_status_array();
  rcl_ret_t ret = rcl_action_get_goal_status_array(&this->action_server, &status_array);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Add a goal before publishing the status array
  rcl_action_goal_info_t goal_info = rcl_action_get_zero_initialized_goal_info();
  rcl_action_goal_handle_t * goal_handle;
  goal_handle = rcl_action_accept_new_goal(&this->action_server, &goal_info);
  ASSERT_NE(goal_handle, nullptr) << rcl_get_error_string().str;

  ret = rcl_action_get_goal_status_array(&this->action_server, &status_array);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Publish status with valid arguments (one goal in array)
  ret = rcl_action_publish_status(&this->action_server, &status_array.msg);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait_set_clear(&this->wait_set);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_client(
    &this->wait_set, &this->action_client, NULL, NULL);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_client_wait_set_get_entities_ready(
    &this->wait_set,
    &this->action_client,
    &this->is_feedback_ready,
    &this->is_status_ready,
    &this->is_goal_response_ready,
    &this->is_cancel_response_ready,
    &this->is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_TRUE(this->is_status_ready);
  EXPECT_FALSE(this->is_result_response_ready);
  EXPECT_FALSE(this->is_cancel_response_ready);
  EXPECT_FALSE(this->is_feedback_ready);
  EXPECT_FALSE(this->is_goal_response_ready);

  // Take status with valid arguments (one goal in array)
  ret = rcl_action_take_status(&this->action_client, &incoming_status_array);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Check that status was received correctly
  ASSERT_EQ(status_array.msg.status_list.size, incoming_status_array.status_list.size);
  for (size_t i = 0; i < status_array.msg.status_list.size; ++i) {
    const action_msgs__msg__GoalStatus * outgoing_status =
      &status_array.msg.status_list.data[i];
    const action_msgs__msg__GoalStatus * incoming_status =
      &incoming_status_array.status_list.data[i];
    EXPECT_TRUE(
      uuidcmp(outgoing_status->goal_info.goal_id.uuid, incoming_status->goal_info.goal_id.uuid));
    EXPECT_EQ(outgoing_status->goal_info.stamp.sec, incoming_status->goal_info.stamp.sec);
    EXPECT_EQ(outgoing_status->goal_info.stamp.nanosec, incoming_status->goal_info.stamp.nanosec);
    EXPECT_EQ(outgoing_status->status, incoming_status->status);
  }

  ret = rcl_action_goal_status_array_fini(&status_array);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  action_msgs__msg__GoalStatusArray__fini(&incoming_status_array);
  EXPECT_EQ(RCL_RET_OK, rcl_action_goal_handle_fini(goal_handle));
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_valid_feedback_comm)
{
  test_msgs__action__Fibonacci_FeedbackMessage outgoing_feedback;
  test_msgs__action__Fibonacci_FeedbackMessage incoming_feedback;
  test_msgs__action__Fibonacci_FeedbackMessage__init(&outgoing_feedback);
  test_msgs__action__Fibonacci_FeedbackMessage__init(&incoming_feedback);

  // Initialize feedback
  ASSERT_TRUE(
    rosidl_runtime_c__int32__Sequence__init(
      &outgoing_feedback.feedback.sequence, 3));
  outgoing_feedback.feedback.sequence.data[0] = 0;
  outgoing_feedback.feedback.sequence.data[1] = 1;
  outgoing_feedback.feedback.sequence.data[2] = 2;
  init_test_uuid0(outgoing_feedback.goal_id.uuid);

  // Publish feedback with valid arguments
  rcl_ret_t ret = rcl_action_publish_feedback(&this->action_server, &outgoing_feedback);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_wait_set_add_action_client(
    &this->wait_set, &this->action_client, NULL, NULL);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  ret = rcl_action_client_wait_set_get_entities_ready(
    &this->wait_set,
    &this->action_client,
    &this->is_feedback_ready,
    &this->is_status_ready,
    &this->is_goal_response_ready,
    &this->is_cancel_response_ready,
    &this->is_result_response_ready);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_TRUE(this->is_feedback_ready);
  EXPECT_FALSE(this->is_status_ready);
  EXPECT_FALSE(this->is_result_response_ready);
  EXPECT_FALSE(this->is_cancel_response_ready);
  EXPECT_FALSE(this->is_goal_response_ready);

  // Take feedback with valid arguments
  ret = rcl_action_take_feedback(&this->action_client, &incoming_feedback);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Check that feedback was received correctly
  EXPECT_TRUE(
    uuidcmp(
      outgoing_feedback.goal_id.uuid,
      incoming_feedback.goal_id.uuid));
  ASSERT_EQ(outgoing_feedback.feedback.sequence.size, incoming_feedback.feedback.sequence.size);
  EXPECT_TRUE(
    !memcmp(
      outgoing_feedback.feedback.sequence.data,
      incoming_feedback.feedback.sequence.data,
      outgoing_feedback.feedback.sequence.size));

  test_msgs__action__Fibonacci_FeedbackMessage__fini(&incoming_feedback);
  test_msgs__action__Fibonacci_FeedbackMessage__fini(&outgoing_feedback);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_invalid_goal_request_opts)
{
  test_msgs__action__Fibonacci_SendGoal_Request outgoing_goal_request;
  test_msgs__action__Fibonacci_SendGoal_Request incoming_goal_request;
  test_msgs__action__Fibonacci_SendGoal_Request__init(&outgoing_goal_request);
  test_msgs__action__Fibonacci_SendGoal_Request__init(&incoming_goal_request);

  // Initialize goal request
  init_test_uuid0(outgoing_goal_request.goal_id.uuid);
  outgoing_goal_request.goal.order = 10;
  int64_t sequence_number = 1234;
  rcl_ret_t ret = 0;

  // Send goal request with null action client
  ret = rcl_action_send_goal_request(nullptr, &outgoing_goal_request, &sequence_number);
  ASSERT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID);
  rcl_reset_error();

  // Send goal request with invalid action client
  rcl_action_client_t invalid_action_client = rcl_action_get_zero_initialized_client();
  ret = rcl_action_send_goal_request(
    &invalid_action_client, &outgoing_goal_request, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID);
  rcl_reset_error();

  // Send goal request with null message
  ret = rcl_action_send_goal_request(&this->action_client, nullptr, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take goal request with null action server
  rmw_request_id_t request_header;
  ret = rcl_action_take_goal_request(nullptr, &request_header, &incoming_goal_request);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take goal request with null header
  ret = rcl_action_take_goal_request(&this->action_server, nullptr, &incoming_goal_request);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  // Take goal request with null message
  ret = rcl_action_take_goal_request(&this->action_server, &request_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take goal request with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_take_goal_request(
    &invalid_action_server, &request_header, &incoming_goal_request);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  test_msgs__action__Fibonacci_SendGoal_Request__fini(&outgoing_goal_request);
  test_msgs__action__Fibonacci_SendGoal_Request__fini(&incoming_goal_request);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_invalid_goal_response_opts)
{
  test_msgs__action__Fibonacci_SendGoal_Response outgoing_goal_response;
  test_msgs__action__Fibonacci_SendGoal_Response incoming_goal_response;
  test_msgs__action__Fibonacci_SendGoal_Response__init(&outgoing_goal_response);
  test_msgs__action__Fibonacci_SendGoal_Response__init(&incoming_goal_response);

  // Initialize goal response
  outgoing_goal_response.accepted = true;
  outgoing_goal_response.stamp.sec = 123;
  outgoing_goal_response.stamp.nanosec = 456789u;

  // Send goal response with null action server
  rmw_request_id_t response_header;
  rcl_ret_t ret = rcl_action_send_goal_response(nullptr, &response_header, &outgoing_goal_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send goal response with null header
  ret = rcl_action_send_goal_response(&this->action_server, nullptr, &outgoing_goal_response);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  // Send goal response with null message
  ret = rcl_action_send_goal_response(&this->action_server, &response_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send goal response with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_send_goal_response(
    &invalid_action_server, &response_header, &outgoing_goal_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take goal response with null action client
  ret = rcl_action_take_goal_response(nullptr, &response_header, &incoming_goal_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID);
  rcl_reset_error();

  // Take goal response with null header
  ret = rcl_action_take_goal_response(&this->action_client, nullptr, &incoming_goal_response);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  // Take goal response with null message
  ret = rcl_action_take_goal_response(&this->action_client, &response_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  // Take goal response with invalid action client
  rcl_action_client_t invalid_action_client = rcl_action_get_zero_initialized_client();
  ret = rcl_action_take_goal_response(
    &invalid_action_client, &response_header, &incoming_goal_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  test_msgs__action__Fibonacci_SendGoal_Response__fini(&incoming_goal_response);
  test_msgs__action__Fibonacci_SendGoal_Response__fini(&outgoing_goal_response);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_invalid_cancel_request_opts)
{
  action_msgs__srv__CancelGoal_Request outgoing_cancel_request;
  action_msgs__srv__CancelGoal_Request incoming_cancel_request;
  action_msgs__srv__CancelGoal_Request__init(&outgoing_cancel_request);
  action_msgs__srv__CancelGoal_Request__init(&incoming_cancel_request);

  // Initialize cancel request
  init_test_uuid0(outgoing_cancel_request.goal_info.goal_id.uuid);
  outgoing_cancel_request.goal_info.stamp.sec = 321;
  outgoing_cancel_request.goal_info.stamp.nanosec = 987654u;

  // Send cancel request with null action client
  int64_t sequence_number = 1324;
  rcl_ret_t ret = rcl_action_send_cancel_request(
    nullptr, &outgoing_cancel_request, &sequence_number);
  ASSERT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send cancel request with invalid action client
  rcl_action_client_t invalid_action_client = rcl_action_get_zero_initialized_client();
  ret = rcl_action_send_cancel_request(
    &invalid_action_client, &outgoing_cancel_request, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send cancel request with null message
  ret = rcl_action_send_cancel_request(&this->action_client, nullptr, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take cancel request with null action server
  rmw_request_id_t request_header;
  ret = rcl_action_take_cancel_request(nullptr, &request_header, &incoming_cancel_request);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take cancel request with null header
  ret = rcl_action_take_cancel_request(&this->action_server, nullptr, &incoming_cancel_request);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  // Take cancel request with null message
  ret = rcl_action_take_cancel_request(&this->action_server, &request_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take cancel request with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_take_cancel_request(
    &invalid_action_server, &request_header, &incoming_cancel_request);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  action_msgs__srv__CancelGoal_Request__fini(&incoming_cancel_request);
  action_msgs__srv__CancelGoal_Request__fini(&outgoing_cancel_request);
}


TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_invalid_cancel_response_opts)
{
  action_msgs__srv__CancelGoal_Response outgoing_cancel_response;
  action_msgs__srv__CancelGoal_Response incoming_cancel_response;
  action_msgs__srv__CancelGoal_Response__init(&outgoing_cancel_response);
  action_msgs__srv__CancelGoal_Response__init(&incoming_cancel_response);

  // Initialize cancel request
  ASSERT_TRUE(
    action_msgs__msg__GoalInfo__Sequence__init(
      &outgoing_cancel_response.goals_canceling, 2));
  init_test_uuid0(outgoing_cancel_response.goals_canceling.data[0].goal_id.uuid);
  outgoing_cancel_response.goals_canceling.data[0].stamp.sec = 102;
  outgoing_cancel_response.goals_canceling.data[0].stamp.nanosec = 9468u;
  init_test_uuid1(outgoing_cancel_response.goals_canceling.data[1].goal_id.uuid);
  outgoing_cancel_response.goals_canceling.data[1].stamp.sec = 867;
  outgoing_cancel_response.goals_canceling.data[1].stamp.nanosec = 6845u;

  // Send cancel response with null action server
  rmw_request_id_t response_header;
  rcl_ret_t ret = rcl_action_send_cancel_response(
    nullptr, &response_header, &outgoing_cancel_response);
  ASSERT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send cancel response with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_send_cancel_response(
    &invalid_action_server, &response_header, &outgoing_cancel_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send cancel response with null header
  ret = rcl_action_send_cancel_response(&this->action_server, nullptr, &outgoing_cancel_response);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send cancel response with null message
  ret = rcl_action_send_cancel_response(&this->action_server, &response_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take cancel response with null action client
  ret = rcl_action_take_cancel_response(nullptr, &response_header, &incoming_cancel_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take cancel response with invalid action client
  rcl_action_client_t invalid_action_client = rcl_action_get_zero_initialized_client();
  ret = rcl_action_take_cancel_response(
    &invalid_action_client, &response_header, &incoming_cancel_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take cancel response with null message
  ret = rcl_action_take_cancel_response(&this->action_client, &response_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  action_msgs__srv__CancelGoal_Response__fini(&incoming_cancel_response);
  action_msgs__srv__CancelGoal_Response__fini(&outgoing_cancel_response);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_invalid_result_request_opts)
{
  test_msgs__action__Fibonacci_GetResult_Request outgoing_result_request;
  test_msgs__action__Fibonacci_GetResult_Request incoming_result_request;
  test_msgs__action__Fibonacci_GetResult_Request__init(&outgoing_result_request);
  test_msgs__action__Fibonacci_GetResult_Request__init(&incoming_result_request);

  // Initialize result request
  init_test_uuid0(outgoing_result_request.goal_id.uuid);

  // Send result request with null action client
  int64_t sequence_number = 1324;
  rcl_ret_t ret = rcl_action_send_result_request(
    nullptr, &outgoing_result_request, &sequence_number);
  ASSERT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send result request with invalid action client
  rcl_action_client_t invalid_action_client = rcl_action_get_zero_initialized_client();
  ret = rcl_action_send_result_request(
    &invalid_action_client, &outgoing_result_request, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send result request with null message
  ret = rcl_action_send_result_request(&this->action_client, nullptr, &sequence_number);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take result request with null action server
  rmw_request_id_t request_header;
  ret = rcl_action_take_result_request(nullptr, &request_header, &incoming_result_request);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take result request with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_take_result_request(
    &invalid_action_server, &request_header, &incoming_result_request);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take result request with null header
  ret = rcl_action_take_result_request(&this->action_server, nullptr, &incoming_result_request);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take result request with null message
  ret = rcl_action_take_result_request(&this->action_server, &request_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  test_msgs__action__Fibonacci_GetResult_Request__fini(&incoming_result_request);
  test_msgs__action__Fibonacci_GetResult_Request__fini(&outgoing_result_request);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_invalid_result_response_opts)
{
  test_msgs__action__Fibonacci_GetResult_Response outgoing_result_response;
  test_msgs__action__Fibonacci_GetResult_Response incoming_result_response;
  test_msgs__action__Fibonacci_GetResult_Response__init(&outgoing_result_response);
  test_msgs__action__Fibonacci_GetResult_Response__init(&incoming_result_response);

  // Initialize result response
  ASSERT_TRUE(
    rosidl_runtime_c__int32__Sequence__init(
      &outgoing_result_response.result.sequence, 4));
  outgoing_result_response.result.sequence.data[0] = 0;
  outgoing_result_response.result.sequence.data[1] = 1;
  outgoing_result_response.result.sequence.data[2] = 2;
  outgoing_result_response.result.sequence.data[3] = 6;
  outgoing_result_response.status =
    action_msgs__msg__GoalStatus__STATUS_SUCCEEDED;

  // Send result response with null action client
  rmw_request_id_t response_header;
  rcl_ret_t ret = rcl_action_send_result_response(
    nullptr, &response_header, &outgoing_result_response);
  ASSERT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send result response with invalid action client
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_send_result_response(
    &invalid_action_server, &response_header, &outgoing_result_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send result response with null header
  ret = rcl_action_send_result_response(&this->action_server, nullptr, &outgoing_result_response);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Send result response with null message
  ret = rcl_action_send_result_response(&this->action_server, &response_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take result response with null action client
  ret = rcl_action_take_result_response(nullptr, &response_header, &incoming_result_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take result response with null message
  ret = rcl_action_take_result_response(&this->action_client, &response_header, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take result response with invalid action client
  rcl_action_client_t invalid_action_client = rcl_action_get_zero_initialized_client();
  ret = rcl_action_take_result_response(
    &invalid_action_client, &response_header, &incoming_result_response);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  test_msgs__action__Fibonacci_GetResult_Response__fini(&incoming_result_response);
  test_msgs__action__Fibonacci_GetResult_Response__fini(&outgoing_result_response);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_invalid_feedback_opts)
{
  test_msgs__action__Fibonacci_FeedbackMessage outgoing_feedback;
  test_msgs__action__Fibonacci_FeedbackMessage incoming_feedback;
  test_msgs__action__Fibonacci_FeedbackMessage__init(&outgoing_feedback);
  test_msgs__action__Fibonacci_FeedbackMessage__init(&incoming_feedback);

  // Initialize feedback
  ASSERT_TRUE(
    rosidl_runtime_c__int32__Sequence__init(
      &outgoing_feedback.feedback.sequence, 3));
  outgoing_feedback.feedback.sequence.data[0] = 0;
  outgoing_feedback.feedback.sequence.data[1] = 1;
  outgoing_feedback.feedback.sequence.data[2] = 2;
  init_test_uuid0(outgoing_feedback.goal_id.uuid);

  // Publish feedback with null action server
  rcl_ret_t ret = rcl_action_publish_feedback(nullptr, &outgoing_feedback);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Publish feedback with null message
  ret = rcl_action_publish_feedback(&this->action_server, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Publish feedback with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_publish_feedback(&invalid_action_server, &outgoing_feedback);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take feedback with null action client
  ret = rcl_action_take_feedback(nullptr, &incoming_feedback);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take feedback with invalid action client
  rcl_action_client_t invalid_action_client = rcl_action_get_zero_initialized_client();
  ret = rcl_action_take_feedback(&invalid_action_client, &incoming_feedback);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take feedback with null message
  ret = rcl_action_take_feedback(&this->action_client, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  test_msgs__action__Fibonacci_FeedbackMessage__fini(&incoming_feedback);
  test_msgs__action__Fibonacci_FeedbackMessage__fini(&outgoing_feedback);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_invalid_status_opts)
{
  action_msgs__msg__GoalStatusArray incoming_status_array;
  action_msgs__msg__GoalStatusArray__init(&incoming_status_array);

  // Using rcl_action_goal_status_array_t in lieu of a message instance works
  // because these tests make use of C type support
  rcl_action_goal_status_array_t status_array =
    rcl_action_get_zero_initialized_goal_status_array();
  rcl_ret_t ret = rcl_action_get_goal_status_array(&this->action_server, &status_array);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  // Publish status with null action server
  ret = rcl_action_publish_status(nullptr, &status_array.msg);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Publish status with null message
  ret = rcl_action_publish_status(&this->action_server, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  // Publish status with invalid action server
  rcl_action_server_t invalid_action_server = rcl_action_get_zero_initialized_server();
  ret = rcl_action_publish_status(&invalid_action_server, &status_array.msg);
  EXPECT_EQ(ret, RCL_RET_ACTION_SERVER_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take status with null action client
  ret = rcl_action_take_status(nullptr, &incoming_status_array);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take status with invalid action client
  rcl_action_client_t invalid_action_client = rcl_action_get_zero_initialized_client();
  ret = rcl_action_take_status(&invalid_action_client, &incoming_status_array);
  EXPECT_EQ(ret, RCL_RET_ACTION_CLIENT_INVALID) << rcl_get_error_string().str;
  rcl_reset_error();

  // Take status with null message
  ret = rcl_action_take_status(&this->action_client, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();

  ret = rcl_action_goal_status_array_fini(&status_array);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  action_msgs__msg__GoalStatusArray__fini(&incoming_status_array);
}

TEST_F(CLASSNAME(TestActionCommunication, RMW_IMPLEMENTATION), test_valid_feedback_comm_maybe_fail)
{
  test_msgs__action__Fibonacci_FeedbackMessage outgoing_feedback;
  test_msgs__action__Fibonacci_FeedbackMessage incoming_feedback;
  test_msgs__action__Fibonacci_FeedbackMessage__init(&outgoing_feedback);
  test_msgs__action__Fibonacci_FeedbackMessage__init(&incoming_feedback);

  // Initialize feedback
  ASSERT_TRUE(
    rosidl_runtime_c__int32__Sequence__init(
      &outgoing_feedback.feedback.sequence, 3));
  outgoing_feedback.feedback.sequence.data[0] = 0;
  outgoing_feedback.feedback.sequence.data[1] = 1;
  outgoing_feedback.feedback.sequence.data[2] = 2;
  init_test_uuid0(outgoing_feedback.goal_id.uuid);

  RCUTILS_FAULT_INJECTION_TEST(
  {
    // Publish feedback with valid arguments
    rcl_ret_t ret = rcl_action_publish_feedback(&this->action_server, &outgoing_feedback);
    if (RCL_RET_OK != ret) {
      // This isn't set in all cases, but reset anyway.
      rcl_reset_error();
      continue;
    }

    ret = rcl_action_wait_set_add_action_client(
      &this->wait_set, &this->action_client, NULL, NULL);
    if (RCL_RET_OK != ret) {
      EXPECT_TRUE(rcl_error_is_set());
      rcl_reset_error();
      continue;
    }

    ret = rcl_wait(&this->wait_set, RCL_S_TO_NS(10));
    if (RCL_RET_OK != ret) {
      // All non-OK retcodes should set an error string, except RCL_RET_TIMEOUT.
      if (RCL_RET_TIMEOUT != ret) {
        EXPECT_TRUE(rcl_error_is_set());
        rcl_reset_error();
      }
      continue;
    }

    ret = rcl_action_client_wait_set_get_entities_ready(
      &this->wait_set,
      &this->action_client,
      &this->is_feedback_ready,
      &this->is_status_ready,
      &this->is_goal_response_ready,
      &this->is_cancel_response_ready,
      &this->is_result_response_ready);
    if (RCL_RET_OK != ret) {
      EXPECT_TRUE(rcl_error_is_set());
      rcl_reset_error();
      continue;
    }

    // Take feedback with valid arguments
    ret = rcl_action_take_feedback(&this->action_client, &incoming_feedback);
    if (RCL_RET_OK != ret) {
      EXPECT_TRUE(rcl_error_is_set());
      rcl_reset_error();
      continue;
    }

    test_msgs__action__Fibonacci_FeedbackMessage__fini(&incoming_feedback);
    test_msgs__action__Fibonacci_FeedbackMessage__fini(&outgoing_feedback);
  });
}
