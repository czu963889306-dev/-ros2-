// Copyright 2016 Open Source Robotics Foundation, Inc.
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
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "lifecycle_msgs/msg/state.h"
#include "lifecycle_msgs/msg/transition.h"

#include "rcl/error_handling.h"
#include "rcl/rcl.h"

#include "rcutils/logging_macros.h"
#include "rcutils/testing/fault_injection.h"

#include "rcl_lifecycle/rcl_lifecycle.h"
#include "rcl_lifecycle/default_state_machine.h"

class TestDefaultStateMachine : public ::testing::Test
{
public:
  rcl_context_t * context_ptr;
  rcl_node_t * node_ptr;
  const rcl_allocator_t * allocator;
  rcl_lifecycle_state_machine_options_t state_machine_options;
  void SetUp()
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
      this->context_ptr = new rcl_context_t;
      *this->context_ptr = rcl_get_zero_initialized_context();
      ret = rcl_init(0, nullptr, &init_options, this->context_ptr);
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    }
    this->node_ptr = new rcl_node_t;
    *this->node_ptr = rcl_get_zero_initialized_node();
    const char * name = "test_state_machine_node";
    rcl_node_options_t node_options = rcl_node_get_default_options();
    ret = rcl_node_init(this->node_ptr, name, "", this->context_ptr, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    const rcl_node_options_t * node_ops = rcl_node_get_options(this->node_ptr);
    state_machine_options = rcl_lifecycle_get_default_state_machine_options();
    state_machine_options.allocator = node_ops->allocator;
  }

  void TearDown()
  {
    rcl_ret_t ret = rcl_node_fini(this->node_ptr);
    delete this->node_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_shutdown(this->context_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(this->context_ptr);
    delete this->context_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
};

void
test_trigger_transition(
  rcl_lifecycle_state_machine_t * state_machine,
  uint8_t key_id,
  unsigned int expected_current_state,
  unsigned int expected_goal_state)
{
  EXPECT_EQ(
    expected_current_state, state_machine->current_state->id);
  EXPECT_EQ(
    RCL_RET_OK, rcl_lifecycle_trigger_transition_by_id(
      state_machine, key_id, false));
  EXPECT_EQ(
    expected_goal_state, state_machine->current_state->id);
}

/*
 * Test suite
 */
TEST_F(TestDefaultStateMachine, zero_init) {
  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();
  EXPECT_EQ(rcl_lifecycle_state_machine_is_initialized(&state_machine), RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();
  const rcl_lifecycle_transition_map_t * transition_map = &state_machine.transition_map;
  EXPECT_EQ(transition_map->states_size, 0u);
  EXPECT_EQ(transition_map->states, nullptr);
  EXPECT_EQ(transition_map->transitions_size, 0u);
  EXPECT_EQ(transition_map->transitions, nullptr);

  auto ret = rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(TestDefaultStateMachine, default_init) {
  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();

  // Because this init method is so complex, the succession of failures caused by a null
  // allocator will result in several error messages overwriting themselves.
  auto ret = rcl_lifecycle_init_default_state_machine(&state_machine, nullptr);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcutils_reset_error();

  state_machine.options = this->state_machine_options;
  ret = rcl_lifecycle_init_default_state_machine(&state_machine, &state_machine.options.allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(TestDefaultStateMachine, default_sequence) {
  rcl_ret_t ret;

  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();
  state_machine.options = this->state_machine_options;
  ret = rcl_lifecycle_init_default_state_machine(&state_machine, &state_machine.options.allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_UNCONFIGURED_SHUTDOWN,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN,
    lifecycle_msgs__msg__State__PRIMARY_STATE_FINALIZED);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr));
}

TEST_F(TestDefaultStateMachine, wrong_default_sequence) {
  rcl_ret_t ret;

  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();
  state_machine.options = this->state_machine_options;
  ret = rcl_lifecycle_init_default_state_machine(&state_machine, &state_machine.options.allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  std::vector<uint8_t> transition_ids =
  {
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
    lifecycle_msgs__msg__Transition__TRANSITION_UNCONFIGURED_SHUTDOWN,
    lifecycle_msgs__msg__Transition__TRANSITION_INACTIVE_SHUTDOWN,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVE_SHUTDOWN,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_FAILURE,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_ERROR,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_SUCCESS,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_FAILURE,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_ERROR,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_SUCCESS,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_FAILURE,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_ERROR,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_SUCCESS,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_FAILURE,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_ERROR,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_SUCCESS,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_FAILURE,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_ERROR,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_SUCCESS,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_FAILURE,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_ERROR,
  };

  { // supposed to stay unconfigured for all invalid
    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      if (*it == lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_UNCONFIGURED_SHUTDOWN) {continue;}

      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);
    }
  }

  { // supposed to stay configuring for all invalid
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      if (*it == lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_FAILURE ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_ERROR) {continue;}

      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);
    }
  }

  { // supposed to stay inactive for all invalid
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      if (*it == lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_INACTIVE_SHUTDOWN) {continue;}

      RCUTILS_LOG_INFO_NAMED(ROS_PACKAGE_NAME, "applying key %u", *it);
      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);
    }
  }

  { // supposed to stay activating for all invalid
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);

    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      if (*it == lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_SUCCESS ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_FAILURE ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_ERROR) {continue;}

      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);
    }
  }

  { // supposed to stay active for all invalid
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE);

    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      if (*it == lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ACTIVE_SHUTDOWN)
      {continue;}

      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE);
    }
  }

  { // supposed to stay deactivating for all invalid
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE,
      lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING);

    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      if (*it == lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_SUCCESS ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_FAILURE ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_ERROR) {continue;}

      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING);
    }
  }

  { // supposed to stay cleanup for all invalid
    // skip inactive, we tested that already
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP);

    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      if (*it == lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_SUCCESS ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_FAILURE ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_ERROR) {continue;}

      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP);
    }
  }

  { // supposed to stay shutting down for all invalid
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP,
      lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);
    // shutdown directly, since we tested already unconfigured
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_UNCONFIGURED_SHUTDOWN,
      lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
      lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN);

    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      if (*it == lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_SUCCESS ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_FAILURE ||
        *it == lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_ERROR) {continue;}

      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN);
    }
  }

  { // supposed to stay finalized for all invalid
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN,
      lifecycle_msgs__msg__State__PRIMARY_STATE_FINALIZED);

    for (auto it = transition_ids.begin(); it != transition_ids.end(); ++it) {
      EXPECT_EQ(
        RCL_RET_INVALID_ARGUMENT,
        rcl_lifecycle_trigger_transition_by_id(&state_machine, *it, false));
      rcl_reset_error();
      EXPECT_EQ(
        state_machine.current_state->id,
        lifecycle_msgs__msg__State__PRIMARY_STATE_FINALIZED);
    }
  }

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr));
}

