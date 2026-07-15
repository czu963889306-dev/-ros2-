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

#include "rcl/subscription.h"

#include <stdio.h>

#include "rcl/error_handling.h"
#include "rcl/node.h"
#include "rcutils/env.h"
#include "rcutils/logging_macros.h"
#include "rcutils/strdup.h"
#include "rcutils/types/string_array.h"
#include "rmw/error_handling.h"
#include "rmw/subscription_content_filter_options.h"
#include "rmw/validate_full_topic_name.h"
#include "tracetools/tracetools.h"

#include "./common.h"
#include "./subscription_impl.h"


rcl_subscription_t
rcl_get_zero_initialized_subscription()
{
  static rcl_subscription_t null_subscription = {0};
  return null_subscription;
}

rcl_ret_t
rcl_subscription_init(
  rcl_subscription_t * subscription,
  const rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name,
  const rcl_subscription_options_t * options
)
{
  rcl_ret_t fail_ret = RCL_RET_ERROR;

  // Check options and allocator first, so the allocator can be used in errors.
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = (rcl_allocator_t *)&options->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(subscription, RCL_RET_INVALID_ARGUMENT);
  if (!rcl_node_is_valid(node)) {
    return RCL_RET_NODE_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(type_support, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(topic_name, RCL_RET_INVALID_ARGUMENT);
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Initializing subscription for topic name '%s'", topic_name);
  if (subscription->impl) {
    RCL_SET_ERROR_MSG("subscription already initialized, or memory was uninitialized");
    return RCL_RET_ALREADY_INIT;
  }

  // Expand and remap the given topic name.
  char * remapped_topic_name = NULL;
  rcl_ret_t ret = rcl_node_resolve_name(
    node,
    topic_name,
    *allocator,
    false,
    false,
    &remapped_topic_name);
  if (ret != RCL_RET_OK) {
    if (ret == RCL_RET_TOPIC_NAME_INVALID || ret == RCL_RET_UNKNOWN_SUBSTITUTION) {
      ret = RCL_RET_TOPIC_NAME_INVALID;
    } else if (ret != RCL_RET_BAD_ALLOC) {
      ret = RCL_RET_ERROR;
    }
    goto cleanup;
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Expanded and remapped topic name '%s'", remapped_topic_name);

  // Allocate memory for the implementation struct.
  subscription->impl = (rcl_subscription_impl_t *)allocator->zero_allocate(
    1, sizeof(rcl_subscription_impl_t), allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    subscription->impl, "allocating memory failed", ret = RCL_RET_BAD_ALLOC; goto cleanup);
  // Fill out the implemenation struct.
  // rmw_handle
  // TODO(wjwwood): pass allocator once supported in rmw api.
  subscription->impl->rmw_handle = rmw_create_subscription(
    rcl_node_get_rmw_handle(node),
    type_support,
    remapped_topic_name,
    &(options->qos),
    &(options->rmw_subscription_options));
  if (!subscription->impl->rmw_handle) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }
  // get actual qos, and store it
  rmw_ret_t rmw_ret = rmw_subscription_get_actual_qos(
    subscription->impl->rmw_handle,
    &subscription->impl->actual_qos);
  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }
  subscription->impl->actual_qos.avoid_ros_namespace_conventions =
    options->qos.avoid_ros_namespace_conventions;
  // options
  subscription->impl->options = *options;
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Subscription initialized");
  ret = RCL_RET_OK;
  TRACEPOINT(
    rcl_subscription_init,
    (const void *)subscription,
    (const void *)node,
    (const void *)subscription->impl->rmw_handle,
    remapped_topic_name,
    options->qos.depth);
  goto cleanup;
fail:
  if (subscription->impl) {
    if (subscription->impl->rmw_handle) {
      rmw_ret_t rmw_fail_ret = rmw_destroy_subscription(
        rcl_node_get_rmw_handle(node), subscription->impl->rmw_handle);
      if (RMW_RET_OK != rmw_fail_ret) {
        RCUTILS_SAFE_FWRITE_TO_STDERR(rmw_get_error_string().str);
        RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
      }
    }

    ret = rcl_subscription_options_fini(&subscription->impl->options);
    if (RCL_RET_OK != ret) {
      RCUTILS_SAFE_FWRITE_TO_STDERR(rmw_get_error_string().str);
      RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
    }

    allocator->deallocate(subscription->impl, allocator->state);
    subscription->impl = NULL;
  }
  ret = fail_ret;
  // Fall through to cleanup
cleanup:
  allocator->deallocate(remapped_topic_name, allocator->state);
  return ret;
}

rcl_ret_t
rcl_subscription_fini(rcl_subscription_t * subscription, rcl_node_t * node)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_SUBSCRIPTION_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Finalizing subscription");
  rcl_ret_t result = RCL_RET_OK;
  RCL_CHECK_ARGUMENT_FOR_NULL(subscription, RCL_RET_SUBSCRIPTION_INVALID);
  if (!rcl_node_is_valid_except_context(node)) {
    return RCL_RET_NODE_INVALID;  // error already set
  }
  if (subscription->impl) {
    rcl_allocator_t allocator = subscription->impl->options.allocator;
    rmw_node_t * rmw_node = rcl_node_get_rmw_handle(node);
    if (!rmw_node) {
      return RCL_RET_INVALID_ARGUMENT;
    }
    rmw_ret_t ret =
      rmw_destroy_subscription(rmw_node, subscription->impl->rmw_handle);
    if (ret != RMW_RET_OK) {
      RCL_SET_ERROR_MSG(rmw_get_error_string().str);
      result = RCL_RET_ERROR;
    }
    rcl_ret_t rcl_ret = rcl_subscription_options_fini(&subscription->impl->options);
    if (RCL_RET_OK != rcl_ret) {
      RCUTILS_SAFE_FWRITE_TO_STDERR(rcl_get_error_string().str);
      RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
      result = RCL_RET_ERROR;
    }

    allocator.deallocate(subscription->impl, allocator.state);
    subscription->impl = NULL;
  }
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Subscription finalized");
  return result;
}

