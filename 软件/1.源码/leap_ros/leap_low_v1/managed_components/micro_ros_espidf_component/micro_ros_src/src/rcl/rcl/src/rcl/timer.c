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

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/timer.h"

#include <inttypes.h>

#include "rcl/error_handling.h"
#include "rcutils/logging_macros.h"
#include "rcutils/stdatomic_helper.h"
#include "rcutils/time.h"
#include "tracetools/tracetools.h"

struct rcl_timer_impl_s
{
  // The clock providing time.
  rcl_clock_t * clock;
  // The associated context.
  rcl_context_t * context;
  // A guard condition used to wake the associated wait set, either when
  // ROSTime causes the timer to expire or when the timer is reset.
  rcl_guard_condition_t guard_condition;
  // The user supplied callback.
  atomic_uintptr_t callback;
  // This is a duration in nanoseconds.
  atomic_uint_least64_t period;
  // This is a time in nanoseconds since an unspecified time.
  atomic_int_least64_t last_call_time;
  // This is a time in nanoseconds since an unspecified time.
  atomic_int_least64_t next_call_time;
  // Credit for time elapsed before ROS time is activated or deactivated.
  atomic_int_least64_t time_credit;
  // A flag which indicates if the timer is canceled.
  atomic_bool canceled;
  // The user supplied allocator.
  rcl_allocator_t allocator;
};

rcl_timer_t
rcl_get_zero_initialized_timer()
{
  static rcl_timer_t null_timer = {0};
  return null_timer;
}

void _rcl_timer_time_jump(
  const rcl_time_jump_t * time_jump,
  bool before_jump,
  void * user_data)
{
  rcl_timer_t * timer = (rcl_timer_t *)user_data;

  if (before_jump) {
    if (RCL_ROS_TIME_ACTIVATED == time_jump->clock_change ||
      RCL_ROS_TIME_DEACTIVATED == time_jump->clock_change)
    {
      rcl_time_point_value_t now;
      if (RCL_RET_OK != rcl_clock_get_now(timer->impl->clock, &now)) {
        RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to get current time in jump callback");
        return;
      }
      // Source of time is changing, but the timer has ellapsed some portion of its period
      // Save ellapsed duration pre jump so the timer only waits the remainder in the new epoch
      if (0 == now) {
        // No time credit if clock is uninitialized
        return;
      }
      const int64_t next_call_time = rcutils_atomic_load_int64_t(&timer->impl->next_call_time);
      rcutils_atomic_store(&timer->impl->time_credit, next_call_time - now);
    }
  } else {
    rcl_time_point_value_t now;
    if (RCL_RET_OK != rcl_clock_get_now(timer->impl->clock, &now)) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to get current time in jump callback");
      return;
    }
    const int64_t last_call_time = rcutils_atomic_load_int64_t(&timer->impl->last_call_time);
    const int64_t next_call_time = rcutils_atomic_load_int64_t(&timer->impl->next_call_time);
    const int64_t period = rcutils_atomic_load_uint64_t(&timer->impl->period);
    if (RCL_ROS_TIME_ACTIVATED == time_jump->clock_change ||
      RCL_ROS_TIME_DEACTIVATED == time_jump->clock_change)
    {
      // ROS time activated or deactivated
      if (0 == now) {
        // Can't apply time credit if clock is uninitialized
        return;
      }
      int64_t time_credit = rcutils_atomic_exchange_int64_t(&timer->impl->time_credit, 0);
      if (time_credit) {
        // set times in new epoch so timer only waits the remainder of the period
        rcutils_atomic_store(&timer->impl->next_call_time, now - time_credit + period);
        rcutils_atomic_store(&timer->impl->last_call_time, now - time_credit);
      }
    } else if (next_call_time <= now) {
      // Post Forward jump and timer is ready
      if (RCL_RET_OK != rcl_trigger_guard_condition(&timer->impl->guard_condition)) {
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME, "Failed to get trigger guard condition in jump callback");
      }
    } else if (now < last_call_time) {
      // Post backwards time jump that went further back than 1 period
      // next callback should happen after 1 period
      rcutils_atomic_store(&timer->impl->next_call_time, now + period);
      rcutils_atomic_store(&timer->impl->last_call_time, now);
      return;
    }
  }
}

