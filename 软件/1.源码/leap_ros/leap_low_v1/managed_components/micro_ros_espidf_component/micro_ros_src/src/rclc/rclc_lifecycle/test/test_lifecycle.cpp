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

// testing default transition sequence.
// This test requires that the transitions are set
// as depicted in design.ros2.org

#include <gtest/gtest.h>

extern "C"
{
#include <lifecycle_msgs/msg/state.h>
#include <lifecycle_msgs/msg/transition.h>

#include "rclc_lifecycle/rclc_lifecycle.h"
}

static int callback_mockup_counter = 0;

rcl_ret_t callback_mockup_0()
{
  callback_mockup_counter += 1;
  return RCL_RET_OK;
}

rcl_ret_t callback_mockup_1()
{
  callback_mockup_counter += 2;
  return RCL_RET_OK;
}

rcl_ret_t callback_mockup_2()
{
  callback_mockup_counter += 4;
  return RCL_RET_OK;
}

rcl_ret_t callback_mockup_3()
{
  callback_mockup_counter += 8;
  return RCL_RET_OK;
}

TEST(TestRclcLifecycle, lifecycle_node) {
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_allocator_t allocator = rcl_get_default_allocator();

  rcl_ret_t res = rcl_init_options_init(&init_options, allocator);
  res += rcl_init(0, nullptr, &init_options, &context);

  rcl_node_t my_node = rcl_get_zero_initialized_node();
  rcl_node_options_t node_ops = rcl_node_get_default_options();
  res += rcl_node_init(&my_node, "lifecycle_node", "rclc", &context, &node_ops);

  rclc_lifecycle_node_t lifecycle_node;
  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();

  res += rclc_make_node_a_lifecycle_node(
    &lifecycle_node,
    &my_node,
    &state_machine,
    &allocator,
    true);

  EXPECT_EQ(RCL_RET_OK, res);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_lifecycle_state_machine_is_initialized(lifecycle_node.state_machine));

  // clean up
  res = rcl_node_fini(&my_node);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rcl_node_options_fini(&node_ops);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rcl_init_options_fini(&init_options);
  EXPECT_EQ(RCL_RET_OK, res);
}

TEST(TestRclcLifecycle, lifecycle_node_transitions) {
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_allocator_t allocator = rcl_get_default_allocator();

  rcl_ret_t res = rcl_init_options_init(&init_options, allocator);
  res += rcl_init(0, nullptr, &init_options, &context);

  rcl_node_t my_node = rcl_get_zero_initialized_node();
  rcl_node_options_t node_ops = rcl_node_get_default_options();
  res += rcl_node_init(&my_node, "lifecycle_node", "rclc", &context, &node_ops);

  rclc_lifecycle_node_t lifecycle_node;
  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();

  res += rclc_make_node_a_lifecycle_node(
    &lifecycle_node,
    &my_node,
    &state_machine,
    &allocator,
    false);

  // configure
  res = rclc_lifecycle_change_state(
    &lifecycle_node,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    true);
  EXPECT_EQ(RCL_RET_OK, res);
  EXPECT_EQ(
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_node.state_machine->current_state->id);

  // activate
  res = rclc_lifecycle_change_state(
    &lifecycle_node,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    true);
  EXPECT_EQ(RCL_RET_OK, res);
  EXPECT_EQ(
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE,
    lifecycle_node.state_machine->current_state->id);

  // deactivate
  res = rclc_lifecycle_change_state(
    &lifecycle_node,
    lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
    true);
  EXPECT_EQ(RCL_RET_OK, res);
  EXPECT_EQ(
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_node.state_machine->current_state->id);

  // cleanup
  res = rclc_lifecycle_change_state(
    &lifecycle_node,
    lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
    true);
  EXPECT_EQ(RCL_RET_OK, res);
  EXPECT_EQ(
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_node.state_machine->current_state->id);

  res = rcl_node_fini(&my_node);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rcl_node_options_fini(&node_ops);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rcl_init_options_fini(&init_options);
  EXPECT_EQ(RCL_RET_OK, res);
}

