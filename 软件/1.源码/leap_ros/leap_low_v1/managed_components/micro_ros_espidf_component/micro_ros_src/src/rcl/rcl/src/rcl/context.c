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

#include "rcl/context.h"

#include <stdbool.h>

#include "./common.h"
#include "./context_impl.h"
#include "rcutils/stdatomic_helper.h"

rcl_context_t
rcl_get_zero_initialized_context(void)
{
  static rcl_context_t context = {
    .impl = NULL,
    .instance_id_storage = 0,
  };
  // this is not constexpr so it cannot be in the struct initialization
#ifdef RCL_COMMAND_LINE_ENABLED
  context.global_arguments = rcl_get_zero_initialized_arguments();
#endif // RCL_COMMAND_LINE_ENABLED
  return context;
}

// See `rcl_init()` for initialization of the context.

rcl_ret_t
rcl_context_fini(rcl_context_t * context)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(context, RCL_RET_INVALID_ARGUMENT);
  if (!context->impl) {
    // Context is zero-initialized
    return RCL_RET_OK;
  }
  if (rcl_context_is_valid(context)) {
    RCL_SET_ERROR_MSG("rcl_shutdown() not called on the given context");
    return RCL_RET_INVALID_ARGUMENT;
  }
  RCL_CHECK_ALLOCATOR_WITH_MSG(
    &(context->impl->allocator), "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  return __cleanup_context(context);
}

// See `rcl_shutdown()` for invalidation of the context.

const rcl_init_options_t *
rcl_context_get_init_options(const rcl_context_t * context)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(context, NULL);
  RCL_CHECK_FOR_NULL_WITH_MSG(context->impl, "context is zero-initialized", return NULL);
  return &(context->impl->init_options);
}

rcl_context_instance_id_t
rcl_context_get_instance_id(const rcl_context_t * context)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(context, 0);
  return context->instance_id_storage;
}

rcl_ret_t
rcl_context_get_domain_id(rcl_context_t * context, size_t * domain_id)
{
  if (!rcl_context_is_valid(context)) {
    return RCL_RET_INVALID_ARGUMENT;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(domain_id, RCL_RET_INVALID_ARGUMENT);
  *domain_id = context->impl->rmw_context.actual_domain_id;
  return RCL_RET_OK;
}

bool
rcl_context_is_valid(const rcl_context_t * context)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(context, false);
  return 0 != rcl_context_get_instance_id(context);
}

rmw_context_t *
rcl_context_get_rmw_context(rcl_context_t * context)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(context, NULL);
  RCL_CHECK_FOR_NULL_WITH_MSG(context->impl, "context is zero-initialized", return NULL);
  return &(context->impl->rmw_context);
}

rcl_ret_t
__cleanup_context(rcl_context_t * context)
{
  rcl_ret_t ret = RCL_RET_OK;
  // reset the instance id to 0 to indicate "invalid" (should already be 0, but this is defensive)
  context->instance_id_storage = 0;

#ifdef RCL_COMMAND_LINE_ENABLED
  // clean up global_arguments if initialized
  if (NULL != context->global_arguments.impl) {
    ret = rcl_arguments_fini(&(context->global_arguments));
    if (RCL_RET_OK != ret) {
      RCUTILS_SAFE_FWRITE_TO_STDERR(
        "[rcl|context.c:" RCUTILS_STRINGIFY(__LINE__)
        "] failed to finalize global arguments while cleaning up context, memory may be leaked: ");
      RCUTILS_SAFE_FWRITE_TO_STDERR(rcl_get_error_string().str);
      RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
      rcl_reset_error();
    }
  }
#endif // RCL_COMMAND_LINE_ENABLED

  // if impl is null, nothing else can be cleaned up
  if (NULL != context->impl) {
    // pull allocator out for use during deallocation
    rcl_allocator_t allocator = context->impl->allocator;

    // finalize init options if valid
    if (NULL != context->impl->init_options.impl) {
      rcl_ret_t init_options_fini_ret = rcl_init_options_fini(&(context->impl->init_options));
      if (RCL_RET_OK != init_options_fini_ret) {
        if (RCL_RET_OK == ret) {
          ret = init_options_fini_ret;
        }
        RCUTILS_SAFE_FWRITE_TO_STDERR(
          "[rcl|context.c:" RCUTILS_STRINGIFY(__LINE__)
          "] failed to finalize init options while cleaning up context, memory may be leaked: ");
        RCUTILS_SAFE_FWRITE_TO_STDERR(rcl_get_error_string().str);
        RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
        rcl_reset_error();
      }
    }

    // clean up rmw_context
    if (NULL != context->impl->rmw_context.implementation_identifier) {
      rmw_ret_t rmw_context_fini_ret = rmw_context_fini(&(context->impl->rmw_context));
      if (RMW_RET_OK != rmw_context_fini_ret) {
        if (RCL_RET_OK == ret) {
          ret = rcl_convert_rmw_ret_to_rcl_ret(rmw_context_fini_ret);
        }
        RCUTILS_SAFE_FWRITE_TO_STDERR(
          "[rcl|context.c:" RCUTILS_STRINGIFY(__LINE__)
          "] failed to finalize rmw context while cleaning up context, memory may be leaked: ");
        RCUTILS_SAFE_FWRITE_TO_STDERR(rcutils_get_error_string().str);
        RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
        rcutils_reset_error();
      }
    }

    // clean up copy of argv if valid
    if (NULL != context->impl->argv) {
      int64_t i;
      for (i = 0; i < context->impl->argc; ++i) {
        if (NULL != context->impl->argv[i]) {
          allocator.deallocate(context->impl->argv[i], allocator.state);
        }
      }
      allocator.deallocate(context->impl->argv, allocator.state);
    }
    allocator.deallocate(context->impl, allocator.state);
  }  // if (NULL != context->impl)

  // zero-initialize the context
  *context = rcl_get_zero_initialized_context();

  return ret;
}

#ifdef __cplusplus
}
#endif