rcl_ret_t
rcl_timer_init(
  rcl_timer_t * timer,
  rcl_clock_t * clock,
  rcl_context_t * context,
  int64_t period,
  const rcl_timer_callback_t callback,
  rcl_allocator_t allocator)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  if (period < 0) {
    RCL_SET_ERROR_MSG("timer period must be non-negative");
    return RCL_RET_INVALID_ARGUMENT;
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Initializing timer with period: %" PRIu64 "ns", period);
  if (timer->impl) {
    RCL_SET_ERROR_MSG("timer already initialized, or memory was uninitialized");
    return RCL_RET_ALREADY_INIT;
  }
  rcl_time_point_value_t now;
  rcl_ret_t now_ret = rcl_clock_get_now(clock, &now);
  if (now_ret != RCL_RET_OK) {
    return now_ret;  // rcl error state should already be set.
  }
  rcl_timer_impl_t impl;
  impl.clock = clock;
  impl.context = context;
  impl.guard_condition = rcl_get_zero_initialized_guard_condition();
  rcl_guard_condition_options_t options = rcl_guard_condition_get_default_options();
  rcl_ret_t ret = rcl_guard_condition_init(&(impl.guard_condition), context, options);
  if (RCL_RET_OK != ret) {
    return ret;
  }
  if (RCL_ROS_TIME == impl.clock->type) {
    rcl_jump_threshold_t threshold;
    threshold.on_clock_change = true;
    threshold.min_forward.nanoseconds = 1;
    threshold.min_backward.nanoseconds = -1;
    ret = rcl_clock_add_jump_callback(clock, threshold, _rcl_timer_time_jump, timer);
    if (RCL_RET_OK != ret) {
      if (RCL_RET_OK != rcl_guard_condition_fini(&(impl.guard_condition))) {
        // Should be impossible
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME, "Failed to fini guard condition after failing to add jump callback");
      }
      return ret;
    }
  }
  atomic_init(&impl.callback, (uintptr_t)callback);
  atomic_init(&impl.period, period);
  atomic_init(&impl.time_credit, 0);
  atomic_init(&impl.last_call_time, now);
  atomic_init(&impl.next_call_time, now + period);
  atomic_init(&impl.canceled, false);
  impl.allocator = allocator;
  timer->impl = (rcl_timer_impl_t *)allocator.allocate(sizeof(rcl_timer_impl_t), allocator.state);
  if (NULL == timer->impl) {
    if (RCL_RET_OK != rcl_guard_condition_fini(&(impl.guard_condition))) {
      // Should be impossible
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to fini guard condition after bad alloc");
    }
    if (RCL_RET_OK != rcl_clock_remove_jump_callback(clock, _rcl_timer_time_jump, timer)) {
      // Should be impossible
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to remove callback after bad alloc");
    }

    RCL_SET_ERROR_MSG("allocating memory failed");
    return RCL_RET_BAD_ALLOC;
  }
  *timer->impl = impl;
  TRACEPOINT(rcl_timer_init, (const void *)timer, period);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_fini(rcl_timer_t * timer)
{
  if (!timer || !timer->impl) {
    return RCL_RET_OK;
  }
  // Will return either RCL_RET_OK or RCL_RET_ERROR since the timer is valid.
  rcl_ret_t result = rcl_timer_cancel(timer);
  rcl_allocator_t allocator = timer->impl->allocator;
  rcl_ret_t fail_ret;
  if (RCL_ROS_TIME == timer->impl->clock->type) {
    // The jump callbacks use the guard condition, so we have to remove it
    // before freeing the guard condition below.
    fail_ret = rcl_clock_remove_jump_callback(timer->impl->clock, _rcl_timer_time_jump, timer);
    if (RCL_RET_OK != fail_ret) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to remove timer jump callback");
    }
  }
  fail_ret = rcl_guard_condition_fini(&(timer->impl->guard_condition));
  if (RCL_RET_OK != fail_ret) {
    RCL_SET_ERROR_MSG("Failure to fini guard condition");
  }
  allocator.deallocate(timer->impl, allocator.state);
  timer->impl = NULL;
  return result;
}