TEST(TestRclcLifecycle, lifecycle_node_callbacks) {
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_allocator_t allocator = rcl_get_default_allocator();

  rcl_ret_t res = rcl_init_options_init(&init_options, allocator);
  res += rcl_init(0, nullptr, &init_options, &context);

  rcl_node_t my_node = rcl_get_zero_initialized_node();
  rcl_node_options_t node_ops = rcl_node_get_default_options();
  res += rcl_node_init(&my_node, "lifecycle_node", "rclc", &context, &node_ops);

  rclc_lifecycle_node_t lifecycle_node;
  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();

  res += rclc_make_node_a_lifecycle_node(
    &lifecycle_node,
    &my_node,
    &state_machine,
    &allocator,
    true);

  // register callbacks
  rclc_lifecycle_register_on_configure(&lifecycle_node, &callback_mockup_0);
  rclc_lifecycle_register_on_activate(&lifecycle_node, &callback_mockup_1);
  rclc_lifecycle_register_on_deactivate(&lifecycle_node, &callback_mockup_2);
  rclc_lifecycle_register_on_cleanup(&lifecycle_node, &callback_mockup_3);

  // run through the lifecycle
  res += rclc_lifecycle_change_state(
    &lifecycle_node,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    true);
  EXPECT_EQ(RCL_RET_OK, res);
  EXPECT_EQ(1, callback_mockup_counter);

  res += rclc_lifecycle_change_state(
    &lifecycle_node,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    true);
  EXPECT_EQ(RCL_RET_OK, res);
  EXPECT_EQ(3, callback_mockup_counter);

  res += rclc_lifecycle_change_state(
    &lifecycle_node,
    lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
    true);
  EXPECT_EQ(RCL_RET_OK, res);
  EXPECT_EQ(7, callback_mockup_counter);

  res += rclc_lifecycle_change_state(
    &lifecycle_node,
    lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
    true);
  EXPECT_EQ(RCL_RET_OK, res);
  EXPECT_EQ(15, callback_mockup_counter);

  res = rcl_node_fini(&my_node);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rcl_node_options_fini(&node_ops);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rcl_init_options_fini(&init_options);
  EXPECT_EQ(RCL_RET_OK, res);
}

TEST(TestRclcLifecycle, lifecycle_node_servers) {
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_allocator_t allocator = rcl_get_default_allocator();

  rcl_ret_t res = rcl_init_options_init(&init_options, allocator);
  res += rcl_init(0, nullptr, &init_options, &context);

  rcl_node_t my_node = rcl_get_zero_initialized_node();
  rcl_node_options_t node_ops = rcl_node_get_default_options();
  res += rcl_node_init(&my_node, "lifecycle_node", "rclc", &context, &node_ops);

  rclc_lifecycle_node_t lifecycle_node;
  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();

  res += rclc_make_node_a_lifecycle_node(
    &lifecycle_node,
    &my_node,
    &state_machine,
    &allocator,
    true);

  // register callbacks
  rclc_lifecycle_register_on_configure(&lifecycle_node, &callback_mockup_0);
  rclc_lifecycle_register_on_activate(&lifecycle_node, &callback_mockup_1);
  rclc_lifecycle_register_on_deactivate(&lifecycle_node, &callback_mockup_2);
  rclc_lifecycle_register_on_cleanup(&lifecycle_node, &callback_mockup_3);

  rclc_lifecycle_service_context_t lcontext;
  lcontext.lifecycle_node = &lifecycle_node;

  // create lifecycle servers
  rclc_executor_t executor;
  res = rclc_executor_init(
    &executor,
    &context,
    1,  // too little
    &allocator);
  EXPECT_EQ(RCL_RET_OK, res);

  // Too little executor handles
  res = rclc_lifecycle_init_get_state_server(&lcontext, &executor);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rclc_lifecycle_init_get_available_states_server(&lcontext, &executor);
  EXPECT_EQ(RCL_RET_ERROR, res);

  // Now with correct number of handles
  rclc_executor_init(
    &executor,
    &context,
    3,  // 1 for each lifecycle service
    &allocator);
  res = rclc_lifecycle_init_get_state_server(&lcontext, &executor);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rclc_lifecycle_init_get_available_states_server(&lcontext, &executor);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rclc_lifecycle_init_change_state_server(&lcontext, &executor);
  EXPECT_EQ(RCL_RET_OK, res);

  // Cleanup
  res = rclc_lifecycle_node_fini(&lifecycle_node, &allocator);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rcl_node_fini(&my_node);
  EXPECT_EQ(RCL_RET_OK, res);
  res = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, res);
}
