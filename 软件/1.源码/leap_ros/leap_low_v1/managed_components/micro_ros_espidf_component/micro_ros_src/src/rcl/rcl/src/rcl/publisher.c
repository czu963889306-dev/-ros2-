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

#include "rcl/publisher.h"

#include <stdio.h>
#include <string.h>

#include "rcl/allocator.h"
#include "rcl/error_handling.h"
#include "rcl/node.h"
#include "rcutils/logging_macros.h"
#include "rcutils/macros.h"
#include "rcl/time.h"
#include "rmw/time.h"
#include "rmw/error_handling.h"
#include "tracetools/tracetools.h"

#include "./common.h"
#include "./publisher_impl.h"

rcl_publisher_t
rcl_get_zero_initialized_publisher()
{
  static rcl_publisher_t null_publisher = {0};
  return null_publisher;
}

rcl_ret_t
rcl_publisher_init(
  rcl_publisher_t * publisher,
  const rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name,
  const rcl_publisher_options_t * options
)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ALREADY_INIT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_TOPIC_NAME_INVALID);

  rcl_ret_t fail_ret = RCL_RET_ERROR;

  // Check options and allocator first, so allocator can be used with errors.
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = (rcl_allocator_t *)&options->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  RCL_CHECK_ARGUMENT_FOR_NULL(publisher, RCL_RET_INVALID_ARGUMENT);
  if (publisher->impl) {
    RCL_SET_ERROR_MSG("publisher already initialized, or memory was unintialized");
    return RCL_RET_ALREADY_INIT;
  }
  if (!rcl_node_is_valid(node)) {
    return RCL_RET_NODE_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(type_support, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(topic_name, RCL_RET_INVALID_ARGUMENT);
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Initializing publisher for topic name '%s'", topic_name);

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

  // Allocate space for the implementation struct.
  publisher->impl = (rcl_publisher_impl_t *)allocator->allocate(
    sizeof(rcl_publisher_impl_t), allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    publisher->impl, "allocating memory failed", ret = RCL_RET_BAD_ALLOC; goto cleanup);

  // Fill out implementation struct.
  // rmw handle (create rmw publisher)
  // TODO(wjwwood): pass along the allocator to rmw when it supports it
  publisher->impl->rmw_handle = rmw_create_publisher(
    rcl_node_get_rmw_handle(node),
    type_support,
    remapped_topic_name,
    &(options->qos),
    &(options->rmw_publisher_options));
  RCL_CHECK_FOR_NULL_WITH_MSG(
    publisher->impl->rmw_handle, rmw_get_error_string().str, goto fail);
  // get actual qos, and store it
  rmw_ret_t rmw_ret = rmw_publisher_get_actual_qos(
    publisher->impl->rmw_handle,
    &publisher->impl->actual_qos);
  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }
  publisher->impl->actual_qos.avoid_ros_namespace_conventions =
    options->qos.avoid_ros_namespace_conventions;
  // options
  publisher->impl->options = *options;
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Publisher initialized");
  // context
  publisher->impl->context = node->context;
  TRACEPOINT(
    rcl_publisher_init,
    (const void *)publisher,
    (const void *)node,
    (const void *)publisher->impl->rmw_handle,
    remapped_topic_name,
    options->qos.depth);
  goto cleanup;
fail:
  if (publisher->impl) {
    if (publisher->impl->rmw_handle) {
      rmw_ret_t rmw_fail_ret = rmw_destroy_publisher(
        rcl_node_get_rmw_handle(node), publisher->impl->rmw_handle);
      if (RMW_RET_OK != rmw_fail_ret) {
        RCUTILS_SAFE_FWRITE_TO_STDERR(rmw_get_error_string().str);
        RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
      }
    }

    allocator->deallocate(publisher->impl, allocator->state);
    publisher->impl = NULL;
  }

  ret = fail_ret;
  // Fall through to cleanup
cleanup:
  allocator->deallocate(remapped_topic_name, allocator->state);
  return ret;
}