RCL_PUBLIC
RCL_WARN_UNUSED
rcl_ret_t
rcl_timer_clock(rcl_timer_t * timer, rcl_clock_t ** clock)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(clock, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer->impl, RCL_RET_TIMER_INVALID);
  *clock = timer->impl->clock;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_call(rcl_timer_t * timer)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Calling timer");
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer->impl, RCL_RET_TIMER_INVALID);
  if (rcutils_atomic_load_bool(&timer->impl->canceled)) {
    RCL_SET_ERROR_MSG("timer is canceled");
    return RCL_RET_TIMER_CANCELED;
  }
  rcl_time_point_value_t now;
  rcl_ret_t now_ret = rcl_clock_get_now(timer->impl->clock, &now);
  if (now_ret != RCL_RET_OK) {
    return now_ret;  // rcl error state should already be set.
  }
  if (now < 0) {
    RCL_SET_ERROR_MSG("clock now returned negative time point value");
    return RCL_RET_ERROR;
  }
  rcl_time_point_value_t previous_ns =
    rcutils_atomic_exchange_int64_t(&timer->impl->last_call_time, now);
  rcl_timer_callback_t typed_callback =
    (rcl_timer_callback_t)rcutils_atomic_load_uintptr_t(&timer->impl->callback);

  int64_t next_call_time = rcutils_atomic_load_int64_t(&timer->impl->next_call_time);
  int64_t period = rcutils_atomic_load_uint64_t(&timer->impl->period);
  // always move the next call time by exactly period forward
  // don't use now as the base to avoid extending each cycle by the time
  // between the timer being ready and the callback being triggered
  next_call_time += period;
  // in case the timer has missed at least once cycle
  if (next_call_time < now) {
    if (0 == period) {
      // a timer with a period of zero is considered always ready
      next_call_time = now;
    } else {
      // move the next call time forward by as many periods as necessary
      int64_t now_ahead = now - next_call_time;
      // rounding up without overflow
      int64_t periods_ahead = 1 + (now_ahead - 1) / period;
      next_call_time += periods_ahead * period;
    }
  }
  rcutils_atomic_store(&timer->impl->next_call_time, next_call_time);

  if (typed_callback != NULL) {
    int64_t since_last_call = now - previous_ns;
    typed_callback(timer, since_last_call);
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_is_ready(const rcl_timer_t * timer, bool * is_ready)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer->impl, RCL_RET_TIMER_INVALID);
  RCL_CHECK_ARGUMENT_FOR_NULL(is_ready, RCL_RET_INVALID_ARGUMENT);
  int64_t time_until_next_call;
  rcl_ret_t ret = rcl_timer_get_time_until_next_call(timer, &time_until_next_call);
  if (ret == RCL_RET_TIMER_CANCELED) {
    *is_ready = false;
    return RCL_RET_OK;
  } else if (ret != RCL_RET_OK) {
    return ret;  // rcl error state should already be set.
  }
  *is_ready = (time_until_next_call <= 0);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_get_time_until_next_call(const rcl_timer_t * timer, int64_t * time_until_next_call)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer->impl, RCL_RET_TIMER_INVALID);
  RCL_CHECK_ARGUMENT_FOR_NULL(time_until_next_call, RCL_RET_INVALID_ARGUMENT);
  if (rcutils_atomic_load_bool(&timer->impl->canceled)) {
    return RCL_RET_TIMER_CANCELED;
  }
  rcl_time_point_value_t now;
  rcl_ret_t ret = rcl_clock_get_now(timer->impl->clock, &now);
  if (ret != RCL_RET_OK) {
    return ret;  // rcl error state should already be set.
  }
  *time_until_next_call =
    rcutils_atomic_load_int64_t(&timer->impl->next_call_time) - now;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_get_time_since_last_call(
  const rcl_timer_t * timer,
  rcl_time_point_value_t * time_since_last_call)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer->impl, RCL_RET_TIMER_INVALID);
  RCL_CHECK_ARGUMENT_FOR_NULL(time_since_last_call, RCL_RET_INVALID_ARGUMENT);
  rcl_time_point_value_t now;
  rcl_ret_t ret = rcl_clock_get_now(timer->impl->clock, &now);
  if (ret != RCL_RET_OK) {
    return ret;  // rcl error state should already be set.
  }
  *time_since_last_call =
    now - rcutils_atomic_load_int64_t(&timer->impl->last_call_time);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_get_period(const rcl_timer_t * timer, int64_t * period)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer->impl, RCL_RET_TIMER_INVALID);
  RCL_CHECK_ARGUMENT_FOR_NULL(period, RCL_RET_INVALID_ARGUMENT);
  *period = rcutils_atomic_load_uint64_t(&timer->impl->period);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_exchange_period(const rcl_timer_t * timer, int64_t new_period, int64_t * old_period)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);

  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer->impl, RCL_RET_TIMER_INVALID);
  RCL_CHECK_ARGUMENT_FOR_NULL(old_period, RCL_RET_INVALID_ARGUMENT);
  *old_period = rcutils_atomic_exchange_uint64_t(&timer->impl->period, new_period);
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Updated timer period from '%" PRIu64 "ns' to '%" PRIu64 "ns'",
    *old_period, new_period);
  return RCL_RET_OK;
}