rcl_subscription_options_t
rcl_subscription_get_default_options()
{
  // !!! MAKE SURE THAT CHANGES TO THESE DEFAULTS ARE REFLECTED IN THE HEADER DOC STRING
  static rcl_subscription_options_t default_options;
  // Must set these after declaration because they are not a compile time constants.
  default_options.qos = rmw_qos_profile_default;
  default_options.allocator = rcl_get_default_allocator();
  default_options.rmw_subscription_options = rmw_get_default_subscription_options();
  return default_options;
}

rcl_ret_t
rcl_subscription_options_fini(rcl_subscription_options_t * option)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(option, RCL_RET_INVALID_ARGUMENT);
  // fini rmw_subscription_options_t
  const rcl_allocator_t * allocator = &option->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  if (option->rmw_subscription_options.content_filter_options) {
    rmw_ret_t ret = rmw_subscription_content_filter_options_fini(
      option->rmw_subscription_options.content_filter_options, allocator);
    if (RCUTILS_RET_OK != ret) {
      RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to fini content filter options.\n");
      return rcl_convert_rmw_ret_to_rcl_ret(ret);
    }
    allocator->deallocate(
      option->rmw_subscription_options.content_filter_options, allocator->state);
    option->rmw_subscription_options.content_filter_options = NULL;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_subscription_options_set_content_filter_options(
  const char * filter_expression,
  size_t expression_parameters_argc,
  const char * expression_parameter_argv[],
  rcl_subscription_options_t * options)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(filter_expression, RCL_RET_INVALID_ARGUMENT);
  if (expression_parameters_argc > 100) {
    RCL_SET_ERROR_MSG("The maximum of expression parameters argument number is 100");
    return RCL_RET_INVALID_ARGUMENT;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  const rcl_allocator_t * allocator = &options->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret;
  rmw_ret_t rmw_ret;
  rmw_subscription_content_filter_options_t * original_content_filter_options =
    options->rmw_subscription_options.content_filter_options;
  rmw_subscription_content_filter_options_t content_filter_options_backup =
    rmw_get_zero_initialized_content_filter_options();

  if (original_content_filter_options) {
    // make a backup, restore the data if failure happened
    rmw_ret = rmw_subscription_content_filter_options_copy(
      original_content_filter_options,
      allocator,
      &content_filter_options_backup
    );
    if (rmw_ret != RMW_RET_OK) {
      return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
    }
  } else {
    options->rmw_subscription_options.content_filter_options =
      allocator->allocate(
      sizeof(rmw_subscription_content_filter_options_t), allocator->state);
    if (!options->rmw_subscription_options.content_filter_options) {
      RCL_SET_ERROR_MSG("failed to allocate memory");
      return RCL_RET_BAD_ALLOC;
    }
    *options->rmw_subscription_options.content_filter_options =
      rmw_get_zero_initialized_content_filter_options();
  }

  rmw_ret = rmw_subscription_content_filter_options_set(
    filter_expression,
    expression_parameters_argc,
    expression_parameter_argv,
    allocator,
    options->rmw_subscription_options.content_filter_options
  );

  if (rmw_ret != RMW_RET_OK) {
    ret = rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
    goto failed;
  }

  rmw_ret = rmw_subscription_content_filter_options_fini(
    &content_filter_options_backup,
    allocator
  );
  if (rmw_ret != RMW_RET_OK) {
    return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
  }

  return RMW_RET_OK;

failed:

  if (original_content_filter_options == NULL) {
    if (options->rmw_subscription_options.content_filter_options) {
      rmw_ret = rmw_subscription_content_filter_options_fini(
        options->rmw_subscription_options.content_filter_options,
        allocator
      );

      if (rmw_ret != RMW_RET_OK) {
        return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
      }

      allocator->deallocate(
        options->rmw_subscription_options.content_filter_options, allocator->state);
      options->rmw_subscription_options.content_filter_options = NULL;
    }
  } else {
    rmw_ret = rmw_subscription_content_filter_options_copy(
      &content_filter_options_backup,
      allocator,
      options->rmw_subscription_options.content_filter_options
    );
    if (rmw_ret != RMW_RET_OK) {
      return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
    }

    rmw_ret = rmw_subscription_content_filter_options_fini(
      &content_filter_options_backup,
      allocator
    );
    if (rmw_ret != RMW_RET_OK) {
      return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
    }
  }

  return ret;
}

rcl_subscription_content_filter_options_t
rcl_get_zero_initialized_subscription_content_filter_options()
{
  return (const rcl_subscription_content_filter_options_t) {
           .rmw_subscription_content_filter_options =
             rmw_get_zero_initialized_content_filter_options()
  };  // NOLINT(readability/braces): false positive
}

rcl_ret_t
rcl_subscription_content_filter_options_init(
  const rcl_subscription_t * subscription,
  const char * filter_expression,
  size_t expression_parameters_argc,
  const char * expression_parameter_argv[],
  rcl_subscription_content_filter_options_t * options)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  const rcl_allocator_t * allocator = &subscription->impl->options.allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  if (expression_parameters_argc > 100) {
    RCL_SET_ERROR_MSG("The maximum of expression parameters argument number is 100");
    return RCL_RET_INVALID_ARGUMENT;
  }

  rmw_ret_t rmw_ret = rmw_subscription_content_filter_options_init(
    filter_expression,
    expression_parameters_argc,
    expression_parameter_argv,
    allocator,
    &options->rmw_subscription_content_filter_options
  );

  return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
}

