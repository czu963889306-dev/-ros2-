// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#include "rcl/client.h"

#include <stdio.h>
#include <string.h>

#include "rcl/error_handling.h"
#include "rcl/node.h"
#include "rcutils/logging_macros.h"
#include "rcutils/macros.h"
#include "rcutils/stdatomic_helper.h"
#include "rmw/error_handling.h"
#include "rmw/rmw.h"
#include "tracetools/tracetools.h"

#include "./common.h"

struct rcl_client_impl_s
{
  rcl_client_options_t options;
  rmw_qos_profile_t actual_request_publisher_qos;
  rmw_qos_profile_t actual_response_subscription_qos;
  rmw_client_t * rmw_handle;
  atomic_int_least64_t sequence_number;
};

rcl_client_t
rcl_get_zero_initialized_client()
{
  static rcl_client_t null_client = {0};
  return null_client;
}

rcl_ret_t
rcl_client_init(
  rcl_client_t * client,
  const rcl_node_t * node,
  const rosidl_service_type_support_t * type_support,
  const char * service_name,
  const rcl_client_options_t * options)
{
  rcl_ret_t fail_ret = RCL_RET_ERROR;

  // check the options and allocator first, so the allocator can be passed to errors
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = (rcl_allocator_t *)&options->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(client, RCL_RET_INVALID_ARGUMENT);
  if (!rcl_node_is_valid(node)) {
    return RCL_RET_NODE_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(type_support, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(service_name, RCL_RET_INVALID_ARGUMENT);
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Initializing client for service name '%s'", service_name);
  if (client->impl) {
    RCL_SET_ERROR_MSG("client already initialized, or memory was unintialized");
    return RCL_RET_ALREADY_INIT;
  }

  // Expand the given service name.
  char * remapped_service_name = NULL;
  rcl_ret_t ret = rcl_node_resolve_name(
    node,
    service_name,
    *allocator,
    true,
    false,
    &remapped_service_name);
  if (ret != RCL_RET_OK) {
    if (ret == RCL_RET_SERVICE_NAME_INVALID || ret == RCL_RET_UNKNOWN_SUBSTITUTION) {
      ret = RCL_RET_SERVICE_NAME_INVALID;
    } else if (RCL_RET_BAD_ALLOC != ret) {
      ret = RCL_RET_ERROR;
    }
    goto cleanup;
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Expanded and remapped service name '%s'", remapped_service_name);

  // Allocate space for the implementation struct.
  client->impl = (rcl_client_impl_t *)allocator->allocate(
    sizeof(rcl_client_impl_t), allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    client->impl, "allocating memory failed", ret = RCL_RET_BAD_ALLOC; goto cleanup);
  // Fill out implementation struct.
  // rmw handle (create rmw client)
  // TODO(wjwwood): pass along the allocator to rmw when it supports it
  client->impl->rmw_handle = rmw_create_client(
    rcl_node_get_rmw_handle(node),
    type_support,
    remapped_service_name,
    &options->qos);
  if (!client->impl->rmw_handle) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }

  // get actual qos, and store it
  rmw_ret_t rmw_ret = rmw_client_request_publisher_get_actual_qos(
    client->impl->rmw_handle,
    &client->impl->actual_request_publisher_qos);

  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }

  rmw_ret = rmw_client_response_subscription_get_actual_qos(
    client->impl->rmw_handle,
    &client->impl->actual_response_subscription_qos);

  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }

  // ROS specific namespacing conventions avoidance
  // is not retrieved by get_actual_qos
  client->impl->actual_request_publisher_qos.avoid_ros_namespace_conventions =
    options->qos.avoid_ros_namespace_conventions;
  client->impl->actual_response_subscription_qos.avoid_ros_namespace_conventions =
    options->qos.avoid_ros_namespace_conventions;

  // options
  client->impl->options = *options;
  atomic_init(&client->impl->sequence_number, 0);
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Client initialized");
  ret = RCL_RET_OK;
  TRACEPOINT(
    rcl_client_init,
    (const void *)client,
    (const void *)node,
    (const void *)client->impl->rmw_handle,
    remapped_service_name);
  goto cleanup;
fail:
  if (client->impl) {
    allocator->deallocate(client->impl, allocator->state);
    client->impl = NULL;
  }
  ret = fail_ret;
  // Fall through to cleanup
cleanup:
  allocator->deallocate(remapped_service_name, allocator->state);
  return ret;
}

