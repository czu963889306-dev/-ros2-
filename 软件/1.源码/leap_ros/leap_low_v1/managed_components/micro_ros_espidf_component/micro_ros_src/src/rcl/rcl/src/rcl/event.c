// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include "rcl/event.h"

#include <stdio.h>

#include "rcl/error_handling.h"
#include "rcl/expand_topic_name.h"
#include "rcl/remap.h"
#include "rcutils/allocator.h"
#include "rcutils/logging_macros.h"
#include "rmw/error_handling.h"
#include "rmw/validate_full_topic_name.h"
#include "rmw/event.h"

#include "./common.h"
#include "./event_impl.h"
#include "./publisher_impl.h"
#include "./subscription_impl.h"

rcl_event_t
rcl_get_zero_initialized_event()
{
  static rcl_event_t null_event = {0};
  return null_event;
}

rcl_ret_t
rcl_publisher_event_init(
  rcl_event_t * event,
  const rcl_publisher_t * publisher,
  const rcl_publisher_event_type_t event_type)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(event, RCL_RET_EVENT_INVALID);
  // Check publisher and allocator first, so allocator can be used with errors.
  RCL_CHECK_ARGUMENT_FOR_NULL(publisher, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = &publisher->impl->options.allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  rmw_event_type_t rmw_event_type = RMW_EVENT_INVALID;
  switch (event_type) {
    case RCL_PUBLISHER_OFFERED_DEADLINE_MISSED:
      rmw_event_type = RMW_EVENT_OFFERED_DEADLINE_MISSED;
      break;
    case RCL_PUBLISHER_LIVELINESS_LOST:
      rmw_event_type = RMW_EVENT_LIVELINESS_LOST;
      break;
    case RCL_PUBLISHER_OFFERED_INCOMPATIBLE_QOS:
      rmw_event_type = RMW_EVENT_OFFERED_QOS_INCOMPATIBLE;
      break;
    default:
      RCL_SET_ERROR_MSG("Event type for publisher not supported");
      return RCL_RET_INVALID_ARGUMENT;
  }

  // Allocate space for the implementation struct.
  event->impl = (rcl_event_impl_t *) allocator->allocate(
    sizeof(rcl_event_impl_t), allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    event->impl, "allocating memory failed", return RCL_RET_BAD_ALLOC);

  event->impl->rmw_handle = rmw_get_zero_initialized_event();
  event->impl->allocator = *allocator;

  rmw_ret_t ret = rmw_publisher_event_init(
    &event->impl->rmw_handle,
    publisher->impl->rmw_handle,
    rmw_event_type);
  if (ret != RMW_RET_OK) {
    goto fail;
  }

  return RCL_RET_OK;
fail:
  allocator->deallocate(event->impl, allocator->state);
  event->impl = NULL;
  return rcl_convert_rmw_ret_to_rcl_ret(ret);
}

rcl_ret_t
rcl_subscription_event_init(
  rcl_event_t * event,
  const rcl_subscription_t * subscription,
  const rcl_subscription_event_type_t event_type)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(event, RCL_RET_EVENT_INVALID);
  // Check subscription and allocator first, so allocator can be used with errors.
  RCL_CHECK_ARGUMENT_FOR_NULL(subscription, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = &subscription->impl->options.allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  rmw_event_type_t rmw_event_type = RMW_EVENT_INVALID;
  switch (event_type) {
    case RCL_SUBSCRIPTION_REQUESTED_DEADLINE_MISSED:
      rmw_event_type = RMW_EVENT_REQUESTED_DEADLINE_MISSED;
      break;
    case RCL_SUBSCRIPTION_LIVELINESS_CHANGED:
      rmw_event_type = RMW_EVENT_LIVELINESS_CHANGED;
      break;
    case RCL_SUBSCRIPTION_REQUESTED_INCOMPATIBLE_QOS:
      rmw_event_type = RMW_EVENT_REQUESTED_QOS_INCOMPATIBLE;
      break;
    case RCL_SUBSCRIPTION_MESSAGE_LOST:
      rmw_event_type = RMW_EVENT_MESSAGE_LOST;
      break;
    default:
      RCL_SET_ERROR_MSG("Event type for subscription not supported");
      return RCL_RET_INVALID_ARGUMENT;
  }

  // Allocate space for the implementation struct.
  event->impl = (rcl_event_impl_t *) allocator->allocate(
    sizeof(rcl_event_impl_t), allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    event->impl, "allocating memory failed", return RCL_RET_BAD_ALLOC);

  event->impl->rmw_handle = rmw_get_zero_initialized_event();
  event->impl->allocator = *allocator;

  rmw_ret_t ret = rmw_subscription_event_init(
    &event->impl->rmw_handle,
    subscription->impl->rmw_handle,
    rmw_event_type);
  if (ret != RMW_RET_OK) {
    goto fail;
  }

  return RCL_RET_OK;
fail:
  allocator->deallocate(event->impl, allocator->state);
  event->impl = NULL;
  return rcl_convert_rmw_ret_to_rcl_ret(ret);
}

rcl_ret_t
rcl_take_event(
  const rcl_event_t * event,
  void * event_info)
{
  bool taken = false;
  if (!rcl_event_is_valid(event)) {
    return RCL_RET_EVENT_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(event_info, RCL_RET_INVALID_ARGUMENT);
  rmw_ret_t ret = rmw_take_event(&event->impl->rmw_handle, event_info, &taken);
  if (RMW_RET_OK != ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }
  if (!taken) {
    RCUTILS_LOG_DEBUG_NAMED(
      ROS_PACKAGE_NAME, "take_event request complete, unable to take event");
    return RCL_RET_EVENT_TAKE_FAILED;
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "take_event request success");
  return rcl_convert_rmw_ret_to_rcl_ret(ret);
}

rcl_ret_t
rcl_event_fini(rcl_event_t * event)
{
  rcl_ret_t result = RCL_RET_OK;
  RCL_CHECK_ARGUMENT_FOR_NULL(event, RCL_RET_EVENT_INVALID);

  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Finalizing event");
  if (NULL != event->impl) {
    rcl_allocator_t allocator = event->impl->allocator;
    rmw_ret_t ret = rmw_event_fini(&event->impl->rmw_handle);
    if (ret != RMW_RET_OK) {
      RCL_SET_ERROR_MSG(rmw_get_error_string().str);
      result = rcl_convert_rmw_ret_to_rcl_ret(ret);
    }
    allocator.deallocate(event->impl, allocator.state);
    event->impl = NULL;
  }
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Event finalized");

  return result;
}

rmw_event_t *
rcl_event_get_rmw_handle(const rcl_event_t * event)
{
  if (!rcl_event_is_valid(event)) {
    return NULL;  // error already set
  } else {
    return &event->impl->rmw_handle;
  }
}

bool
rcl_event_is_valid(const rcl_event_t * event)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(event, "event pointer is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(event->impl, "event's implementation is invalid", return false);
  if (event->impl->rmw_handle.event_type == RMW_EVENT_INVALID) {
    RCUTILS_SET_ERROR_MSG("event's implementation not init");
    return false;
  }
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &event->impl->allocator, "not valid allocator", return false);
  return true;
}

rcl_ret_t
rcl_event_set_callback(
  const rcl_event_t * event,
  rcl_event_callback_t callback,
  const void * user_data)
{
  if (!rcl_event_is_valid(event)) {
    // error state already set
    return RCL_RET_INVALID_ARGUMENT;
  }

  return rmw_event_set_callback(
    &event->impl->rmw_handle,
    callback,
    user_data);
}

#ifdef __cplusplus
}
#endif
