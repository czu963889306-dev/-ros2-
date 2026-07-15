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

#include "rcl_lifecycle/rcl_lifecycle.h"

#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcutils/testing/fault_injection.h"

#include "lifecycle_msgs/msg/transition_event.h"
#include "lifecycle_msgs/srv/change_state.h"
#include "lifecycle_msgs/srv/get_available_states.h"
#include "lifecycle_msgs/srv/get_available_transitions.h"
#include "lifecycle_msgs/srv/get_state.h"

static void * bad_malloc(size_t, void *)
{
  return nullptr;
}

static void * bad_realloc(void *, size_t, void *)
{
  return nullptr;
}

TEST(TestRclLifecycle, lifecycle_state) {
  rcl_lifecycle_state_t state = rcl_lifecycle_get_zero_initialized_state();
  EXPECT_EQ(0u, state.id);
  EXPECT_EQ(nullptr, state.label);

  rcl_allocator_t allocator = rcl_get_default_allocator();
  uint8_t expected_id = 42;
  const char expected_label[] = "label";
  rcl_ret_t ret = rcl_lifecycle_state_init(&state, expected_id, &expected_label[0], nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_state_init(&state, expected_id, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_state_init(nullptr, expected_id, &expected_label[0], &allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  rcl_allocator_t bad_allocator = rcl_get_default_allocator();
  bad_allocator.allocate = bad_malloc;
  bad_allocator.reallocate = bad_realloc;
  ret = rcl_lifecycle_state_init(&state, expected_id, &expected_label[0], &bad_allocator);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_state_init(&state, expected_id, &expected_label[0], &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(expected_id, state.id);
  EXPECT_STREQ(&expected_label[0], state.label);

  ret = rcl_lifecycle_state_fini(&state, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  // Already finalized
  ret = rcl_lifecycle_state_fini(nullptr, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_lifecycle_state_fini(&state, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST(TestRclLifecycle, lifecycle_transition) {
  rcl_lifecycle_transition_t transition = rcl_lifecycle_get_zero_initialized_transition();
  EXPECT_EQ(0u, transition.id);
  EXPECT_EQ(nullptr, transition.label);
  EXPECT_EQ(nullptr, transition.start);
  EXPECT_EQ(nullptr, transition.goal);

  rcl_allocator_t allocator = rcl_get_default_allocator();

  // These need to be allocated on heap so rcl_lifecycle_transition_fini doesn't free a stack
  // allocated variable
  rcl_lifecycle_state_t * start = reinterpret_cast<rcl_lifecycle_state_t *>(
    allocator.allocate(sizeof(rcl_lifecycle_state_t), allocator.state));
  EXPECT_NE(nullptr, start);
  rcl_lifecycle_state_t * end = reinterpret_cast<rcl_lifecycle_state_t *>(
    allocator.allocate(sizeof(rcl_lifecycle_state_t), allocator.state));
  EXPECT_NE(nullptr, end);
  const char start_label[] = "start";
  const char end_label[] = "end";
  *start = rcl_lifecycle_get_zero_initialized_state();
  *end = rcl_lifecycle_get_zero_initialized_state();

  rcl_ret_t ret = rcl_lifecycle_state_init(start, 0u, &start_label[0], &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_lifecycle_state_init(end, 1u, &end_label[0], &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  unsigned int expected_id = 42;
  const char expected_label[] = "label";

  ret = rcl_lifecycle_transition_init(
    nullptr, expected_id, nullptr, nullptr, nullptr, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_transition_init(
    &transition, expected_id, nullptr, nullptr, nullptr, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_transition_init(
    nullptr, expected_id, nullptr, nullptr, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_transition_init(
    &transition, expected_id, nullptr, nullptr, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_transition_init(
    &transition, expected_id, &expected_label[0], nullptr, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();
  ret = rcl_lifecycle_transition_fini(&transition, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();

  ret = rcl_lifecycle_transition_init(
    &transition, expected_id, &expected_label[0], start, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();
  ret = rcl_lifecycle_transition_fini(&transition, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();

  start = reinterpret_cast<rcl_lifecycle_state_t *>(
    allocator.allocate(sizeof(rcl_lifecycle_state_t), allocator.state));
  *start = rcl_lifecycle_get_zero_initialized_state();
  rcl_allocator_t bad_allocator = rcl_get_default_allocator();
  bad_allocator.allocate = bad_malloc;
  bad_allocator.reallocate = bad_realloc;
  ret = rcl_lifecycle_transition_init(
    &transition, expected_id, &expected_label[0], start, end, &bad_allocator);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_transition_init(
    &transition, expected_id, &expected_label[0], start, end, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(expected_id, transition.id);
  EXPECT_STREQ(&expected_label[0], transition.label);

  ret = rcl_lifecycle_transition_fini(nullptr, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_transition_fini(&transition, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  // Already finalized
  ret = rcl_lifecycle_transition_fini(nullptr, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_lifecycle_transition_fini(&transition, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST(TestRclLifecycle, state_machine) {
  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();
  EXPECT_EQ(nullptr, state_machine.current_state);
  EXPECT_EQ(nullptr, state_machine.transition_map.states);
  EXPECT_EQ(nullptr, state_machine.transition_map.transitions);
  EXPECT_EQ(0u, state_machine.transition_map.states_size);
  EXPECT_EQ(0u, state_machine.transition_map.transitions_size);

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_node_options_t options = rcl_node_get_default_options();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_node_options_fini(&options));
  });
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options));
  });

  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context)) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context)) << rcl_get_error_string().str;
  });

  ret = rcl_node_init(&node, "node", "namespace", &context, &options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node)) << rcl_get_error_string().str;
  });

  const rosidl_message_type_support_t * pn =
    ROSIDL_GET_MSG_TYPE_SUPPORT(lifecycle_msgs, msg, TransitionEvent);
  const rosidl_service_type_support_t * cs =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, ChangeState);
  const rosidl_service_type_support_t * gs =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetState);
  const rosidl_service_type_support_t * gas =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableStates);
  const rosidl_service_type_support_t * gat =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableTransitions);
  const rosidl_service_type_support_t * gtg =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableTransitions);

  rcl_lifecycle_state_machine_options_t state_machine_options =
    rcl_lifecycle_get_default_state_machine_options();
  state_machine_options.initialize_default_states = false;

  // Check various arguments are null
  ret = rcl_lifecycle_state_machine_init(
    nullptr, &node, pn, cs, gs, gas, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, nullptr, pn, cs, gs, gas, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, nullptr, cs, gs, gas, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, pn, nullptr, gs, gas, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, pn, cs, nullptr, gas, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, pn, cs, gs, nullptr, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, pn, cs, gs, gas, nullptr, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, pn, cs, gs, gas, gat, nullptr, &state_machine_options);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  // Com interface not enabled
  // The transition event publisher is active
  // The external transition services are inactive
  state_machine_options = rcl_lifecycle_get_default_state_machine_options();
  state_machine_options.enable_com_interface = false;

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, pn, cs, gs, gas, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_NE(nullptr, &state_machine.com_interface);
  EXPECT_NE(nullptr, &state_machine.com_interface.pub_transition_event.impl);
  EXPECT_EQ(nullptr, state_machine.com_interface.srv_change_state.impl);
  EXPECT_EQ(nullptr, state_machine.com_interface.srv_get_state.impl);
  EXPECT_EQ(nullptr, state_machine.com_interface.srv_get_available_states.impl);
  EXPECT_EQ(nullptr, state_machine.com_interface.srv_get_available_transitions.impl);
  EXPECT_EQ(nullptr, state_machine.com_interface.srv_get_transition_graph.impl);
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_lifecycle_state_machine_is_initialized(&state_machine)) << rcl_get_error_string().str;
  // Reset the state machine as the previous init call was successful
  ret = rcl_lifecycle_state_machine_fini(&state_machine, &node);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Everything should be good
  state_machine_options = rcl_lifecycle_get_default_state_machine_options();
  state_machine_options.initialize_default_states = false;

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, pn, cs, gs, gas, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Transition_map is not initialized
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  state_machine.com_interface.srv_change_state.impl = nullptr;
  // get_state service is valid, but not change_state service
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();

  // Allocate some memory and initialize states and transitions so is_initialized will pass
  state_machine.transition_map.states_size = 1u;
  state_machine.transition_map.states = reinterpret_cast<rcl_lifecycle_state_t *>(
    allocator.allocate(
      state_machine.transition_map.states_size * sizeof(rcl_lifecycle_state_t),
      allocator.state));
  ASSERT_NE(nullptr, state_machine.transition_map.states);
  state_machine.transition_map.states[0] = rcl_lifecycle_get_zero_initialized_state();

  state_machine.transition_map.transitions_size = 1u;
  state_machine.transition_map.transitions =
    reinterpret_cast<rcl_lifecycle_transition_t *>(allocator.allocate(
      state_machine.transition_map.transitions_size * sizeof(rcl_lifecycle_transition_t),
      allocator.state));
  ASSERT_NE(nullptr, state_machine.transition_map.transitions);
  state_machine.transition_map.transitions[0] = rcl_lifecycle_get_zero_initialized_transition();

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));

  // get_state service is valid, but not change_state service
  void * temp_function = state_machine.com_interface.srv_change_state.impl;
  state_machine.com_interface.srv_change_state.impl = nullptr;
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_lifecycle_state_machine_is_initialized(&state_machine));
  rcutils_reset_error();
  state_machine.com_interface.srv_change_state.impl =
    reinterpret_cast<rcl_service_impl_t *>(temp_function);

  ret = rcl_lifecycle_state_machine_fini(&state_machine, &node);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  state_machine = rcl_lifecycle_get_zero_initialized_state_machine();

  // Node is null
  ret = rcl_lifecycle_state_machine_fini(&state_machine, nullptr);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();
}