TEST_F(TestDefaultStateMachine, default_in_a_loop) {
  rcl_ret_t ret;

  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();
  state_machine.options = this->state_machine_options;
  ret = rcl_lifecycle_init_default_state_machine(&state_machine, &state_machine.options.allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  for (auto i = 0; i < 5; ++i) {
    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE,
      lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP,
      lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);
  }

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_UNCONFIGURED_SHUTDOWN,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN,
    lifecycle_msgs__msg__State__PRIMARY_STATE_FINALIZED);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr));
}

TEST_F(TestDefaultStateMachine, default_sequence_failure) {
  rcl_ret_t ret;

  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();
  state_machine.options = this->state_machine_options;
  ret = rcl_lifecycle_init_default_state_machine(&state_machine, &state_machine.options.allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_FAILURE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);

  ///////////////
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_FAILURE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

  //////////////////////////
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_FAILURE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE);

  //////////////////////////////
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_FAILURE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

  /////////////////////////////
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_UNCONFIGURED_SHUTDOWN,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_FAILURE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN,
    lifecycle_msgs__msg__State__PRIMARY_STATE_FINALIZED);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr));
}

TEST_F(TestDefaultStateMachine, default_sequence_error_resolved) {
  rcl_ret_t ret;

  rcl_lifecycle_state_machine_t state_machine = rcl_lifecycle_get_zero_initialized_state_machine();
  state_machine.options = this->state_machine_options;
  ret = rcl_lifecycle_init_default_state_machine(&state_machine, &state_machine.options.allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_ERROR,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);

  ///////////////
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_ERROR,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);

  //////////////////////////
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_ACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_ERROR,
    lifecycle_msgs__msg__State__TRANSITION_STATE_DEACTIVATING,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);

  //////////////////////////////
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
    lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_ERROR,
    lifecycle_msgs__msg__State__TRANSITION_STATE_CLEANINGUP,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING);

  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);

  /////////////////////////////
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_UNCONFIGURED_SHUTDOWN,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
    lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_SHUTDOWN_ERROR,
    lifecycle_msgs__msg__State__TRANSITION_STATE_SHUTTINGDOWN,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING);
  test_trigger_transition(
    &state_machine,
    lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_SUCCESS,
    lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING,
    lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr));
}

TEST_F(TestDefaultStateMachine, default_sequence_error_unresolved) {
  rcl_ret_t ret;

  {
    rcl_lifecycle_state_machine_t state_machine =
      rcl_lifecycle_get_zero_initialized_state_machine();
    state_machine.options = this->state_machine_options;
    ret = rcl_lifecycle_init_default_state_machine(
      &state_machine, &state_machine.options.allocator);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_ERROR,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_FAILURE,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_FINALIZED);

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr));
  }

  {
    rcl_lifecycle_state_machine_t state_machine =
      rcl_lifecycle_get_zero_initialized_state_machine();
    state_machine.options = this->state_machine_options;
    ret = rcl_lifecycle_init_default_state_machine(
      &state_machine, &state_machine.options.allocator);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_UNCONFIGURED,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
      lifecycle_msgs__msg__State__TRANSITION_STATE_CONFIGURING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
      lifecycle_msgs__msg__State__PRIMARY_STATE_INACTIVE,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_ERROR,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ACTIVATING,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING);

    test_trigger_transition(
      &state_machine,
      lifecycle_msgs__msg__Transition__TRANSITION_ON_ERROR_ERROR,
      lifecycle_msgs__msg__State__TRANSITION_STATE_ERRORPROCESSING,
      lifecycle_msgs__msg__State__PRIMARY_STATE_FINALIZED);

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_lifecycle_state_machine_fini(&state_machine, this->node_ptr));
  }
}

TEST_F(TestDefaultStateMachine, init_fini_maybe_fail) {
  rcl_lifecycle_state_machine_t sm = rcl_lifecycle_get_zero_initialized_state_machine();
  sm.options = this->state_machine_options;
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_ret_t ret = rcl_lifecycle_init_default_state_machine(&sm, &sm.options.allocator);
    if (RCL_RET_OK == ret) {
      ret = rcl_lifecycle_state_machine_fini(&sm, this->node_ptr);
      if (RCL_RET_OK != ret) {
        EXPECT_EQ(
          RCL_RET_OK,
          rcl_lifecycle_state_machine_fini(&sm, this->node_ptr));
      }
    }
  });
}