rcl_ret_t
rcl_publisher_fini(rcl_publisher_t * publisher, rcl_node_t * node)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_PUBLISHER_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  rcl_ret_t result = RCL_RET_OK;
  RCL_CHECK_ARGUMENT_FOR_NULL(publisher, RCL_RET_PUBLISHER_INVALID);
  if (!rcl_node_is_valid_except_context(node)) {
    return RCL_RET_NODE_INVALID;  // error already set
  }

  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Finalizing publisher");
  if (publisher->impl) {
    rcl_allocator_t allocator = publisher->impl->options.allocator;
    rmw_node_t * rmw_node = rcl_node_get_rmw_handle(node);
    if (!rmw_node) {
      return RCL_RET_INVALID_ARGUMENT;
    }
    rmw_ret_t ret =
      rmw_destroy_publisher(rmw_node, publisher->impl->rmw_handle);
    if (ret != RMW_RET_OK) {
      RCL_SET_ERROR_MSG(rmw_get_error_string().str);
      result = RCL_RET_ERROR;
    }
    allocator.deallocate(publisher->impl, allocator.state);
    publisher->impl = NULL;
  }
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Publisher finalized");
  return result;
}

rcl_publisher_options_t
rcl_publisher_get_default_options()
{
  // !!! MAKE SURE THAT CHANGES TO THESE DEFAULTS ARE REFLECTED IN THE HEADER DOC STRING
  static rcl_publisher_options_t default_options;
  // Must set the allocator and qos after because they are not a compile time constant.
  default_options.qos = rmw_qos_profile_default;
  default_options.allocator = rcl_get_default_allocator();
  default_options.rmw_publisher_options = rmw_get_default_publisher_options();
  return default_options;
}

rcl_ret_t
rcl_borrow_loaned_message(
  const rcl_publisher_t * publisher,
  const rosidl_message_type_support_t * type_support,
  void ** ros_message)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_PUBLISHER_INVALID;  // error already set
  }
  return rcl_convert_rmw_ret_to_rcl_ret(
    rmw_borrow_loaned_message(publisher->impl->rmw_handle, type_support, ros_message));
}

rcl_ret_t
rcl_return_loaned_message_from_publisher(
  const rcl_publisher_t * publisher,
  void * loaned_message)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_PUBLISHER_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(loaned_message, RCL_RET_INVALID_ARGUMENT);
  return rcl_convert_rmw_ret_to_rcl_ret(
    rmw_return_loaned_message_from_publisher(publisher->impl->rmw_handle, loaned_message));
}

rcl_ret_t
rcl_publish(
  const rcl_publisher_t * publisher,
  const void * ros_message,
  rmw_publisher_allocation_t * allocation)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_PUBLISHER_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_PUBLISHER_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(ros_message, RCL_RET_INVALID_ARGUMENT);
  TRACEPOINT(rcl_publish, (const void *)publisher, (const void *)ros_message);
  if (rmw_publish(publisher->impl->rmw_handle, ros_message, allocation) != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return RCL_RET_ERROR;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_publish_serialized_message(
  const rcl_publisher_t * publisher,
  const rcl_serialized_message_t * serialized_message,
  rmw_publisher_allocation_t * allocation)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_PUBLISHER_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(serialized_message, RCL_RET_INVALID_ARGUMENT);
  rmw_ret_t ret = rmw_publish_serialized_message(
    publisher->impl->rmw_handle, serialized_message, allocation);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    if (ret == RMW_RET_BAD_ALLOC) {
      return RCL_RET_BAD_ALLOC;
    }
    return RCL_RET_ERROR;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_publish_loaned_message(
  const rcl_publisher_t * publisher,
  void * ros_message,
  rmw_publisher_allocation_t * allocation)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_PUBLISHER_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(ros_message, RCL_RET_INVALID_ARGUMENT);
  rmw_ret_t ret = rmw_publish_loaned_message(publisher->impl->rmw_handle, ros_message, allocation);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return RCL_RET_ERROR;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_publisher_assert_liveliness(const rcl_publisher_t * publisher)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_PUBLISHER_INVALID;  // error already set
  }
  if (rmw_publisher_assert_liveliness(publisher->impl->rmw_handle) != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return RCL_RET_ERROR;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_publisher_wait_for_all_acked(const rcl_publisher_t * publisher, rcl_duration_value_t timeout)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_PUBLISHER_INVALID;  // error already set
  }

  rmw_time_t rmw_timeout;
  if (timeout > 0) {
    rmw_timeout.sec = RCL_NS_TO_S(timeout);
    rmw_timeout.nsec = timeout % 1000000000;
  } else if (timeout < 0) {
    rmw_time_t infinite = RMW_DURATION_INFINITE;
    rmw_timeout = infinite;
  } else {
    rmw_time_t zero = RMW_DURATION_UNSPECIFIED;
    rmw_timeout = zero;
  }

  rmw_ret_t ret = rmw_publisher_wait_for_all_acked(publisher->impl->rmw_handle, rmw_timeout);
  if (ret != RMW_RET_OK) {
    if (ret == RMW_RET_TIMEOUT) {
      return RCL_RET_TIMEOUT;
    }
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    if (ret == RMW_RET_UNSUPPORTED) {
      return RCL_RET_UNSUPPORTED;
    } else {
      return RCL_RET_ERROR;
    }
  }

  return RCL_RET_OK;
}

