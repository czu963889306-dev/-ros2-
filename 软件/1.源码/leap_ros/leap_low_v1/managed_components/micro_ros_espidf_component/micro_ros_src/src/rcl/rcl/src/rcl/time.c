// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include "rcl/time.h"

#include <stdbool.h>
#include <stdlib.h>

#include "./common.h"
#include "rcl/allocator.h"
#include "rcl/error_handling.h"
#include "rcutils/macros.h"
#include "rcutils/stdatomic_helper.h"
#include "rcutils/time.h"

// Internal storage for RCL_ROS_TIME implementation
typedef struct rcl_ros_clock_storage_s
{
  atomic_uint_least64_t current_time;
  bool active;
} rcl_ros_clock_storage_t;

// Implementation only
static rcl_ret_t
rcl_get_steady_time(void * data, rcl_time_point_value_t * current_time)
{
  (void)data;  // unused
  return rcutils_steady_time_now(current_time);
}

// Implementation only
static rcl_ret_t
rcl_get_system_time(void * data, rcl_time_point_value_t * current_time)
{
  (void)data;  // unused
  return rcutils_system_time_now(current_time);
}

// Internal method for zeroing values on init, assumes clock is valid
static void
rcl_init_generic_clock(rcl_clock_t * clock, rcl_allocator_t * allocator)
{
  clock->type = RCL_CLOCK_UNINITIALIZED;
  clock->jump_callbacks = NULL;
  clock->num_jump_callbacks = 0u;
  clock->get_now = NULL;
  clock->data = NULL;
  clock->allocator = *allocator;
}

// The function used to get the current ros time.
// This is in the implementation only
static rcl_ret_t
rcl_get_ros_time(void * data, rcl_time_point_value_t * current_time)
{
  rcl_ros_clock_storage_t * t = (rcl_ros_clock_storage_t *)data;
  if (!t->active) {
    return rcl_get_system_time(data, current_time);
  }
  *current_time = rcutils_atomic_load_uint64_t(&(t->current_time));
  return RCL_RET_OK;
}

bool
rcl_clock_time_started(rcl_clock_t * clock)
{
  rcl_time_point_value_t query_now;
  if (rcl_clock_get_now(clock, &query_now) == RCL_RET_OK) {
    return query_now > 0;
  }
  return false;
}

bool
rcl_clock_valid(rcl_clock_t * clock)
{
  if (clock == NULL ||
    clock->type == RCL_CLOCK_UNINITIALIZED ||
    clock->get_now == NULL)
  {
    return false;
  }
  return true;
}

rcl_ret_t
rcl_clock_init(
  rcl_clock_type_t clock_type, rcl_clock_t * clock,
  rcl_allocator_t * allocator)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  switch (clock_type) {
    case RCL_CLOCK_UNINITIALIZED:
      RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
      rcl_init_generic_clock(clock, allocator);
      return RCL_RET_OK;
    case RCL_ROS_TIME:
      return rcl_ros_clock_init(clock, allocator);
    case RCL_SYSTEM_TIME:
      return rcl_system_clock_init(clock, allocator);
    case RCL_STEADY_TIME:
      return rcl_steady_clock_init(clock, allocator);
    default:
      return RCL_RET_INVALID_ARGUMENT;
  }
}

static void
rcl_clock_generic_fini(
  rcl_clock_t * clock)
{
  // Internal function; assume caller has already checked that clock is valid.
  if (clock->num_jump_callbacks > 0) {
    clock->num_jump_callbacks = 0;
    clock->allocator.deallocate(clock->jump_callbacks, clock->allocator.state);
    clock->jump_callbacks = NULL;
  }
}

rcl_ret_t
rcl_clock_fini(
  rcl_clock_t * clock)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(
    &clock->allocator, "clock has invalid allocator", return RCL_RET_ERROR);
  switch (clock->type) {
    case RCL_ROS_TIME:
      return rcl_ros_clock_fini(clock);
    case RCL_SYSTEM_TIME:
      return rcl_system_clock_fini(clock);
    case RCL_STEADY_TIME:
      return rcl_steady_clock_fini(clock);
    case RCL_CLOCK_UNINITIALIZED:
    // fall through
    default:
      return RCL_RET_INVALID_ARGUMENT;
  }
}

