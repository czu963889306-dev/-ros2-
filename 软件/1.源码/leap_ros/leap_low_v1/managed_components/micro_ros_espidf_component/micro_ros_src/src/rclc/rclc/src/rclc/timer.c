// Copyright (c) 2019 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
// Copyright 2014 Open Source Robotics Foundation, Inc.
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

#include "rclc/timer.h"

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>

rcl_ret_t
rclc_timer_init_default(
  rcl_timer_t * timer,
  rclc_support_t * support,
  const uint64_t timeout_ns,
  const rcl_timer_callback_t callback)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    timer, "timer is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    support, "support is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  (*timer) = rcl_get_zero_initialized_timer();
  rcl_ret_t rc = rcl_timer_init(
    timer,
    &support->clock,
    &support->context,
    timeout_ns,
    callback,
    (*support->allocator));
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_timer_init_default, rcl_timer_init);
  } else {
    RCUTILS_LOG_INFO("Created a timer with period %ld ms.\n", timeout_ns / 1000000);
  }
  return rc;
}
