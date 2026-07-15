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

#include "rclc/init.h"

#include <rcl/context.h>
#include <rcl/error_handling.h>
#include <rcl/init.h>
#include <rcl/init_options.h>
#include <rcutils/logging_macros.h>

rcl_ret_t
rclc_support_init(
  rclc_support_t * support,
  int argc,
  char const * const * argv,
  rcl_allocator_t * allocator)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    support, "support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    allocator, "allocator is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  rcl_ret_t rc = RCL_RET_OK;

  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rc = rcl_init_options_init(&init_options, (*allocator) );
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_support_init, rcl_init_options_init);
    return rc;
  }

  rc = rclc_support_init_with_options(support, argc, argv, &init_options, allocator);
  if (rcl_init_options_fini(&init_options) != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_support_init, rcl_init_options_fini);
  }

  return rc;
}

rcl_ret_t
rclc_support_init_with_options(
  rclc_support_t * support,
  int argc,
  char const * const * argv,
  rcl_init_options_t * init_options,
  rcl_allocator_t * allocator)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    support, "support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    init_options, "init_options is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    allocator, "allocator is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  rcl_ret_t rc = RCL_RET_OK;

  support->context = rcl_get_zero_initialized_context();
  rc = rcl_init(argc, argv, init_options, &support->context);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_init, rcl_init);
    return rc;
  }
  support->allocator = allocator;

  rc = rcl_clock_init(RCL_STEADY_TIME, &support->clock, support->allocator);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_init, rcl_clock_init);
  }
  return rc;
}

rcl_ret_t
rclc_support_fini(rclc_support_t * support)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    support, "support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  rcl_ret_t result = RCL_RET_OK;
  rcl_ret_t rc;
  rc = rcl_clock_fini(&support->clock);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_WARN(rclc_support_fini, rcl_clock_fini);
    result = RCL_RET_ERROR;
  }
  rc = rcl_shutdown(&support->context);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_WARN(rclc_support_fini, rcl_shutdown);
    result = RCL_RET_ERROR;
  }
  rc = rcl_context_fini(&support->context);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_WARN(rclc_support_fini, rcl_context_fini);
    result = RCL_RET_ERROR;
  }
  return result;
}