const char *
rcl_publisher_get_topic_name(const rcl_publisher_t * publisher)
{
  if (!rcl_publisher_is_valid_except_context(publisher)) {
    return NULL;  // error already set
  }
  return publisher->impl->rmw_handle->topic_name;
}

#define _publisher_get_options(pub) & pub->impl->options

const rcl_publisher_options_t *
rcl_publisher_get_options(const rcl_publisher_t * publisher)
{
  if (!rcl_publisher_is_valid_except_context(publisher)) {
    return NULL;  // error already set
  }
  return _publisher_get_options(publisher);
}

rmw_publisher_t *
rcl_publisher_get_rmw_handle(const rcl_publisher_t * publisher)
{
  if (!rcl_publisher_is_valid_except_context(publisher)) {
    return NULL;  // error already set
  }
  return publisher->impl->rmw_handle;
}

rcl_context_t *
rcl_publisher_get_context(const rcl_publisher_t * publisher)
{
  if (!rcl_publisher_is_valid_except_context(publisher)) {
    return NULL;  // error already set
  }
  return publisher->impl->context;
}

bool
rcl_publisher_is_valid(const rcl_publisher_t * publisher)
{
  if (!rcl_publisher_is_valid_except_context(publisher)) {
    return false;  // error already set
  }
  if (!rcl_context_is_valid(publisher->impl->context)) {
    RCL_SET_ERROR_MSG("publisher's context is invalid");
    return false;
  }
  RCL_CHECK_FOR_NULL_WITH_MSG(
    publisher->impl->rmw_handle, "publisher's rmw handle is invalid", return false);
  return true;
}

bool
rcl_publisher_is_valid_except_context(const rcl_publisher_t * publisher)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(publisher, "publisher pointer is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    publisher->impl, "publisher implementation is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    publisher->impl->rmw_handle, "publisher's rmw handle is invalid", return false);
  return true;
}

rcl_ret_t
rcl_publisher_get_subscription_count(
  const rcl_publisher_t * publisher,
  size_t * subscription_count)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_PUBLISHER_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(subscription_count, RCL_RET_INVALID_ARGUMENT);

  rmw_ret_t ret = rmw_publisher_count_matched_subscriptions(
    publisher->impl->rmw_handle, subscription_count);

  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }
  return RCL_RET_OK;
}

const rmw_qos_profile_t *
rcl_publisher_get_actual_qos(const rcl_publisher_t * publisher)
{
  if (!rcl_publisher_is_valid_except_context(publisher)) {
    return NULL;
  }
  return &publisher->impl->actual_qos;
}

bool
rcl_publisher_can_loan_messages(const rcl_publisher_t * publisher)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return false;  // error message already set
  }

  bool disable_loaned_message = false;
  rcl_ret_t ret = rcl_get_disable_loaned_message(&disable_loaned_message);
  if (ret == RCL_RET_OK && disable_loaned_message) {
    return false;
  }

  return publisher->impl->rmw_handle->can_loan_messages;
}

#ifdef __cplusplus
}
#endif
