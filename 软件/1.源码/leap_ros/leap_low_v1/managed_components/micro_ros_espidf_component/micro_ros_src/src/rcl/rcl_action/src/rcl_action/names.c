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

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl_action/names.h"

#include <string.h>

#include "rcl/error_handling.h"
#include "rcutils/format_string.h"

rcl_ret_t
rcl_action_get_goal_service_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** goal_service_name)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(action_name, RCL_RET_INVALID_ARGUMENT);
  if (0 == strlen(action_name)) {
    RCL_SET_ERROR_MSG("invalid empty action name");
    return RCL_RET_ACTION_NAME_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(goal_service_name, RCL_RET_INVALID_ARGUMENT);
  if (NULL != *goal_service_name) {
    RCL_SET_ERROR_MSG("writing action goal service name may leak memory");
    return RCL_RET_INVALID_ARGUMENT;
  }
  *goal_service_name = rcutils_format_string(allocator, "%s/_action/send_goal", action_name);
  if (NULL == *goal_service_name) {
    RCL_SET_ERROR_MSG("failed to allocate memory for action goal service name");
    return RCL_RET_BAD_ALLOC;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_action_get_cancel_service_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** cancel_service_name)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(action_name, RCL_RET_INVALID_ARGUMENT);
  if (0 == strlen(action_name)) {
    RCL_SET_ERROR_MSG("invalid empty action name");
    return RCL_RET_ACTION_NAME_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(cancel_service_name, RCL_RET_INVALID_ARGUMENT);
  if (NULL != *cancel_service_name) {
    RCL_SET_ERROR_MSG("writing action cancel service name may leak memory");
    return RCL_RET_INVALID_ARGUMENT;
  }
  *cancel_service_name = rcutils_format_string(allocator, "%s/_action/cancel_goal", action_name);
  if (NULL == *cancel_service_name) {
    RCL_SET_ERROR_MSG("failed to allocate memory for action cancel service name");
    return RCL_RET_BAD_ALLOC;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_action_get_result_service_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** result_service_name)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(action_name, RCL_RET_INVALID_ARGUMENT);
  if (0 == strlen(action_name)) {
    RCL_SET_ERROR_MSG("invalid empty action name");
    return RCL_RET_ACTION_NAME_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(result_service_name, RCL_RET_INVALID_ARGUMENT);
  if (NULL != *result_service_name) {
    RCL_SET_ERROR_MSG("writing action result service name may leak memory");
    return RCL_RET_INVALID_ARGUMENT;
  }
  *result_service_name = rcutils_format_string(allocator, "%s/_action/get_result", action_name);
  if (NULL == *result_service_name) {
    RCL_SET_ERROR_MSG("failed to allocate memory for action result service name");
    return RCL_RET_BAD_ALLOC;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_action_get_feedback_topic_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** feedback_topic_name)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(action_name, RCL_RET_INVALID_ARGUMENT);
  if (0 == strlen(action_name)) {
    RCL_SET_ERROR_MSG("invalid empty action name");
    return RCL_RET_ACTION_NAME_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(feedback_topic_name, RCL_RET_INVALID_ARGUMENT);
  if (NULL != *feedback_topic_name) {
    RCL_SET_ERROR_MSG("writing action feedback topic name may leak memory");
    return RCL_RET_INVALID_ARGUMENT;
  }
  *feedback_topic_name = rcutils_format_string(allocator, "%s/_action/feedback", action_name);
  if (NULL == *feedback_topic_name) {
    RCL_SET_ERROR_MSG("failed to allocate memory for action feedback topic name");
    return RCL_RET_BAD_ALLOC;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_action_get_status_topic_name(
  const char * action_name,
  rcl_allocator_t allocator,
  char ** status_topic_name)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(action_name, RCL_RET_INVALID_ARGUMENT);
  if (0 == strlen(action_name)) {
    RCL_SET_ERROR_MSG("invalid empty action name");
    return RCL_RET_ACTION_NAME_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(status_topic_name, RCL_RET_INVALID_ARGUMENT);
  if (NULL != *status_topic_name) {
    RCL_SET_ERROR_MSG("writing action status topic name may leak memory");
    return RCL_RET_INVALID_ARGUMENT;
  }
  *status_topic_name = rcutils_format_string(allocator, "%s/_action/status", action_name);
  if (NULL == *status_topic_name) {
    RCL_SET_ERROR_MSG("failed to allocate memory for action status topic name");
    return RCL_RET_BAD_ALLOC;
  }
  return RCL_RET_OK;
}

#ifdef __cplusplus
}
#endif
