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

#include <rclc/action_server.h>

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>

#include "./action_generic_types.h"
#include "./action_goal_handle_internal.h"

rcl_ret_t
rclc_action_server_init_default(
  rclc_action_server_t * action_server,
  rcl_node_t * node,
  rclc_support_t * support,
  const rosidl_action_type_support_t * type_support,
  const char * action_name)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    action_server, "action_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    support, "support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    type_support, "type_support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    action_name, "action_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  *action_server = (rclc_action_server_t) {0};

  action_server->rcl_handle = rcl_action_get_zero_initialized_server();
  rcl_action_server_options_t action_server_opt = rcl_action_server_get_default_options();
  rcl_ret_t rc = rcl_action_server_init(
    &action_server->rcl_handle,
    node,
    &support->clock,
    type_support,
    action_name,
    &action_server_opt);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_action_server_init_default, rcl_action_server_init);
  }

  return rc;
}

static bool rclc_action_server_is_valid_handle(
  rclc_action_goal_handle_t * goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return false);

  return rclc_action_check_handle_in_list(
    &goal_handle->action_server->used_goal_handles,
    goal_handle);
}

rcl_ret_t
rclc_action_server_response_goal_request(
  rclc_action_goal_handle_t * goal_handle,
  const bool accepted)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rclc_action_server_t * action_server = goal_handle->action_server;

  Generic_SendGoal_Response res = {0};
  res.accepted = accepted;

  rcl_ret_t rc = rcl_action_send_goal_response(
    &action_server->rcl_handle,
    &goal_handle->goal_request_header, &res);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_action_server_response_goal_request, rcl_action_send_goal_response);
    return rc;
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_action_server_goal_cancel_accept(
  rclc_action_goal_handle_t * goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_action_cancel_response_t cancel_response =
    rcl_action_get_zero_initialized_cancel_response();
  cancel_response.msg.return_code = CANCEL_STATE_OK;

  action_msgs__msg__GoalInfo goal_info;
  goal_info.goal_id = goal_handle->goal_id;

  cancel_response.msg.goals_canceling.data = &goal_info;
  cancel_response.msg.goals_canceling.size = 1;
  cancel_response.msg.goals_canceling.capacity = 1;

  return rcl_action_send_cancel_response(
    &goal_handle->action_server->rcl_handle,
    &goal_handle->cancel_request_header, &cancel_response.msg);
}

rcl_ret_t
rclc_action_server_goal_cancel_reject(
  rclc_action_server_t * action_server,
  rcl_action_cancel_state_t state,
  rmw_request_id_t cancel_request_header)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    action_server, "action_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_action_cancel_response_t cancel_response =
    rcl_action_get_zero_initialized_cancel_response();
  cancel_response.msg.return_code = state;

  return rcl_action_send_cancel_response(
    &action_server->rcl_handle,
    &cancel_request_header, &cancel_response.msg);
}

rcl_ret_t rclc_action_publish_feedback(
  rclc_action_goal_handle_t * goal_handle,
  void * ros_feedback)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    ros_feedback, "ros_feedback is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (!rclc_action_server_is_valid_handle(goal_handle)) {
    return RCL_RET_INVALID_ARGUMENT;
  }

  Generic_FeedbackMessage * feedback = (Generic_FeedbackMessage *) ros_feedback;

  memcpy(
    feedback->goal_id.uuid, goal_handle->goal_id.uuid,
    sizeof(feedback->goal_id.uuid));
  return rcl_action_publish_feedback(&goal_handle->action_server->rcl_handle, feedback);
}

rcl_ret_t rclc_action_send_result(
  rclc_action_goal_handle_t * goal_handle,
  rcl_action_goal_state_t status,
  void * ros_response)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    ros_response, "ros_response is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (!rclc_action_server_is_valid_handle(goal_handle)) {
    return RCL_RET_INVALID_ARGUMENT;
  }

  if (status <= GOAL_STATE_CANCELING) {
    return RCL_RET_INVALID_ARGUMENT;
  } else if (goal_handle->status != GOAL_STATE_EXECUTING && // NOLINT
    goal_handle->status != GOAL_STATE_CANCELING)
  {
    return RCLC_RET_ACTION_WAIT_RESULT_REQUEST;
  }

  Generic_GetResult_Response * response = (Generic_GetResult_Response *)ros_response;
  response->status = status;

  rcl_ret_t rc = rcl_action_send_result_response(
    &goal_handle->action_server->rcl_handle,
    &goal_handle->result_request_header, response);

  goal_handle->status = status;
  goal_handle->action_server->goal_ended = true;

  return rc;
}


rcl_ret_t
rclc_action_server_fini(
  rclc_action_server_t * action_server,
  rcl_node_t * node)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    action_server, "action_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rc;

  if (NULL != action_server->goal_handles_memory) {
    action_server->allocator->deallocate(
      action_server->goal_handles_memory,
      action_server->allocator->state);
    action_server->goal_handles_memory = NULL;
  }

  rc = rcl_action_server_fini(&action_server->rcl_handle, node);

  return rc;
}
