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

#include "rcl_action/goal_state_machine.h"


TEST(TestGoalStateMachine, test_valid_transitions)
{
  rcl_action_goal_state_t state = rcl_action_transition_goal_state(
    GOAL_STATE_ACCEPTED,
    GOAL_EVENT_EXECUTE);
  EXPECT_EQ(GOAL_STATE_EXECUTING, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_ACCEPTED,
    GOAL_EVENT_CANCEL_GOAL);
  EXPECT_EQ(GOAL_STATE_CANCELING, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_EXECUTING,
    GOAL_EVENT_CANCEL_GOAL);
  EXPECT_EQ(GOAL_STATE_CANCELING, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_EXECUTING,
    GOAL_EVENT_SUCCEED);
  EXPECT_EQ(GOAL_STATE_SUCCEEDED, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_EXECUTING,
    GOAL_EVENT_ABORT);
  EXPECT_EQ(GOAL_STATE_ABORTED, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELING,
    GOAL_EVENT_CANCELED);
  EXPECT_EQ(GOAL_STATE_CANCELED, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELING,
    GOAL_EVENT_ABORT);
  EXPECT_EQ(GOAL_STATE_ABORTED, state);
}

TEST(TestGoalStateMachine, test_invalid_transitions)
{
  // Invalid from ACCEPTED
  rcl_action_goal_state_t state = rcl_action_transition_goal_state(
    GOAL_STATE_ACCEPTED,
    GOAL_EVENT_SUCCEED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_ACCEPTED,
    GOAL_EVENT_ABORT);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_ACCEPTED,
    GOAL_EVENT_CANCELED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);

  // Invalid from EXECUTING
  state = rcl_action_transition_goal_state(
    GOAL_STATE_EXECUTING,
    GOAL_EVENT_EXECUTE);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_EXECUTING,
    GOAL_EVENT_CANCELED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);

  // Invalid from CANCELING
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELING,
    GOAL_EVENT_EXECUTE);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELING,
    GOAL_EVENT_CANCEL_GOAL);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);

  // Invalid from SUCCEEDED
  state = rcl_action_transition_goal_state(
    GOAL_STATE_SUCCEEDED,
    GOAL_EVENT_EXECUTE);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_SUCCEEDED,
    GOAL_EVENT_CANCEL_GOAL);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_SUCCEEDED,
    GOAL_EVENT_SUCCEED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_SUCCEEDED,
    GOAL_EVENT_ABORT);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_SUCCEEDED,
    GOAL_EVENT_CANCELED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);

  // Invalid from ABORTED
  state = rcl_action_transition_goal_state(
    GOAL_STATE_ABORTED,
    GOAL_EVENT_EXECUTE);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_ABORTED,
    GOAL_EVENT_CANCEL_GOAL);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_ABORTED,
    GOAL_EVENT_SUCCEED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_ABORTED,
    GOAL_EVENT_ABORT);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_ABORTED,
    GOAL_EVENT_CANCELED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);

  // Invalid from CANCELED
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELED,
    GOAL_EVENT_EXECUTE);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELED,
    GOAL_EVENT_CANCEL_GOAL);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELED,
    GOAL_EVENT_SUCCEED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELED,
    GOAL_EVENT_ABORT);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
  state = rcl_action_transition_goal_state(
    GOAL_STATE_CANCELED,
    GOAL_EVENT_CANCELED);
  EXPECT_EQ(GOAL_STATE_UNKNOWN, state);
}
