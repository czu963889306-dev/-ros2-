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

#include <time.h>

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>
#include <rclc/action_client.h>

#include "./action_generic_types.h"
#include "./action_goal_handle_internal.h"

rcl_ret_t
rclc_action_client_init_default(
  rclc_action_client_t * action_client,
  rcl_node_t * node,
  const rosidl_action_type_support_t * type_support,
  const char * action_name)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    action_client, "action_client is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    type_support, "type_support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    action_name, "action_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  *action_client = (rclc_action_client_t) {0};

  action_client->rcl_handle = rcl_action_get_zero_initialized_client();
  rcl_action_client_options_t action_client_opt = rcl_action_client_get_default_options();
  rcl_ret_t rc = rcl_action_client_init(
    &action_client->rcl_handle,
    node,
    type_support,
    action_name,
    &action_client_opt);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_client_init_default, rcl_client_init);
  }

  return rc;
}

static void set_uuid(
  uint8_t * uuid)
{
  static bool uuid_gen_init = false;
  static uint64_t uuid_lsb = 0;
  static uint64_t uuid_msb = 0;

  RCL_CHECK_FOR_NULL_WITH_MSG(
    uuid, "uuid is a null pointer", return );

  if (!uuid_gen_init) {
    uuid_gen_init = true;
    srand(time(NULL));
    uuid_lsb = (uint64_t)rand();
    uuid_msb = (uint64_t)rand();
  }

  uuid_lsb++;
  if (0 == uuid_lsb) {
    uuid_msb++;
  }

  *(uint64_t *)(&uuid[0]) = uuid_msb;
  *(uint64_t *)(&uuid[8]) = uuid_lsb;
}

rcl_ret_t
rclc_action_send_goal_request(
  rclc_action_client_t * action_client,
  void * ros_request,
  rclc_action_goal_handle_t ** goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    action_client, "action_client is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    ros_request, "ros_request is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  Generic_SendGoal_Request * request = (Generic_SendGoal_Request *) ros_request;
  rclc_action_goal_handle_t * handle = rclc_action_take_goal_handle(action_client);

  if (NULL == handle) {
    PRINT_RCLC_ERROR(rclc_action_send_goal_request, rclc_action_take_goal_handle);
    return RCL_RET_ERROR;
  }

  set_uuid(handle->goal_id.uuid);
  request->goal_id = handle->goal_id;

  rcl_ret_t rc = rcl_action_send_goal_request(
    &action_client->rcl_handle, ros_request,
    &handle->goal_request_sequence_number);

  if (rc != RCL_RET_OK) {
    rclc_action_remove_used_goal_handle(action_client, handle);
    PRINT_RCLC_ERROR(rclc_action_send_goal_request, rcl_action_send_goal_request);
    return RCL_RET_ERROR;
  }

  handle->status = GOAL_STATE_UNKNOWN;
  handle->ros_goal_request = ros_request;
  handle->available_goal_response = false;
  handle->available_feedback = false;
  handle->available_result_response = false;
  handle->available_cancel_response = false;

  if (NULL != goal_handle) {
    *goal_handle = handle;
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_action_send_result_request(
  rclc_action_goal_handle_t * goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  Generic_GetResult_Request result_request;

  result_request.goal_id = goal_handle->goal_id;

  rcl_ret_t rc = rcl_action_send_result_request(
    &goal_handle->action_client->rcl_handle,
    &result_request,
    &goal_handle->result_request_sequence_number);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_action_send_result_request, rcl_action_send_result_request);
    return rc;
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_action_send_cancel_request(
  rclc_action_goal_handle_t * goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  action_msgs__srv__CancelGoal_Request cancel_request;

  cancel_request.goal_info.goal_id = goal_handle->goal_id;

  rcl_ret_t rc = rcl_action_send_cancel_request(
    &goal_handle->action_client->rcl_handle,
    &cancel_request,
    &goal_handle->cancel_request_sequence_number);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_action_send_cancel_request, rcl_action_send_cancel_request);
    return rc;
  }

  return RCL_RET_OK;
}


rcl_ret_t
rclc_action_client_fini(
  rclc_action_client_t * action_client,
  rcl_node_t * node)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    action_client, "action_client is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rc;

  if (NULL != action_client->goal_handles_memory) {
    action_client->allocator->deallocate(
      action_client->goal_handles_memory,
      action_client->allocator->state);
    action_client->goal_handles_memory = NULL;
  }

  if (NULL != action_client->ros_cancel_response.goals_canceling.data) {
    action_client->allocator->deallocate(
      action_client->ros_cancel_response.goals_canceling.data,
      action_client->allocator->state);
    action_client->ros_cancel_response.goals_canceling.data = NULL;
  }

  rc = rcl_action_client_fini(&action_client->rcl_handle, node);

  return rc;
}
