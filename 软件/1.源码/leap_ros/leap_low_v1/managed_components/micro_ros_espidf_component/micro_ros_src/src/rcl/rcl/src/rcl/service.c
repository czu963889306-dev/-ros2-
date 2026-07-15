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

#include "rcl/service.h"

#include <stdio.h>
#include <string.h>

#include "rcl/error_handling.h"
#include "rcl/node.h"
#include "rcutils/logging_macros.h"
#include "rcutils/macros.h"
#include "rmw/error_handling.h"
#include "rmw/rmw.h"
#include "tracetools/tracetools.h"

struct rcl_service_impl_s
{
  rcl_service_options_t options;
  rmw_qos_profile_t actual_request_subscription_qos;
  rmw_qos_profile_t actual_response_publisher_qos;
  rmw_service_t * rmw_handle;
};

rcl_service_t
rcl_get_zero_initialized_service()
{
  static rcl_service_t null_service = {0};
  return null_service;
}

rcl_ret_t
rcl_service_init(
  rcl_service_t * service,
  const rcl_node_t * node,
  const rosidl_service_type_support_t * type_support,
  const char * service_name,
  const rcl_service_options_t * options)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ALREADY_INIT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_SERVICE_NAME_INVALID);

  rcl_ret_t fail_ret = RCL_RET_ERROR;

  // Check options and allocator first, so the allocator can be used in errors.
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  rcl_allocator_t * allocator = (rcl_allocator_t *)&options->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  RCL_CHECK_ARGUMENT_FOR_NULL(service, RCL_RET_INVALID_ARGUMENT);
  if (!rcl_node_is_valid(node)) {
    return RCL_RET_NODE_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(type_support, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(service_name, RCL_RET_INVALID_ARGUMENT);
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Initializing service for service name '%s'", service_name);
  if (service->impl) {
    RCL_SET_ERROR_MSG("service already initialized, or memory was unintialized");
    return RCL_RET_ALREADY_INIT;
  }

  // Expand and remap the given service name.
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
    } else if (ret != RCL_RET_BAD_ALLOC) {
      ret = RCL_RET_ERROR;
    }
    goto cleanup;
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Expanded and remapped service name '%s'", remapped_service_name);

  // Allocate space for the implementation struct.
  service->impl = (rcl_service_impl_t *)allocator->allocate(
    sizeof(rcl_service_impl_t), allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    service->impl, "allocating memory failed", ret = RCL_RET_BAD_ALLOC; goto cleanup);

  if (RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL == options->qos.durability) {
    RCUTILS_LOG_WARN_NAMED(
      ROS_PACKAGE_NAME,
      "Warning: Setting QoS durability to 'transient local' for service servers "
      "can cause them to receive requests from clients that have since terminated.");
  }
  // Fill out implementation struct.
  // rmw handle (create rmw service)
  // TODO(wjwwood): pass along the allocator to rmw when it supports it
  service->impl->rmw_handle = rmw_create_service(
    rcl_node_get_rmw_handle(node),
    type_support,
    remapped_service_name,
    &options->qos);
  if (!service->impl->rmw_handle) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }
  // get actual qos, and store it
  rmw_ret_t rmw_ret = rmw_service_request_subscription_get_actual_qos(
    service->impl->rmw_handle,
    &service->impl->actual_request_subscription_qos);

  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }

  rmw_ret = rmw_service_response_publisher_get_actual_qos(
    service->impl->rmw_handle,
    &service->impl->actual_response_publisher_qos);

  if (RMW_RET_OK != rmw_ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto fail;
  }

  // ROS specific namespacing conventions is not retrieved by get_actual_qos
  service->impl->actual_request_subscription_qos.avoid_ros_namespace_conventions =
    options->qos.avoid_ros_namespace_conventions;
  service->impl->actual_response_publisher_qos.avoid_ros_namespace_conventions =
    options->qos.avoid_ros_namespace_conventions;

  // options
  service->impl->options = *options;
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Service initialized");
  ret = RCL_RET_OK;
  TRACEPOINT(
    rcl_service_init,
    (const void *)service,
    (const void *)node,
    (const void *)service->impl->rmw_handle,
    remapped_service_name);
  goto cleanup;
fail:
  if (service->impl) {
    allocator->deallocate(service->impl, allocator->state);
    service->impl = NULL;
  }
  ret = fail_ret;
  // Fall through to clean up
cleanup:
  allocator->deallocate(remapped_service_name, allocator->state);
  return ret;
}

rcl_ret_t
rcl_service_fini(rcl_service_t * service, rcl_node_t * node)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_SERVICE_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Finalizing service");
  RCL_CHECK_ARGUMENT_FOR_NULL(service, RCL_RET_SERVICE_INVALID);
  if (!rcl_node_is_valid_except_context(node)) {
    return RCL_RET_NODE_INVALID;  // error already set
  }

  rcl_ret_t result = RCL_RET_OK;
  if (service->impl) {
    rcl_allocator_t allocator = service->impl->options.allocator;
    rmw_node_t * rmw_node = rcl_node_get_rmw_handle(node);
    if (!rmw_node) {
      return RCL_RET_INVALID_ARGUMENT;
    }
    rmw_ret_t ret = rmw_destroy_service(rmw_node, service->impl->rmw_handle);
    if (ret != RMW_RET_OK) {
      RCL_SET_ERROR_MSG(rmw_get_error_string().str);
      result = RCL_RET_ERROR;
    }
    allocator.deallocate(service->impl, allocator.state);
    service->impl = NULL;
  }
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Service finalized");
  return result;
}

