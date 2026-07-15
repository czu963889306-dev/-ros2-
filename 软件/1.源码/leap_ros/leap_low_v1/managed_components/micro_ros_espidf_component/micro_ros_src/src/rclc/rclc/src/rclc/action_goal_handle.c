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

#include "rclc/action_server.h"

#include "./action_generic_types.h"

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>


typedef struct rclc_generic_entity_t
{
  DECLARE_GOAL_HANDLE_POOL
} rclc_generic_entity_t;


void rclc_action_put_goal_handle_in_list(
  rclc_action_goal_handle_t ** list,
  rclc_action_goal_handle_t * goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    list, "list is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return );

  goal_handle->next = *list;
  *list = goal_handle;
}

bool rclc_action_check_handle_in_list(
  rclc_action_goal_handle_t ** list,
  rclc_action_goal_handle_t * goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    list, "list is a null pointer", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return false);

  rclc_action_goal_handle_t * handle = *list;
  while (NULL != handle) {
    if (handle == goal_handle) {
      return true;
    }
    handle = handle->next;
  }
  return false;
}

rclc_action_goal_handle_t * rclc_action_pop_first_goal_handle_from_list(
  rclc_action_goal_handle_t ** list)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    list, "list is a null pointer", return NULL);

  rclc_action_goal_handle_t * handle = *list;
  *list = (*list == NULL ) ? NULL : (*list)->next;
  return handle;
}

bool rclc_action_pop_goal_handle_from_list(
  rclc_action_goal_handle_t ** list,
  rclc_action_goal_handle_t * goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    list, "list is a null pointer", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return false);

  rclc_action_goal_handle_t * handle = *list;
  if (goal_handle == handle) {
    *list = handle->next;
    return true;
  }

  while (NULL != handle) {
    if (handle->next == goal_handle) {
      handle->next = goal_handle->next;
      return true;
    }
    handle = handle->next;
  }
  return false;
}

rclc_action_goal_handle_t * rclc_action_take_goal_handle(
  void * untyped_entity)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = rclc_action_pop_first_goal_handle_from_list(
    &entity->free_goal_handles);

  if (NULL != handle) {
    // Initialize handle
    handle->available_goal_response = false;
    handle->goal_accepted = false;
    handle->available_feedback = false;
    handle->available_result_response = false;
    handle->available_cancel_response = false;
    handle->goal_cancelled = false;
    handle->status = GOAL_STATE_UNKNOWN;
    rclc_action_put_goal_handle_in_list(&entity->used_goal_handles, handle);
  }

  return handle;
}

void rclc_action_init_goal_handle_memory(
  void * untyped_entity)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return );

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  entity->free_goal_handles = entity->goal_handles_memory;
  size_t size = entity->goal_handles_memory_size;
  for (size_t i = 0; i < size - 1; i++) {
    entity->free_goal_handles[i].next = &entity->free_goal_handles[i + 1];
  }
  entity->free_goal_handles[size - 1].next = NULL;
}

void rclc_action_remove_used_goal_handle(
  void * untyped_entity,
  rclc_action_goal_handle_t * goal_handle)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    goal_handle, "goal_handle is a null pointer", return );

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  if (rclc_action_pop_goal_handle_from_list(&entity->used_goal_handles, goal_handle)) {
    rclc_action_put_goal_handle_in_list(&entity->free_goal_handles, goal_handle);
  }
}

rclc_action_goal_handle_t * rclc_action_find_goal_handle_by_uuid(
  void * untyped_entity,
  const unique_identifier_msgs__msg__UUID * uuid_msg)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    uuid_msg, "uuid_msg is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (uuidcmp(handle->goal_id.uuid, uuid_msg->uuid)) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_first_handle_by_status(
  void * untyped_entity,
  rcl_action_goal_state_t status)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->status == status) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_next_handle_by_status(
  rclc_action_goal_handle_t * handle,
  rcl_action_goal_state_t status)
{
  while (NULL != handle) {
    if (handle->status == status) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_first_terminated_handle(
  void * untyped_entity)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->status > GOAL_STATE_CANCELING) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_handle_by_goal_request_sequence_number(
  void * untyped_entity,
  const int64_t goal_request_sequence_number)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->goal_request_sequence_number == goal_request_sequence_number) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_handle_by_result_request_sequence_number(
  void * untyped_entity,
  const int64_t result_request_sequence_number)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->result_request_sequence_number == result_request_sequence_number) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_handle_by_cancel_request_sequence_number(
  void * untyped_entity,
  const int64_t cancel_request_sequence_number)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->cancel_request_sequence_number == cancel_request_sequence_number) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_first_handle_with_goal_response(
  void * untyped_entity)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->available_goal_response) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_first_handle_with_feedback(
  void * untyped_entity)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->available_feedback) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_first_handle_with_result_response(
  void * untyped_entity)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->available_result_response) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}

rclc_action_goal_handle_t * rclc_action_find_first_handle_with_cancel_response(
  void * untyped_entity)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    untyped_entity, "untyped_entity is a null pointer", return NULL);

  rclc_generic_entity_t * entity = (rclc_generic_entity_t *) untyped_entity;
  rclc_action_goal_handle_t * handle = entity->used_goal_handles;
  while (NULL != handle) {
    if (handle->available_cancel_response) {
      return handle;
    }
    handle = handle->next;
  }
  return handle;
}