rcl_ret_t
rcl_ros_clock_init(
  rcl_clock_t * clock,
  rcl_allocator_t * allocator)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(allocator, RCL_RET_INVALID_ARGUMENT);
  rcl_init_generic_clock(clock, allocator);
  clock->data = allocator->allocate(sizeof(rcl_ros_clock_storage_t), allocator->state);
  if (NULL == clock->data) {
    RCL_SET_ERROR_MSG("allocating memory failed");
    return RCL_RET_BAD_ALLOC;
  }

  rcl_ros_clock_storage_t * storage = (rcl_ros_clock_storage_t *)clock->data;
  // 0 is a special value meaning time has not been set
  atomic_init(&(storage->current_time), 0);
  storage->active = false;
  clock->get_now = rcl_get_ros_time;
  clock->type = RCL_ROS_TIME;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_ros_clock_fini(
  rcl_clock_t * clock)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  if (clock->type != RCL_ROS_TIME) {
    RCL_SET_ERROR_MSG("clock not of type RCL_ROS_TIME");
    return RCL_RET_ERROR;
  }
  rcl_clock_generic_fini(clock);
  clock->allocator.deallocate(clock->data, clock->allocator.state);
  clock->data = NULL;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_steady_clock_init(
  rcl_clock_t * clock,
  rcl_allocator_t * allocator)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(allocator, RCL_RET_INVALID_ARGUMENT);
  rcl_init_generic_clock(clock, allocator);
  clock->get_now = rcl_get_steady_time;
  clock->type = RCL_STEADY_TIME;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_steady_clock_fini(
  rcl_clock_t * clock)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  if (clock->type != RCL_STEADY_TIME) {
    RCL_SET_ERROR_MSG("clock not of type RCL_STEADY_TIME");
    return RCL_RET_ERROR;
  }
  rcl_clock_generic_fini(clock);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_system_clock_init(
  rcl_clock_t * clock,
  rcl_allocator_t * allocator)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(allocator, RCL_RET_INVALID_ARGUMENT);
  rcl_init_generic_clock(clock, allocator);
  clock->get_now = rcl_get_system_time;
  clock->type = RCL_SYSTEM_TIME;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_system_clock_fini(
  rcl_clock_t * clock)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  if (clock->type != RCL_SYSTEM_TIME) {
    RCL_SET_ERROR_MSG("clock not of type RCL_SYSTEM_TIME");
    return RCL_RET_ERROR;
  }
  rcl_clock_generic_fini(clock);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_difference_times(
  const rcl_time_point_t * start, const rcl_time_point_t * finish, rcl_duration_t * delta)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(start, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(finish, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(delta, RCL_RET_INVALID_ARGUMENT);
  if (start->clock_type != finish->clock_type) {
    RCL_SET_ERROR_MSG("Cannot difference between time points with clocks types.");
    return RCL_RET_ERROR;
  }
  if (finish->nanoseconds < start->nanoseconds) {
    rcl_time_point_value_t intermediate = start->nanoseconds - finish->nanoseconds;
    delta->nanoseconds = -1 * (int64_t) intermediate;
  } else {
    delta->nanoseconds = (int64_t)(finish->nanoseconds - start->nanoseconds);
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_clock_get_now(rcl_clock_t * clock, rcl_time_point_value_t * time_point_value)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(time_point_value, RCL_RET_INVALID_ARGUMENT);
  if (clock->type && clock->get_now) {
    return clock->get_now(clock->data, time_point_value);
  }
  RCL_SET_ERROR_MSG("Clock is not initialized or does not have get_now registered.");
  return RCL_RET_ERROR;
}

static void
rcl_clock_call_callbacks(
  rcl_clock_t * clock, const rcl_time_jump_t * time_jump, bool before_jump)
{
  // Internal function; assume parameters are valid.
  bool is_clock_change = time_jump->clock_change == RCL_ROS_TIME_ACTIVATED ||
    time_jump->clock_change == RCL_ROS_TIME_DEACTIVATED;
  for (size_t cb_idx = 0; cb_idx < clock->num_jump_callbacks; ++cb_idx) {
    rcl_jump_callback_info_t * info = &(clock->jump_callbacks[cb_idx]);
    if (
      (is_clock_change && info->threshold.on_clock_change) ||
      (info->threshold.min_backward.nanoseconds < 0 &&
      time_jump->delta.nanoseconds <= info->threshold.min_backward.nanoseconds) ||
      (info->threshold.min_forward.nanoseconds > 0 &&
      time_jump->delta.nanoseconds >= info->threshold.min_forward.nanoseconds))
    {
      info->callback(time_jump, before_jump, info->user_data);
    }
  }
}

rcl_ret_t
rcl_enable_ros_time_override(rcl_clock_t * clock)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  if (clock->type != RCL_ROS_TIME) {
    RCL_SET_ERROR_MSG("Clock is not of type RCL_ROS_TIME, cannot enable override.");
    return RCL_RET_ERROR;
  }
  rcl_ros_clock_storage_t * storage = (rcl_ros_clock_storage_t *)clock->data;
  RCL_CHECK_FOR_NULL_WITH_MSG(
    storage, "Clock storage is not initialized, cannot enable override.", return RCL_RET_ERROR);
  if (!storage->active) {
    rcl_time_jump_t time_jump;
    time_jump.delta.nanoseconds = 0;
    time_jump.clock_change = RCL_ROS_TIME_ACTIVATED;
    rcl_clock_call_callbacks(clock, &time_jump, true);
    storage->active = true;
    rcl_clock_call_callbacks(clock, &time_jump, false);
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_disable_ros_time_override(rcl_clock_t * clock)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  if (clock->type != RCL_ROS_TIME) {
    RCL_SET_ERROR_MSG("Clock is not of type RCL_ROS_TIME, cannot disable override.");
    return RCL_RET_ERROR;
  }
  rcl_ros_clock_storage_t * storage = (rcl_ros_clock_storage_t *)clock->data;
  RCL_CHECK_FOR_NULL_WITH_MSG(
    storage, "Clock storage is not initialized, cannot enable override.", return RCL_RET_ERROR);
  if (storage->active) {
    rcl_time_jump_t time_jump;
    time_jump.delta.nanoseconds = 0;
    time_jump.clock_change = RCL_ROS_TIME_DEACTIVATED;
    rcl_clock_call_callbacks(clock, &time_jump, true);
    storage->active = false;
    rcl_clock_call_callbacks(clock, &time_jump, false);
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_is_enabled_ros_time_override(
  rcl_clock_t * clock,
  bool * is_enabled)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(is_enabled, RCL_RET_INVALID_ARGUMENT);
  if (clock->type != RCL_ROS_TIME) {
    RCL_SET_ERROR_MSG("Clock is not of type RCL_ROS_TIME, cannot query override state.");
    return RCL_RET_ERROR;
  }
  rcl_ros_clock_storage_t * storage = (rcl_ros_clock_storage_t *)clock->data;
  RCL_CHECK_FOR_NULL_WITH_MSG(
    storage, "Clock storage is not initialized, cannot enable override.", return RCL_RET_ERROR);
  *is_enabled = storage->active;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_set_ros_time_override(
  rcl_clock_t * clock,
  rcl_time_point_value_t time_value)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  if (clock->type != RCL_ROS_TIME) {
    RCL_SET_ERROR_MSG("Clock is not of type RCL_ROS_TIME, cannot set time override.");
    return RCL_RET_ERROR;
  }
  rcl_ros_clock_storage_t * storage = (rcl_ros_clock_storage_t *)clock->data;
  RCL_CHECK_FOR_NULL_WITH_MSG(
    storage, "Clock storage is not initialized, cannot enable override.", return RCL_RET_ERROR);
  rcl_time_jump_t time_jump;
  if (storage->active) {
    time_jump.clock_change = RCL_ROS_TIME_NO_CHANGE;
    rcl_time_point_value_t current_time;
    rcl_ret_t ret = rcl_get_ros_time(storage, &current_time);
    if (RCL_RET_OK != ret) {
      return ret;
    }
    time_jump.delta.nanoseconds = time_value - current_time;
    rcl_clock_call_callbacks(clock, &time_jump, true);
    rcutils_atomic_store(&(storage->current_time), time_value);
    rcl_clock_call_callbacks(clock, &time_jump, false);
  } else {
    rcutils_atomic_store(&(storage->current_time), time_value);
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_clock_add_jump_callback(
  rcl_clock_t * clock, rcl_jump_threshold_t threshold, rcl_jump_callback_t callback,
  void * user_data)
{
  // Make sure parameters are valid
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(
    &(clock->allocator), "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(callback, RCL_RET_INVALID_ARGUMENT);
  if (threshold.min_forward.nanoseconds < 0) {
    RCL_SET_ERROR_MSG("forward jump threshold must be positive or zero");
    return RCL_RET_INVALID_ARGUMENT;
  }
  if (threshold.min_backward.nanoseconds > 0) {
    RCL_SET_ERROR_MSG("backward jump threshold must be negative or zero");
    return RCL_RET_INVALID_ARGUMENT;
  }

  // Callback/user_data pair must be unique
  for (size_t cb_idx = 0; cb_idx < clock->num_jump_callbacks; ++cb_idx) {
    const rcl_jump_callback_info_t * info = &(clock->jump_callbacks[cb_idx]);
    if (info->callback == callback && info->user_data == user_data) {
      RCL_SET_ERROR_MSG("callback/user_data are already added to this clock");
      return RCL_RET_ERROR;
    }
  }

  // Add the new callback, increasing the size of the callback list
  rcl_jump_callback_info_t * callbacks = clock->allocator.reallocate(
    clock->jump_callbacks, sizeof(rcl_jump_callback_info_t) * (clock->num_jump_callbacks + 1),
    clock->allocator.state);
  if (NULL == callbacks) {
    RCL_SET_ERROR_MSG("Failed to realloc jump callbacks");
    return RCL_RET_BAD_ALLOC;
  }
  clock->jump_callbacks = callbacks;
  clock->jump_callbacks[clock->num_jump_callbacks].callback = callback;
  clock->jump_callbacks[clock->num_jump_callbacks].threshold = threshold;
  clock->jump_callbacks[clock->num_jump_callbacks].user_data = user_data;
  ++(clock->num_jump_callbacks);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_clock_remove_jump_callback(
  rcl_clock_t * clock, rcl_jump_callback_t callback, void * user_data)
{
  // Make sure parameters are valid
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(
    &(clock->allocator), "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(callback, RCL_RET_INVALID_ARGUMENT);

  // Delete callback if found, moving all callbacks after back one
  bool found_callback = false;
  for (size_t cb_idx = 0; cb_idx < clock->num_jump_callbacks; ++cb_idx) {
    const rcl_jump_callback_info_t * info = &(clock->jump_callbacks[cb_idx]);
    if (found_callback) {
      clock->jump_callbacks[cb_idx - 1] = *info;
    } else if (info->callback == callback && info->user_data == user_data) {
      found_callback = true;
    }
  }
  if (!found_callback) {
    RCL_SET_ERROR_MSG("jump callback was not found");
    return RCL_RET_ERROR;
  }

  // Shrink size of the callback array
  if (--(clock->num_jump_callbacks) == 0) {
    clock->allocator.deallocate(clock->jump_callbacks, clock->allocator.state);
    clock->jump_callbacks = NULL;
  } else {
    rcl_jump_callback_info_t * callbacks = clock->allocator.reallocate(
      clock->jump_callbacks, sizeof(rcl_jump_callback_info_t) * clock->num_jump_callbacks,
      clock->allocator.state);
    if (NULL == callbacks) {
      RCL_SET_ERROR_MSG("Failed to shrink jump callbacks");
      return RCL_RET_BAD_ALLOC;
    }
    clock->jump_callbacks = callbacks;
  }
  return RCL_RET_OK;
}
