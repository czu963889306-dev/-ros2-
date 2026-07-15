// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include "rcl/log_level.h"

#include "rcl/error_handling.h"
#include "rcutils/allocator.h"
#include "rcutils/logging_macros.h"
#include "rcutils/strdup.h"

rcl_log_levels_t
rcl_get_zero_initialized_log_levels()
{
  const rcl_log_levels_t log_levels = {
    .default_logger_level = RCUTILS_LOG_SEVERITY_UNSET,
    .logger_settings = NULL,
    .num_logger_settings = 0,
    .capacity_logger_settings = 0,
    .allocator = {NULL, NULL, NULL, NULL, NULL},
  };
  return log_levels;
}

rcl_ret_t
rcl_log_levels_init(
  rcl_log_levels_t * log_levels, const rcl_allocator_t * allocator, size_t logger_count)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(log_levels, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  if (log_levels->logger_settings != NULL) {
    RCL_SET_ERROR_MSG("invalid logger settings");
    return RCL_RET_INVALID_ARGUMENT;
  }

  log_levels->default_logger_level = RCUTILS_LOG_SEVERITY_UNSET;
  log_levels->logger_settings = NULL;
  log_levels->num_logger_settings = 0;
  log_levels->capacity_logger_settings = logger_count;
  log_levels->allocator = *allocator;

  if (logger_count > 0) {
    log_levels->logger_settings = allocator->allocate(
      sizeof(rcl_logger_setting_t) * logger_count, allocator->state);
    if (NULL == log_levels->logger_settings) {
      RCL_SET_ERROR_MSG("Error allocating memory");
      return RCL_RET_BAD_ALLOC;
    }
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_log_levels_copy(const rcl_log_levels_t * src, rcl_log_levels_t * dst)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(src, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(dst, RCL_RET_INVALID_ARGUMENT);
  const rcl_allocator_t * allocator = &src->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  if (dst->logger_settings != NULL) {
    RCL_SET_ERROR_MSG("invalid logger settings");
    return RCL_RET_INVALID_ARGUMENT;
  }

  dst->logger_settings = allocator->allocate(
    sizeof(rcl_logger_setting_t) * (src->num_logger_settings), allocator->state);
  if (NULL == dst->logger_settings) {
    RCL_SET_ERROR_MSG("Error allocating memory");
    return RCL_RET_BAD_ALLOC;
  }

  dst->default_logger_level = src->default_logger_level;
  dst->capacity_logger_settings = src->capacity_logger_settings;
  dst->allocator = src->allocator;
  for (size_t i = 0; i < src->num_logger_settings; ++i) {
    dst->logger_settings[i].name =
      rcutils_strdup(src->logger_settings[i].name, *allocator);
    if (NULL == dst->logger_settings[i].name) {
      dst->num_logger_settings = i;
      if (RCL_RET_OK != rcl_log_levels_fini(dst)) {
        RCL_SET_ERROR_MSG("Error while finalizing log levels due to another error");
      }
      return RCL_RET_BAD_ALLOC;
    }
    dst->logger_settings[i].level = src->logger_settings[i].level;
  }
  dst->num_logger_settings = src->num_logger_settings;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_log_levels_fini(rcl_log_levels_t * log_levels)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(log_levels, RCL_RET_INVALID_ARGUMENT);
  const rcl_allocator_t * allocator = &log_levels->allocator;
  if (log_levels->logger_settings) {
    // check allocator here, so it's safe to finish a zero initialized rcl_log_levels_t
    RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
    for (size_t i = 0; i < log_levels->num_logger_settings; ++i) {
      allocator->deallocate((void *)log_levels->logger_settings[i].name, allocator->state);
    }
    log_levels->num_logger_settings = 0;

    allocator->deallocate(log_levels->logger_settings, allocator->state);
    log_levels->logger_settings = NULL;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_log_levels_shrink_to_size(rcl_log_levels_t * log_levels)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(log_levels, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = &log_levels->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  if (0U == log_levels->num_logger_settings) {
    allocator->deallocate(log_levels->logger_settings, allocator->state);
    log_levels->logger_settings = NULL;
    log_levels->capacity_logger_settings = 0;
  } else if (log_levels->num_logger_settings < log_levels->capacity_logger_settings) {
    rcl_logger_setting_t * new_logger_settings = allocator->reallocate(
      log_levels->logger_settings,
      sizeof(rcl_logger_setting_t) * log_levels->num_logger_settings,
      allocator->state);
    if (NULL == new_logger_settings) {
      return RCL_RET_BAD_ALLOC;
    }
    log_levels->logger_settings = new_logger_settings;
    log_levels->capacity_logger_settings = log_levels->num_logger_settings;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_log_levels_add_logger_setting(
  rcl_log_levels_t * log_levels, const char * logger_name, rcl_log_severity_t log_level)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(log_levels, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(log_levels->logger_settings, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(logger_name, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = &log_levels->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  // check if there exists a logger with the same name
  rcl_logger_setting_t * logger_setting = NULL;
  for (size_t i = 0; i < log_levels->num_logger_settings; ++i) {
    if (strcmp(log_levels->logger_settings[i].name, logger_name) == 0) {
      logger_setting = &log_levels->logger_settings[i];
      if (logger_setting->level != log_level) {
        RCUTILS_LOG_DEBUG_NAMED(
          ROS_PACKAGE_NAME, "Minimum log level of logger [%s] will be replaced from %d to %d",
          logger_name, logger_setting->level, log_level);
        logger_setting->level = log_level;
      }
      return RCL_RET_OK;
    }
  }

  if (log_levels->num_logger_settings >= log_levels->capacity_logger_settings) {
    RCL_SET_ERROR_MSG("No capacity to store a logger setting");
    return RCL_RET_ERROR;
  }

  char * name = rcutils_strdup(logger_name, *allocator);
  if (NULL == name) {
    RCL_SET_ERROR_MSG("failed to copy logger name");
    return RCL_RET_BAD_ALLOC;
  }

  logger_setting = &log_levels->logger_settings[log_levels->num_logger_settings];
  logger_setting->name = name;
  logger_setting->level = log_level;
  log_levels->num_logger_settings += 1;
  return RCL_RET_OK;
}
