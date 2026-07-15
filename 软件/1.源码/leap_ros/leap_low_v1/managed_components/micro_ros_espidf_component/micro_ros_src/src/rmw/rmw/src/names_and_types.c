// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#include "rmw/names_and_types.h"

#include "rcutils/logging_macros.h"
#include "rcutils/macros.h"
#include "rcutils/types/string_array.h"
#include "rmw/error_handling.h"
#include "rmw/convert_rcutils_ret_to_rmw_ret.h"
#include "rmw/types.h"

rmw_names_and_types_t
rmw_get_zero_initialized_names_and_types(void)
{
  static rmw_names_and_types_t zero = {
    .names = {0, NULL, {NULL, NULL, NULL, NULL, NULL}},
    .types = NULL,
  };
  zero.names = rcutils_get_zero_initialized_string_array();
  return zero;
}

rmw_ret_t
rmw_names_and_types_check_zero(rmw_names_and_types_t * names_and_types)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);

  if (!names_and_types) {
    RMW_SET_ERROR_MSG("names_and_types is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (names_and_types->names.data != 0 || names_and_types->names.size != 0) {
    RMW_SET_ERROR_MSG("names array is not zeroed");
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (names_and_types->types != 0) {
    RMW_SET_ERROR_MSG("types array is not NULL");
    return RMW_RET_INVALID_ARGUMENT;
  }
  return RMW_RET_OK;
}

rmw_ret_t
rmw_names_and_types_init(
  rmw_names_and_types_t * names_and_types,
  size_t size,
  rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_BAD_ALLOC);

  if (!allocator) {
    RMW_SET_ERROR_MSG("allocator is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (!names_and_types) {
    RMW_SET_ERROR_MSG("names_and_types is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  rcutils_ret_t rcutils_ret = rcutils_string_array_init(&names_and_types->names, size, allocator);
  if (rcutils_ret != RCUTILS_RET_OK) {
    RMW_SET_ERROR_MSG(rcutils_get_error_string().str);
    return rmw_convert_rcutils_ret_to_rmw_ret(rcutils_ret);
  }
  names_and_types->types =
    allocator->zero_allocate(size, sizeof(rcutils_string_array_t), allocator->state);
  if (!names_and_types->types && size != 0) {
    rcutils_ret = rcutils_string_array_fini(&names_and_types->names);
    if (rcutils_ret != RCUTILS_RET_OK) {
      RCUTILS_LOG_ERROR("error while reporting error: %s", rcutils_get_error_string().str);
    }
    RMW_SET_ERROR_MSG("failed to allocate memory for types");
    return RMW_RET_BAD_ALLOC;
  }
  return RMW_RET_OK;
}

rmw_ret_t
rmw_names_and_types_fini(rmw_names_and_types_t * names_and_types)
{
  if (!names_and_types) {
    RMW_SET_ERROR_MSG("names_and_types is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  rcutils_ret_t rcutils_ret;
  if (names_and_types->types) {
    RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
      &names_and_types->names.allocator,
      "allocator is invalid",
      return RMW_RET_INVALID_ARGUMENT);

    // Cleanup string arrays for types first
    size_t i;
    for (i = 0; i < names_and_types->names.size; ++i) {
      rcutils_ret = rcutils_string_array_fini(&names_and_types->types[i]);
      if (rcutils_ret != RCUTILS_RET_OK) {
        RMW_SET_ERROR_MSG(rcutils_get_error_string().str);
        return rmw_convert_rcutils_ret_to_rmw_ret(rcutils_ret);
      }
    }

    // Use the allocator in the names string array
    // (prevents this data structure from having to also store it)
    names_and_types->names.allocator.deallocate(
      names_and_types->types, names_and_types->names.allocator.state);
    names_and_types->types = NULL;
  }
  // Cleanup names string array
  rcutils_ret = rcutils_string_array_fini(&names_and_types->names);
  if (rcutils_ret != RCUTILS_RET_OK) {
    RMW_SET_ERROR_MSG(rcutils_get_error_string().str);
    return rmw_convert_rcutils_ret_to_rmw_ret(rcutils_ret);
  }
  return RMW_RET_OK;
}