TEST(TestRclLifecycle, state_transitions) {
  rcl_lifecycle_state_machine_t state_machine =
    rcl_lifecycle_get_zero_initialized_state_machine();
  EXPECT_EQ(nullptr, state_machine.current_state);
  EXPECT_EQ(nullptr, state_machine.transition_map.states);
  EXPECT_EQ(nullptr, state_machine.transition_map.transitions);
  EXPECT_EQ(0u, state_machine.transition_map.states_size);
  EXPECT_EQ(0u, state_machine.transition_map.transitions_size);

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_lifecycle_state_machine_options_t state_machine_options =
    rcl_lifecycle_get_default_state_machine_options();

  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_node_options_t options = rcl_node_get_default_options();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_node_options_fini(&options));
  });

  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options));
  });

  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });

  ret = rcl_node_init(&node, "node", "namespace", &context, &options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node));
  });

  const rosidl_message_type_support_t * pn =
    ROSIDL_GET_MSG_TYPE_SUPPORT(lifecycle_msgs, msg, TransitionEvent);
  const rosidl_service_type_support_t * cs =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, ChangeState);
  const rosidl_service_type_support_t * gs =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetState);
  const rosidl_service_type_support_t * gas =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableStates);
  const rosidl_service_type_support_t * gat =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableTransitions);
  const rosidl_service_type_support_t * gtg =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableTransitions);

  ret = rcl_lifecycle_state_machine_init(
    &state_machine, &node, pn, cs, gs, gas, gat, gtg, &state_machine_options);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_lifecycle_state_machine_is_initialized(&state_machine);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  const rcl_lifecycle_transition_t * transition = rcl_lifecycle_get_transition_by_id(nullptr, 0);
  EXPECT_EQ(nullptr, transition) << rcl_get_error_string().str;
  rcutils_reset_error();

  transition = rcl_lifecycle_get_transition_by_id(
    state_machine.current_state, lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE);
  EXPECT_EQ(lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE, transition->id);

  // Update this test with a new invalid number if 42 ever becomes a valid state id
  transition = rcl_lifecycle_get_transition_by_id(state_machine.current_state, 42);
  EXPECT_EQ(nullptr, transition) << rcl_get_error_string().str;
  rcutils_reset_error();

  transition = rcl_lifecycle_get_transition_by_label(state_machine.current_state, "configure");
  EXPECT_STREQ("configure", transition->label);

  transition = rcl_lifecycle_get_transition_by_label(state_machine.current_state, "NOT A LABEL");
  EXPECT_EQ(nullptr, transition) << rcl_get_error_string().str;
  rcutils_reset_error();

  ret = rcl_lifecycle_trigger_transition_by_id(nullptr, 0, false);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_trigger_transition_by_id(
    &state_machine, lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE, false);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_lifecycle_trigger_transition_by_label(nullptr, "transition_success", true);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcl_lifecycle_trigger_transition_by_label(&state_machine, "transition_success", true);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // If using the public interface to register transitions, this case should already be checked.
  state_machine.current_state->valid_transitions[0].goal = nullptr;
  ret = rcl_lifecycle_trigger_transition_by_label(&state_machine, "transition_success", true);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  rcl_print_state_machine(&state_machine);
  EXPECT_FALSE(rcutils_error_is_set());

  ret = rcl_lifecycle_state_machine_fini(&state_machine, &node);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST(TestRclLifecycle, init_fini_maybe_fail) {
  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_node_options_t options = rcl_node_get_default_options();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_node_options_fini(&options));
  });

  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options));
  });

  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });

  ret = rcl_node_init(&node, "node", "namespace", &context, &options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node));
  });

  const rosidl_message_type_support_t * pn =
    ROSIDL_GET_MSG_TYPE_SUPPORT(lifecycle_msgs, msg, TransitionEvent);
  const rosidl_service_type_support_t * cs =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, ChangeState);
  const rosidl_service_type_support_t * gs =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetState);
  const rosidl_service_type_support_t * gas =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableStates);
  const rosidl_service_type_support_t * gat =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableTransitions);
  const rosidl_service_type_support_t * gtg =
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableTransitions);

  RCUTILS_FAULT_INJECTION_TEST(
  {
    // Init segfaults if this is not zero initialized
    rcl_lifecycle_state_machine_t sm = rcl_lifecycle_get_zero_initialized_state_machine();

    rcl_lifecycle_state_machine_options_t state_machine_options =
    rcl_lifecycle_get_default_state_machine_options();

    ret = rcl_lifecycle_state_machine_init(
      &sm, &node, pn, cs, gs, gas, gat, gtg, &state_machine_options);
    if (RCL_RET_OK == ret) {
      ret = rcl_lifecycle_state_machine_fini(&sm, &node);
      if (RCL_RET_OK != ret) {
        EXPECT_EQ(RCL_RET_OK, rcl_lifecycle_state_machine_fini(&sm, &node));
      }
    }
  });
}