rcl_ret_t
rcl_client_fini(rcl_client_t * client, rcl_node_t * node)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Finalizing client");
  rcl_ret_t result = RCL_RET_OK;
  RCL_CHECK_ARGUMENT_FOR_NULL(client, RCL_RET_INVALID_ARGUMENT);
  if (!rcl_node_is_valid_except_context(node)) {
    return RCL_RET_NODE_INVALID;  // error already set
  }
  if (client->impl) {
    rcl_allocator_t allocator = client->impl->options.allocator;
    rmw_node_t * rmw_node = rcl_node_get_rmw_handle(node);
    if (!rmw_node) {
      return RCL_RET_INVALID_ARGUMENT;
    }
    rmw_ret_t ret = rmw_destroy_client(rmw_node, client->impl->rmw_handle);
    if (ret != RMW_RET_OK) {
      RCL_SET_ERROR_MSG(rmw_get_error_string().str);
      result = RCL_RET_ERROR;
    }
    allocator.deallocate(client->impl, allocator.state);
    client->impl = NULL;
  }
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Client finalized");
  return result;
}

rcl_client_options_t
rcl_client_get_default_options()
{
  // !!! MAKE SURE THAT CHANGES TO THESE DEFAULTS ARE REFLECTED IN THE HEADER DOC STRING
  static rcl_client_options_t default_options;
  // Must set the allocator and qos after because they are not a compile time constant.
  default_options.qos = rmw_qos_profile_services_default;
  default_options.allocator = rcl_get_default_allocator();
  return default_options;
}

const char *
rcl_client_get_service_name(const rcl_client_t * client)
{
  if (!rcl_client_is_valid(client)) {
    return NULL;  // error already set
  }
  return client->impl->rmw_handle->service_name;
}

#define _client_get_options(client) & client->impl->options;

const rcl_client_options_t *
rcl_client_get_options(const rcl_client_t * client)
{
  if (!rcl_client_is_valid(client)) {
    return NULL;  // error already set
  }
  return _client_get_options(client);
}

rmw_client_t *
rcl_client_get_rmw_handle(const rcl_client_t * client)
{
  if (!rcl_client_is_valid(client)) {
    return NULL;  // error already set
  }
  return client->impl->rmw_handle;
}

rcl_ret_t
rcl_send_request(const rcl_client_t * client, const void * ros_request, int64_t * sequence_number)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Client sending service request");
  if (!rcl_client_is_valid(client)) {
    return RCL_RET_CLIENT_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(ros_request, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(sequence_number, RCL_RET_INVALID_ARGUMENT);
  *sequence_number = rcutils_atomic_load_int64_t(&client->impl->sequence_number);
  if (rmw_send_request(
      client->impl->rmw_handle, ros_request, sequence_number) != RMW_RET_OK)
  {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return RCL_RET_ERROR;
  }
  rcutils_atomic_exchange_int64_t(&client->impl->sequence_number, *sequence_number);
  return RCL_RET_OK;
}

rcl_ret_t
rcl_take_response_with_info(
  const rcl_client_t * client,
  rmw_service_info_t * request_header,
  void * ros_response)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Client taking service response");
  if (!rcl_client_is_valid(client)) {
    return RCL_RET_CLIENT_INVALID;  // error already set
  }

  RCL_CHECK_ARGUMENT_FOR_NULL(request_header, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ros_response, RCL_RET_INVALID_ARGUMENT);

  bool taken = false;
  request_header->source_timestamp = 0;
  request_header->received_timestamp = 0;
  if (rmw_take_response(
      client->impl->rmw_handle, request_header, ros_response, &taken) != RMW_RET_OK)
  {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return RCL_RET_ERROR;
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Client take response succeeded: %s", taken ? "true" : "false");
  if (!taken) {
    return RCL_RET_CLIENT_TAKE_FAILED;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_take_response(
  const rcl_client_t * client,
  rmw_request_id_t * request_header,
  void * ros_response)
{
  rmw_service_info_t header;
  header.request_id = *request_header;
  rcl_ret_t ret = rcl_take_response_with_info(client, &header, ros_response);
  *request_header = header.request_id;
  return ret;
}

bool
rcl_client_is_valid(const rcl_client_t * client)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(client, "client pointer is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    client->impl, "client's rmw implementation is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    client->impl->rmw_handle, "client's rmw handle is invalid", return false);
  return true;
}

const rmw_qos_profile_t *
rcl_client_request_publisher_get_actual_qos(const rcl_client_t * client)
{
  if (!rcl_client_is_valid(client)) {
    return NULL;
  }
  return &client->impl->actual_request_publisher_qos;
}

const rmw_qos_profile_t *
rcl_client_response_subscription_get_actual_qos(const rcl_client_t * client)
{
  if (!rcl_client_is_valid(client)) {
    return NULL;
  }
  return &client->impl->actual_response_subscription_qos;
}

rcl_ret_t
rcl_client_set_on_new_response_callback(
  const rcl_client_t * client,
  rcl_event_callback_t callback,
  const void * user_data)
{
  if (!rcl_client_is_valid(client)) {
    // error state already set
    return RCL_RET_INVALID_ARGUMENT;
  }

  return rmw_client_set_on_new_response_callback(
    client->impl->rmw_handle,
    callback,
    user_data);
}

#ifdef __cplusplus
}
#endif
