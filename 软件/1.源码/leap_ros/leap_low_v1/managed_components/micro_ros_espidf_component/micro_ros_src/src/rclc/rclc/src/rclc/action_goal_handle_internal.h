// Copyright (c) 2021 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
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
#ifndef RCLC__ACTION_GOAL_HANDLE_INTERNAL_H_
#define RCLC__ACTION_GOAL_HANDLE_INTERNAL_H_

#if __cplusplus
extern "C"
{
#endif

#include <rclc/action_goal_handle.h>

typedef int8_t rcl_action_cancel_state_t;
#define CANCEL_STATE_OK action_msgs__srv__CancelGoal_Response__ERROR_NONE
#define CANCEL_STATE_REJECTED action_msgs__srv__CancelGoal_Response__ERROR_REJECTED
#define CANCEL_STATE_UNKNOWN_GOAL action_msgs__srv__CancelGoal_Response__ERROR_UNKNOWN_GOAL_ID
#define CANCEL_STATE_TERMINATED action_msgs__srv__CancelGoal_Response__ERROR_GOAL_TERMINATED

void rclc_action_put_goal_handle_in_list(
  rclc_action_goal_handle_t ** list,
  rclc_action_goal_handle_t * goal_handle);

bool rclc_action_check_handle_in_list(
  rclc_action_goal_handle_t ** list,
  rclc_action_goal_handle_t * goal_handle);

rclc_action_goal_handle_t * rclc_action_pop_first_goal_handle_from_list(
  rclc_action_goal_handle_t ** list);

bool rclc_action_pop_goal_handle_from_list(
  rclc_action_goal_handle_t ** list,
  rclc_action_goal_handle_t * goal_handle);

rclc_action_goal_handle_t * rclc_action_take_goal_handle(
  void * untyped_entity);

void rclc_action_init_goal_handle_memory(
  void * untyped_entity);

void rclc_action_remove_used_goal_handle(
  void * untyped_entity,
  rclc_action_goal_handle_t * goal_handle);

rclc_action_goal_handle_t * rclc_action_find_goal_handle_by_uuid(
  void * untyped_entity,
  const unique_identifier_msgs__msg__UUID * uuid_msg);

rclc_action_goal_handle_t * rclc_action_find_first_handle_by_status(
  void * untyped_entity,
  rcl_action_goal_state_t status);

rclc_action_goal_handle_t * rclc_action_find_next_handle_by_status(
  rclc_action_goal_handle_t * handle,
  rcl_action_goal_state_t status);

rclc_action_goal_handle_t * rclc_action_find_handle_by_goal_request_sequence_number(
  void * untyped_entity,
  const int64_t goal_request_sequence_number);

rclc_action_goal_handle_t * rclc_action_find_first_terminated_handle(
  void * untyped_entity);

rclc_action_goal_handle_t * rclc_action_find_handle_by_result_request_sequence_number(
  void * untyped_entity,
  const int64_t result_request_sequence_number);

rclc_action_goal_handle_t * rclc_action_find_handle_by_cancel_request_sequence_number(
  void * untyped_entity,
  const int64_t cancel_request_sequence_number);

rclc_action_goal_handle_t * rclc_action_find_first_handle_with_goal_response(
  void * untyped_entity);

rclc_action_goal_handle_t * rclc_action_find_first_handle_with_feedback(
  void * untyped_entity);

rclc_action_goal_handle_t * rclc_action_find_first_handle_with_result_response(
  void * untyped_entity);

rclc_action_goal_handle_t * rclc_action_find_first_handle_with_cancel_response(
  void * untyped_entity);

#if __cplusplus
}
#endif

#endif  // RCLC__ACTION_GOAL_HANDLE_INTERNAL_H_