rcl_timer_callback_t
rcl_timer_get_callback(const rcl_timer_t * timer)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, NULL);
  RCL_CHECK_FOR_NULL_WITH_MSG(timer->impl, "timer is invalid", return NULL);
  return (rcl_timer_callback_t)rcutils_atomic_load_uintptr_t(&timer->impl->callback);
}

rcl_timer_callback_t
rcl_timer_exchange_callback(rcl_timer_t * timer, const rcl_timer_callback_t new_callback)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Updating timer callback");
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, NULL);
  RCL_CHECK_FOR_NULL_WITH_MSG(timer->impl, "timer is invalid", return NULL);
  return (rcl_timer_callback_t)rcutils_atomic_exchange_uintptr_t(
    &timer->impl->callback, (uintptr_t)new_callback);
}

rcl_ret_t
rcl_timer_cancel(rcl_timer_t * timer)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_TIMER_INVALID);

  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(timer->impl, "timer is invalid", return RCL_RET_TIMER_INVALID);
  rcutils_atomic_store(&timer->impl->canceled, true);
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Timer canceled");
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_is_canceled(const rcl_timer_t * timer, bool * is_canceled)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(timer->impl, RCL_RET_TIMER_INVALID);
  RCL_CHECK_ARGUMENT_FOR_NULL(is_canceled, RCL_RET_INVALID_ARGUMENT);
  *is_canceled = rcutils_atomic_load_bool(&timer->impl->canceled);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_timer_reset(rcl_timer_t * timer)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);

  RCL_CHECK_ARGUMENT_FOR_NULL(timer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(timer->impl, "timer is invalid", return RCL_RET_TIMER_INVALID);
  rcl_time_point_value_t now;
  rcl_ret_t now_ret = rcl_clock_get_now(timer->impl->clock, &now);
  if (now_ret != RCL_RET_OK) {
    return now_ret;  // rcl error state should already be set.
  }
  int64_t period = rcutils_atomic_load_uint64_t(&timer->impl->period);
  rcutils_atomic_store(&timer->impl->next_call_time, now + period);
  rcutils_atomic_store(&timer->impl->canceled, false);
  rcl_ret_t ret = rcl_trigger_guard_condition(&timer->impl->guard_condition);
  if (ret != RCL_RET_OK) {
    RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to trigger timer guard condition");
  }
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Timer successfully reset");
  return RCL_RET_OK;
}

const rcl_allocator_t *
rcl_timer_get_allocator(const rcl_timer_t * timer)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(timer, NULL);
  RCL_CHECK_FOR_NULL_WITH_MSG(timer->impl, "timer is invalid", return NULL);
  return &timer->impl->allocator;
}

rcl_guard_condition_t *
rcl_timer_get_guard_condition(const rcl_timer_t * timer)
{
  if (NULL == timer || NULL == timer->impl || NULL == timer->impl->guard_condition.impl) {
    return NULL;
  }
  return &timer->impl->guard_condition;
}

#ifdef __cplusplus
}
#endif