rcl_ret_t
rcl_subscription_content_filter_options_set(
  const rcl_subscription_t * subscription,
  const char * filter_expression,
  size_t expression_parameters_argc,
  const char * expression_parameter_argv[],
  rcl_subscription_content_filter_options_t * options)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;
  }
  if (expression_parameters_argc > 100) {
    RCL_SET_ERROR_MSG("The maximum of expression parameters argument number is 100");
    return RCL_RET_INVALID_ARGUMENT;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  const rcl_allocator_t * allocator = &subscription->impl->options.allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  rmw_ret_t ret = rmw_subscription_content_filter_options_set(
    filter_expression,
    expression_parameters_argc,
    expression_parameter_argv,
    allocator,
    &options->rmw_subscription_content_filter_options
  );
  return rcl_convert_rmw_ret_to_rcl_ret(ret);
}

rcl_ret_t
rcl_subscription_content_filter_options_fini(
  const rcl_subscription_t * subscription,
  rcl_subscription_content_filter_options_t * options)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  const rcl_allocator_t * allocator = &subscription->impl->options.allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  rmw_ret_t ret = rmw_subscription_content_filter_options_fini(
    &options->rmw_subscription_content_filter_options,
    allocator
  );

  return rcl_convert_rmw_ret_to_rcl_ret(ret);
}

bool
rcl_subscription_is_cft_enabled(const rcl_subscription_t * subscription)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return false;
  }
  return subscription->impl->rmw_handle->is_cft_enabled;
}

