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

#include "rcl/init.h"

#include "rcutils/logging_macros.h"
#include "rcutils/stdatomic_helper.h"
#include "rcutils/strdup.h"

#include "rmw/error_handling.h"

#include "tracetools/tracetools.h"

#ifdef RCL_COMMAND_LINE_ENABLED
#include "rcl/arguments.h"
#endif // RCL_COMMAND_LINE_ENABLED
#include "rcl/domain_id.h"
#include "rcl/error_handling.h"
#include "rcl/localhost.h"
#ifdef RCL_LOGGING_ENABLED
#include "rcl/logging.h"
#endif // RCL_LOGGING_ENABLED
#include "rcl/security.h"
#include "rcl/validate_enclave_name.h"

#ifdef RCL_COMMAND_LINE_ENABLED
#include "./arguments_impl.h"
#endif // RCL_COMMAND_LINE_ENABLED
#include "./common.h"
#include "./context_impl.h"
#include "./init_options_impl.h"

rcl_ret_t
rcl_init(
  int argc,
  char const * const * argv,
  const rcl_init_options_t * options,
  rcl_context_t * context)
{
  rcl_ret_t fail_ret = RCL_RET_ERROR;

  if (argc > 0) {
    RCL_CHECK_ARGUMENT_FOR_NULL(argv, RCL_RET_INVALID_ARGUMENT);
    for (int i = 0; i < argc; ++i) {
      RCL_CHECK_ARGUMENT_FOR_NULL(argv[i], RCL_RET_INVALID_ARGUMENT);
    }
  } else {
    if (NULL != argv) {
      RCL_SET_ERROR_MSG("argc is <= 0, but argv is not NULL");
      return RCL_RET_INVALID_ARGUMENT;
    }
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(options->impl, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t allocator = options->impl->allocator;
  RCL_CHECK_ALLOCATOR(&allocator, return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(context, RCL_RET_INVALID_ARGUMENT);

  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME,
    "Initializing ROS client library, for context at address: %p", (void *) context);

  // test expectation that given context is zero initialized
  if (NULL != context->impl) {
    // note that this can also occur when the given context is used before initialization
    // i.e. it is declared on the stack but never defined or zero initialized
    RCL_SET_ERROR_MSG("rcl_init called on an already initialized context");
    return RCL_RET_ALREADY_INIT;
  }

#ifdef RCL_COMMAND_LINE_ENABLED
  // Zero initialize global arguments.
  context->global_arguments = rcl_get_zero_initialized_arguments();
#endif // RCL_COMMAND_LINE_ENABLED

  // Setup impl for context.
  // use zero_allocate so the cleanup function will not try to clean up uninitialized parts later
  context->impl = allocator.zero_allocate(1, sizeof(rcl_context_impl_t), allocator.state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    context->impl, "failed to allocate memory for context impl", return RCL_RET_BAD_ALLOC);

  // Zero initialize rmw context first so its validity can by checked in cleanup.
  context->impl->rmw_context = rmw_get_zero_initialized_context();

  // Store the allocator.
  context->impl->allocator = allocator;

  // Copy the options into the context for future reference.
  rcl_ret_t ret = rcl_init_options_copy(options, &(context->impl->init_options));
  if (RCL_RET_OK != ret) {
    fail_ret = ret;  // error message already set
    goto fail;
  }

  // Copy the argc and argv into the context, if argc >= 0.
  context->impl->argc = argc;
  context->impl->argv = NULL;
  if (0 != argc && argv != NULL) {
    context->impl->argv = (char **)allocator.zero_allocate(argc, sizeof(char *), allocator.state);
    RCL_CHECK_FOR_NULL_WITH_MSG(
      context->impl->argv,
      "failed to allocate memory for argv",
      fail_ret = RCL_RET_BAD_ALLOC; goto fail);
    int64_t i;
    for (i = 0; i < argc; ++i) {
      size_t argv_i_length = strlen(argv[i]) + 1;
      context->impl->argv[i] = (char *)allocator.allocate(argv_i_length, allocator.state);
      RCL_CHECK_FOR_NULL_WITH_MSG(
        context->impl->argv[i],
        "failed to allocate memory for string entry in argv",
        fail_ret = RCL_RET_BAD_ALLOC; goto fail);
      memcpy(context->impl->argv[i], argv[i], argv_i_length);
    }
  }

#ifdef RCL_COMMAND_LINE_ENABLED
  // Parse the ROS specific arguments.
  ret = rcl_parse_arguments(argc, argv, allocator, &context->global_arguments);
  if (RCL_RET_OK != ret) {
    fail_ret = ret;
    RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to parse global arguments");
    goto fail;
  }
#endif // RCL_COMMAND_LINE_ENABLED

  // Set the instance id.
  static uint32_t next_instance_id = 0;
  next_instance_id++;
  if (0 == next_instance_id) {
    // Avoid invalid value on roll over
    next_instance_id++;
  }
  context->instance_id_storage = next_instance_id;
  context->impl->init_options.impl->rmw_init_options.instance_id = next_instance_id;

  size_t * domain_id = &context->impl->init_options.impl->rmw_init_options.domain_id;
  if (RCL_DEFAULT_DOMAIN_ID == *domain_id) {
    // Get actual domain id based on environment variable.
    ret = rcl_get_default_domain_id(domain_id);
    if (RCL_RET_OK != ret) {
      fail_ret = ret;
      goto fail;
    }
  }

  rmw_localhost_only_t * localhost_only =
    &context->impl->init_options.impl->rmw_init_options.localhost_only;
  if (RMW_LOCALHOST_ONLY_DEFAULT == *localhost_only) {
    // Get actual localhost_only value based on environment variable, if needed.
    ret = rcl_get_localhost_only(localhost_only);
    if (RCL_RET_OK != ret) {
      fail_ret = ret;
      goto fail;
    }
  }

#ifdef RCL_COMMAND_LINE_ENABLED
  if (context->global_arguments.impl->enclave) {
    context->impl->init_options.impl->rmw_init_options.enclave = rcutils_strdup(
      context->global_arguments.impl->enclave,
      context->impl->allocator);
  } else {
    context->impl->init_options.impl->rmw_init_options.enclave = rcutils_strdup(
      "/", context->impl->allocator);
  }

  if (!context->impl->init_options.impl->rmw_init_options.enclave) {
    RCL_SET_ERROR_MSG("failed to set context name");
    fail_ret = RCL_RET_BAD_ALLOC;
    goto fail;
  }

  int validation_result;
  size_t invalid_index;
  ret = rcl_validate_enclave_name(
    context->impl->init_options.impl->rmw_init_options.enclave,
    &validation_result,
    &invalid_index);
  if (RCL_RET_OK != ret) {
    RCL_SET_ERROR_MSG("rcl_validate_enclave_name() failed");
    fail_ret = ret;
    goto fail;
  }
  if (RCL_ENCLAVE_NAME_VALID != validation_result) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Enclave name is not valid: '%s'. Invalid index: %zu",
      rcl_enclave_name_validation_result_string(validation_result),
      invalid_index);
    fail_ret = RCL_RET_ERROR;
    goto fail;
  }

  rmw_security_options_t * security_options =
    &context->impl->init_options.impl->rmw_init_options.security_options;
  ret = rcl_get_security_options_from_environment(
    context->impl->init_options.impl->rmw_init_options.enclave,
    &context->impl->allocator,
    security_options);
  if (RCL_RET_OK != ret) {
    fail_ret = ret;
    goto fail;
  }
#endif //RCL_COMMAND_LINE_ENABLED

  // Initialize rmw_init.
  rmw_ret_t rmw_ret = rmw_init(
    &(context->impl->init_options.impl->rmw_init_options),
    &(context->impl->rmw_context));
  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    fail_ret = rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
    goto fail;
  }

  TRACEPOINT(rcl_init, (const void *)context);

  return RCL_RET_OK;
fail:
  __cleanup_context(context);
  return fail_ret;
}

rcl_ret_t
rcl_shutdown(rcl_context_t * context)
{
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME,
    "Shutting down ROS client library, for context at address: %p", (void *) context);
  RCL_CHECK_ARGUMENT_FOR_NULL(context, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    context->impl, "context is zero-initialized", return RCL_RET_INVALID_ARGUMENT);
  if (!rcl_context_is_valid(context)) {
    RCL_SET_ERROR_MSG("rcl_shutdown already called on the given context");
    return RCL_RET_ALREADY_SHUTDOWN;
  }

  rmw_ret_t rmw_ret = rmw_shutdown(&(context->impl->rmw_context));
  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
  }

  // reset the instance id to 0 to indicate "invalid"
  context->instance_id_storage = 0;

  return RCL_RET_OK;
}

#ifdef __cplusplus
}
#endif
