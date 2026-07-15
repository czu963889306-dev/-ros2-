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

#include "rcl/init_options.h"

#include "./common.h"
#include "./init_options_impl.h"
#include "rcutils/macros.h"
#include "rcl/error_handling.h"
#include "rmw/error_handling.h"
#include "rcutils/logging_macros.h"

rcl_init_options_t
rcl_get_zero_initialized_init_options(void)
{
  return (const rcl_init_options_t) {
           .impl = 0,
  };  // NOLINT(readability/braces): false positive
}

/// Initialize given init_options with the default values and zero-initialize implementation.
static inline
rcl_ret_t
_rcl_init_options_zero_init(rcl_init_options_t * init_options, rcl_allocator_t allocator)
{
  init_options->impl = allocator.allocate(sizeof(rcl_init_options_impl_t), allocator.state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    init_options->impl,
    "failed to allocate memory for init options impl",
    return RCL_RET_BAD_ALLOC);
  init_options->impl->allocator = allocator;
  init_options->impl->rmw_init_options = rmw_get_zero_initialized_init_options();

  return RCL_RET_OK;
}

rcl_ret_t
rcl_init_options_init(rcl_init_options_t * init_options, rcl_allocator_t allocator)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ALREADY_INIT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCL_CHECK_ARGUMENT_FOR_NULL(init_options, RCL_RET_INVALID_ARGUMENT);
  if (NULL != init_options->impl) {
    RCL_SET_ERROR_MSG("given init_options (rcl_init_options_t) is already initialized");
    return RCL_RET_ALREADY_INIT;
  }
  RCL_CHECK_ALLOCATOR(&allocator, return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret = _rcl_init_options_zero_init(init_options, allocator);
  if (RCL_RET_OK != ret) {
    return ret;
  }
  rmw_ret_t rmw_ret = rmw_init_options_init(&(init_options->impl->rmw_init_options), allocator);
  if (RMW_RET_OK != rmw_ret) {
    allocator.deallocate(init_options->impl, allocator.state);
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_init_options_copy(const rcl_init_options_t * src, rcl_init_options_t * dst)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ALREADY_INIT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCL_CHECK_ARGUMENT_FOR_NULL(src, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(src->impl, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR(&src->impl->allocator, return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(dst, RCL_RET_INVALID_ARGUMENT);
  if (NULL != dst->impl) {
    RCL_SET_ERROR_MSG("given dst (rcl_init_options_t) is already initialized");
    return RCL_RET_ALREADY_INIT;
  }

  // initialize dst (since we know it's in a zero initialized state)
  rcl_ret_t ret = _rcl_init_options_zero_init(dst, src->impl->allocator);
  if (RCL_RET_OK != ret) {
    return ret;  // error already set
  }

  // copy src information into dst
  rmw_ret_t rmw_ret =
    rmw_init_options_copy(&(src->impl->rmw_init_options), &(dst->impl->rmw_init_options));
  if (RMW_RET_OK != rmw_ret) {
    rmw_error_string_t error_string = rmw_get_error_string();
    rmw_reset_error();
    ret = rcl_init_options_fini(dst);
    if (RCL_RET_OK != ret) {
      RCUTILS_LOG_ERROR_NAMED(
        "rcl",
        "failed to finalize dst rcl_init_options while handling failure to "
        "copy rmw_init_options, original ret '%d' and error: %s", rmw_ret, error_string.str);
      return ret;  // error already set
    }
    RCL_SET_ERROR_MSG(error_string.str);
    return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
  }

  return RCL_RET_OK;
}

rcl_ret_t
rcl_init_options_fini(rcl_init_options_t * init_options)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options->impl, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t allocator = init_options->impl->allocator;
  RCL_CHECK_ALLOCATOR(&allocator, return RCL_RET_INVALID_ARGUMENT);
  rmw_ret_t rmw_ret = rmw_init_options_fini(&(init_options->impl->rmw_init_options));
  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
  }
  allocator.deallocate(init_options->impl, allocator.state);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_init_options_get_domain_id(const rcl_init_options_t * init_options, size_t * domain_id)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options->impl, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(domain_id, RCL_RET_INVALID_ARGUMENT);
  *domain_id = init_options->impl->rmw_init_options.domain_id;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_init_options_set_domain_id(rcl_init_options_t * init_options, size_t domain_id)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options->impl, RCL_RET_INVALID_ARGUMENT);
  init_options->impl->rmw_init_options.domain_id = domain_id;
  return RCL_RET_OK;
}

rmw_init_options_t *
rcl_init_options_get_rmw_init_options(rcl_init_options_t * init_options)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options, NULL);
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options->impl, NULL);
  return &(init_options->impl->rmw_init_options);
}

const rcl_allocator_t *
rcl_init_options_get_allocator(const rcl_init_options_t * init_options)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options, NULL);
  RCL_CHECK_ARGUMENT_FOR_NULL(init_options->impl, NULL);
  return &(init_options->impl->allocator);
}

#ifdef __cplusplus
}
#endif
