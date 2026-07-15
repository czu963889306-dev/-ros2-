// Copyright 2021 Open Source Robotics Foundation, Inc.
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
#include "rmw/subscription_content_filter_options.h"

rmw_subscription_content_filter_options_t
rmw_get_zero_initialized_content_filter_options()
{
  return (const rmw_subscription_content_filter_options_t) {
           .filter_expression = NULL,
           .expression_parameters = rcutils_get_zero_initialized_string_array()
  };  // NOLINT(readability/braces): false positive
}

rmw_ret_t
rmw_subscription_content_filter_options_init(
  const char * filter_expression,
  size_t expression_parameters_argc,
  const char * expression_parameter_argv[],
  const rcutils_allocator_t * allocator,
  rmw_subscription_content_filter_options_t * options)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(filter_expression, RMW_RET_INVALID_ARGUMENT);
  if (expression_parameters_argc > 0) {
    RMW_CHECK_ARGUMENT_FOR_NULL(expression_parameter_argv, RMW_RET_INVALID_ARGUMENT);
  }
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(options, RMW_RET_INVALID_ARGUMENT);

  rmw_ret_t ret = RMW_RET_OK;
  rcutils_ret_t rcutils_ret;
  char * new_filter_expression = NULL;
  size_t i;

  new_filter_expression = rcutils_strdup(filter_expression, *allocator);
  if (!new_filter_expression) {
    RMW_SET_ERROR_MSG("failed to copy filter expression");
    ret = RMW_RET_BAD_ALLOC;
    goto failed;
  }

  if (expression_parameters_argc > 0) {
    rcutils_ret_t rcutils_ret = rcutils_string_array_init(
      &options->expression_parameters, expression_parameters_argc, allocator);
    if (RCUTILS_RET_OK != rcutils_ret) {
      RMW_SET_ERROR_MSG("failed to init string array for expression parameters");
      ret = RMW_RET_BAD_ALLOC;
      goto failed;
    }

    for (i = 0; i < expression_parameters_argc; i++) {
      options->expression_parameters.data[i] =
        rcutils_strdup(expression_parameter_argv[i], *allocator);
      if (!options->expression_parameters.data[i]) {
        RMW_SET_ERROR_MSG("failed to copy expression parameter");
        ret = RMW_RET_BAD_ALLOC;
        goto clear_expression_parameters;
      }
    }
  }

  options->filter_expression = new_filter_expression;

  return RMW_RET_OK;

clear_expression_parameters:
  rcutils_ret = rcutils_string_array_fini(&options->expression_parameters);
  if (RCUTILS_RET_OK != rcutils_ret) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to fini string array.\n");
  }

failed:
  allocator->deallocate(new_filter_expression, allocator->state);

  return ret;
}

rmw_ret_t
rmw_subscription_content_filter_options_set(
  const char * filter_expression,
  size_t expression_parameters_argc,
  const char * expression_parameter_argv[],
  const rcutils_allocator_t * allocator,
  rmw_subscription_content_filter_options_t * options)
{
  rmw_ret_t ret = rmw_subscription_content_filter_options_fini(options, allocator);
  if (ret != RMW_RET_OK) {
    return ret;
  }

  return rmw_subscription_content_filter_options_init(
    filter_expression,
    expression_parameters_argc,
    expression_parameter_argv,
    allocator,
    options
  );
}

rmw_ret_t
rmw_subscription_content_filter_options_copy(
  const rmw_subscription_content_filter_options_t * src,
  const rcutils_allocator_t * allocator,
  rmw_subscription_content_filter_options_t * dst)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(src, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(dst, RMW_RET_INVALID_ARGUMENT);

  return rmw_subscription_content_filter_options_set(
    src->filter_expression,
    src->expression_parameters.size,
    (const char **)src->expression_parameters.data,
    allocator,
    dst
  );
}

rmw_ret_t
rmw_subscription_content_filter_options_fini(
  rmw_subscription_content_filter_options_t * options,
  const rcutils_allocator_t * allocator)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(options, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);

  if (options->filter_expression) {
    allocator->deallocate(options->filter_expression, allocator->state);
    options->filter_expression = NULL;
  }

  rcutils_ret_t ret = rcutils_string_array_fini(&options->expression_parameters);
  if (RCUTILS_RET_OK != ret) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to fini string array.\n");
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}