rcl_ret_t
rcl_subscription_set_content_filter(
  const rcl_subscription_t * subscription,
  const rcl_subscription_content_filter_options_t * options
)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_SUBSCRIPTION_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);

  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;
  }

  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  rmw_ret_t ret = rmw_subscription_set_content_filter(
    subscription->impl->rmw_handle,
    &options->rmw_subscription_content_filter_options);

  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }

  // copy options into subscription_options
  const rmw_subscription_content_filter_options_t * content_filter_options =
    &options->rmw_subscription_content_filter_options;
  return rcl_subscription_options_set_content_filter_options(
    content_filter_options->filter_expression,
    content_filter_options->expression_parameters.size,
    (const char **)content_filter_options->expression_parameters.data,
    &subscription->impl->options
  );
}

rcl_ret_t
rcl_subscription_get_content_filter(
  const rcl_subscription_t * subscription,
  rcl_subscription_content_filter_options_t * options
)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_SUBSCRIPTION_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);

  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = &subscription->impl->options.allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  rmw_ret_t rmw_ret = rmw_subscription_get_content_filter(
    subscription->impl->rmw_handle,
    allocator,
    &options->rmw_subscription_content_filter_options);

  return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
}

rcl_ret_t
rcl_take(
  const rcl_subscription_t * subscription,
  void * ros_message,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation
)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Subscription taking message");
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;  // error message already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(ros_message, RCL_RET_INVALID_ARGUMENT);

  // If message_info is NULL, use a place holder which can be discarded.
  rmw_message_info_t dummy_message_info;
  rmw_message_info_t * message_info_local = message_info ? message_info : &dummy_message_info;
  *message_info_local = rmw_get_zero_initialized_message_info();
  // Call rmw_take_with_info.
  bool taken = false;
  rmw_ret_t ret = rmw_take_with_info(
    subscription->impl->rmw_handle, ros_message, &taken, message_info_local, allocation);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Subscription take succeeded: %s", taken ? "true" : "false");
  TRACEPOINT(rcl_take, (const void *)ros_message);
  if (!taken) {
    return RCL_RET_SUBSCRIPTION_TAKE_FAILED;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_take_sequence(
  const rcl_subscription_t * subscription,
  size_t count,
  rmw_message_sequence_t * message_sequence,
  rmw_message_info_sequence_t * message_info_sequence,
  rmw_subscription_allocation_t * allocation
)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Subscription taking %zu messages", count);
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;  // error message already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(message_sequence, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(message_info_sequence, RCL_RET_INVALID_ARGUMENT);

  if (message_sequence->capacity < count) {
    RCL_SET_ERROR_MSG("Insufficient message sequence capacity for requested count");
    return RCL_RET_INVALID_ARGUMENT;
  }

  if (message_info_sequence->capacity < count) {
    RCL_SET_ERROR_MSG("Insufficient message info sequence capacity for requested count");
    return RCL_RET_INVALID_ARGUMENT;
  }

  // Set the sizes to zero to indicate that there are no valid messages
  message_sequence->size = 0u;
  message_info_sequence->size = 0u;

  size_t taken = 0u;
  rmw_ret_t ret = rmw_take_sequence(
    subscription->impl->rmw_handle, count, message_sequence, message_info_sequence, &taken,
    allocation);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Subscription took %zu messages", taken);
  if (0u == taken) {
    return RCL_RET_SUBSCRIPTION_TAKE_FAILED;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_take_serialized_message(
  const rcl_subscription_t * subscription,
  rcl_serialized_message_t * serialized_message,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation
)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Subscription taking serialized message");
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(serialized_message, RCL_RET_INVALID_ARGUMENT);
  // If message_info is NULL, use a place holder which can be discarded.
  rmw_message_info_t dummy_message_info;
  rmw_message_info_t * message_info_local = message_info ? message_info : &dummy_message_info;
  *message_info_local = rmw_get_zero_initialized_message_info();
  // Call rmw_take_with_info.
  bool taken = false;
  rmw_ret_t ret = rmw_take_serialized_message_with_info(
    subscription->impl->rmw_handle, serialized_message, &taken, message_info_local, allocation);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Subscription serialized take succeeded: %s", taken ? "true" : "false");
  if (!taken) {
    return RCL_RET_SUBSCRIPTION_TAKE_FAILED;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_take_loaned_message(
  const rcl_subscription_t * subscription,
  void ** loaned_message,
  rmw_message_info_t * message_info,
  rmw_subscription_allocation_t * allocation)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Subscription taking loaned message");
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(loaned_message, RCL_RET_INVALID_ARGUMENT);
  if (*loaned_message) {
    RCL_SET_ERROR_MSG("loaned message is already initialized");
    return RCL_RET_INVALID_ARGUMENT;
  }
  // If message_info is NULL, use a place holder which can be discarded.
  rmw_message_info_t dummy_message_info;
  rmw_message_info_t * message_info_local = message_info ? message_info : &dummy_message_info;
  *message_info_local = rmw_get_zero_initialized_message_info();
  // Call rmw_take_with_info.
  bool taken = false;
  rmw_ret_t ret = rmw_take_loaned_message_with_info(
    subscription->impl->rmw_handle, loaned_message, &taken, message_info_local, allocation);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Subscription loaned take succeeded: %s", taken ? "true" : "false");
  if (!taken) {
    return RCL_RET_SUBSCRIPTION_TAKE_FAILED;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_return_loaned_message_from_subscription(
  const rcl_subscription_t * subscription,
  void * loaned_message)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Subscription releasing loaned message");
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(loaned_message, RCL_RET_INVALID_ARGUMENT);
  return rcl_convert_rmw_ret_to_rcl_ret(
    rmw_return_loaned_message_from_subscription(
      subscription->impl->rmw_handle, loaned_message));
}

const char *
rcl_subscription_get_topic_name(const rcl_subscription_t * subscription)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return NULL;  // error already set
  }
  return subscription->impl->rmw_handle->topic_name;
}

#define _subscription_get_options(subscription) & subscription->impl->options

const rcl_subscription_options_t *
rcl_subscription_get_options(const rcl_subscription_t * subscription)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return NULL;  // error already set
  }
  return _subscription_get_options(subscription);
}