rcl_service_options_t
rcl_service_get_default_options()
{
  // !!! MAKE SURE THAT CHANGES TO THESE DEFAULTS ARE REFLECTED IN THE HEADER DOC STRING
  static rcl_service_options_t default_options;
  // Must set the allocator and qos after because they are not a compile time constant.
  default_options.qos = rmw_qos_profile_services_default;
  default_options.allocator = rcl_get_default_allocator();
  return default_options;
}

const char *
rcl_service_get_service_name(const rcl_service_t * service)
{
  const rcl_service_options_t * options = rcl_service_get_options(service);
  if (!options) {
    return NULL;
  }
  RCL_CHECK_FOR_NULL_WITH_MSG(service->impl->rmw_handle, "service is invalid", return NULL);
  return service->impl->rmw_handle->service_name;
}

#define _service_get_options(service) & service->impl->options

const rcl_service_options_t *
rcl_service_get_options(const rcl_service_t * service)
{
  if (!rcl_service_is_valid(service)) {
    return NULL;  // error already set
  }
  return _service_get_options(service);
}

rmw_service_t *
rcl_service_get_rmw_handle(const rcl_service_t * service)
{
  if (!rcl_service_is_valid(service)) {
    return NULL;  // error already set
  }
  return service->impl->rmw_handle;
}

rcl_ret_t
rcl_take_request_with_info(
  const rcl_service_t * service,
  rmw_service_info_t * request_header,
  void * ros_request)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Service server taking service request");
  if (!rcl_service_is_valid(service)) {
    return RCL_RET_SERVICE_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(request_header, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ros_request, RCL_RET_INVALID_ARGUMENT);
  const rcl_service_options_t * options = rcl_service_get_options(service);
  RCL_CHECK_FOR_NULL_WITH_MSG(options, "Failed to get service options", return RCL_RET_ERROR);

  bool taken = false;
  rmw_ret_t ret = rmw_take_request(
    service->impl->rmw_handle, request_header, ros_request, &taken);
  if (RMW_RET_OK != ret) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    if (RMW_RET_BAD_ALLOC == ret) {
      return RCL_RET_BAD_ALLOC;
    }
    return RCL_RET_ERROR;
  }
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Service take request succeeded: %s", taken ? "true" : "false");
  if (!taken) {
    return RCL_RET_SERVICE_TAKE_FAILED;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_take_request(
  const rcl_service_t * service,
  rmw_request_id_t * request_header,
  void * ros_request)
{
  rmw_service_info_t header;
  header.request_id = *request_header;
  rcl_ret_t ret = rcl_take_request_with_info(service, &header, ros_request);
  *request_header = header.request_id;
  return ret;
}

rcl_ret_t
rcl_send_response(
  const rcl_service_t * service,
  rmw_request_id_t * request_header,
  void * ros_response)
{
  rcl_ret_t ret;
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Sending service response");
  if (!rcl_service_is_valid(service)) {
    return RCL_RET_SERVICE_INVALID;  // error already set
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(request_header, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ros_response, RCL_RET_INVALID_ARGUMENT);
  const rcl_service_options_t * options = rcl_service_get_options(service);
  RCL_CHECK_FOR_NULL_WITH_MSG(options, "Failed to get service options", return RCL_RET_ERROR);

  ret = rmw_send_response(service->impl->rmw_handle, request_header, ros_response);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    if (ret == RMW_RET_TIMEOUT) {
      return RCL_RET_TIMEOUT;
    }
    return RCL_RET_ERROR;
  }
  return RCL_RET_OK;
}

bool
rcl_service_is_valid(const rcl_service_t * service)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(service, "service pointer is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    service->impl, "service's implementation is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    service->impl->rmw_handle, "service's rmw handle is invalid", return false);
  return true;
}

const rmw_qos_profile_t *
rcl_service_request_subscription_get_actual_qos(const rcl_service_t * service)
{
  if (!rcl_service_is_valid(service)) {
    return NULL;
  }
  return &service->impl->actual_request_subscription_qos;
}

const rmw_qos_profile_t *
rcl_service_response_publisher_get_actual_qos(const rcl_service_t * service)
{
  if (!rcl_service_is_valid(service)) {
    return NULL;
  }
  return &service->impl->actual_response_publisher_qos;
}

rcl_ret_t
rcl_service_set_on_new_request_callback(
  const rcl_service_t * service,
  rcl_event_callback_t callback,
  const void * user_data)
{
  if (!rcl_service_is_valid(service)) {
    // error state already set
    return RCL_RET_INVALID_ARGUMENT;
  }

  return rmw_service_set_on_new_request_callback(
    service->impl->rmw_handle,
    callback,
    user_data);
}

#ifdef __cplusplus
}
#endif
