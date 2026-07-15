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

#include <stddef.h>

#include "rcutils/strdup.h"

#include "rmw/error_handling.h"
#include "rmw/security_options.h"

rmw_security_options_t
rmw_get_zero_initialized_security_options()
{
  rmw_security_options_t zero_initialized_options = {0, NULL};
  return zero_initialized_options;
}

rmw_security_options_t
rmw_get_default_security_options()
{
  rmw_security_options_t default_options = {
    RMW_SECURITY_ENFORCEMENT_PERMISSIVE,
    NULL,
  };
  return default_options;
}

rmw_ret_t
rmw_security_options_copy(
  const rmw_security_options_t * src,
  const rcutils_allocator_t * allocator,
  rmw_security_options_t * dst)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(src, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(dst, RMW_RET_INVALID_ARGUMENT);

  char * new_root_path = rcutils_strdup(src->security_root_path, *allocator);
  if (src->security_root_path && !new_root_path) {
    RMW_SET_ERROR_MSG("failed to copy security root path");
    return RMW_RET_BAD_ALLOC;
  }
  allocator->deallocate(dst->security_root_path, allocator->state);
  dst->security_root_path = new_root_path;
  dst->enforce_security = src->enforce_security;
  return RMW_RET_OK;
}

rmw_ret_t
rmw_security_options_set_root_path(
  const char * security_root_path,
  const rcutils_allocator_t * allocator,
  rmw_security_options_t * security_options)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(security_root_path, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(security_options, RMW_RET_INVALID_ARGUMENT);
  char * new_root_path = rcutils_strdup(security_root_path, *allocator);
  if (!new_root_path) {
    RMW_SET_ERROR_MSG("failed to copy security root path");
    return RMW_RET_BAD_ALLOC;
  }
  allocator->deallocate(security_options->security_root_path, allocator->state);
  security_options->security_root_path = new_root_path;
  return RMW_RET_OK;
}

rmw_ret_t
rmw_security_options_fini(
  rmw_security_options_t * security_options,
  const rcutils_allocator_t * allocator)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(security_options, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);
  allocator->deallocate(security_options->security_root_path, allocator->state);
  *security_options = rmw_get_zero_initialized_security_options();
  return RMW_RET_OK;
}