rmw_subscription_t *
rcl_subscription_get_rmw_handle(const rcl_subscription_t * subscription)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return NULL;  // error already  set
  }
  return subscription->impl->rmw_handle;
}

bool
rcl_subscription_is_valid(const rcl_subscription_t * subscription)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(subscription, "subscription pointer is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    subscription->impl, "subscription's implementation is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    subscription->impl->rmw_handle, "subscription's rmw handle is invalid", return false);
  return true;
}

rmw_ret_t
rcl_subscription_get_publisher_count(
  const rcl_subscription_t * subscription,
  size_t * publisher_count)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_SUBSCRIPTION_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);

  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_SUBSCRIPTION_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(publisher_count, RCL_RET_INVALID_ARGUMENT);
  rmw_ret_t ret = rmw_subscription_count_matched_publishers(
    subscription->impl->rmw_handle, publisher_count);

  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }
  return RCL_RET_OK;
}

const rmw_qos_profile_t *
rcl_subscription_get_actual_qos(const rcl_subscription_t * subscription)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return NULL;
  }
  return &subscription->impl->actual_qos;
}

bool
rcl_subscription_can_loan_messages(const rcl_subscription_t * subscription)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return false;  // error message already set
  }

  // Load disable flag to LoanedMessage via environmental variable.
  // TODO(clalancette): This is kind of a copy of rcl_get_disable_loaned_message(), but we need
  // more information than that function provides.
  bool disable_loaned_message = true;

  const char * env_val = NULL;
  const char * env_error_str = rcutils_get_env(RCL_DISABLE_LOANED_MESSAGES_ENV_VAR, &env_val);
  if (NULL != env_error_str) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to get disable_loaned_message: ");
    RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(
      "Error getting env var: '" RCUTILS_STRINGIFY(RCL_DISABLE_LOANED_MESSAGES_ENV_VAR) "': %s\n",
      env_error_str);
    return RCL_RET_ERROR;
  } else {
    disable_loaned_message = !(strcmp(env_val, "0") == 0);
  }

  if (disable_loaned_message) {
    return false;
  }

  return subscription->impl->rmw_handle->can_loan_messages;
}

rcl_ret_t
rcl_subscription_set_on_new_message_callback(
  const rcl_subscription_t * subscription,
  rcl_event_callback_t callback,
  const void * user_data)
{
  if (!rcl_subscription_is_valid(subscription)) {
    // error state already set
    return RCL_RET_INVALID_ARGUMENT;
  }

  return rmw_subscription_set_on_new_message_callback(
    subscription->impl->rmw_handle,
    callback,
    user_data);
}

#ifdef __cplusplus
}
#endif
