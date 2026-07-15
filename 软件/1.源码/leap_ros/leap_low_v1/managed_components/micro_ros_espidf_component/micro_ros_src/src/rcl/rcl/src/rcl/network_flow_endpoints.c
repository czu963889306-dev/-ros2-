// Copyright 2020 Ericsson AB
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

#include "rcl/error_handling.h"
#include "rcl/graph.h"
#include "rcl/network_flow_endpoints.h"
#include "rcl/publisher.h"
#include "rcl/subscription.h"

#include "rcutils/allocator.h"
#include "rcutils/macros.h"
#include "rcutils/types.h"

#include "rmw/error_handling.h"
#include "rmw/get_network_flow_endpoints.h"
#include "rmw/network_flow_endpoint_array.h"
#include "rmw/types.h"

#include "./common.h"

rcl_ret_t
__validate_network_flow_endpoint_array(
  rcl_network_flow_endpoint_array_t * network_flow_endpoint_array)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(network_flow_endpoint_array, RCL_RET_INVALID_ARGUMENT);

  rmw_error_string_t error_string;
  rmw_ret_t rmw_ret = rmw_network_flow_endpoint_array_check_zero(network_flow_endpoint_array);
  if (rmw_ret != RMW_RET_OK) {
    error_string = rmw_get_error_string();
    rmw_reset_error();
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "rcl_network_flow_endpoint_array_t must be zero initialized: %s,\n"
      "Use rcl_get_zero_initialized_network_flow_endpoint_array",
      error_string.str);
  }

  return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
}

rcl_ret_t
rcl_publisher_get_network_flow_endpoints(
  const rcl_publisher_t * publisher,
  rcutils_allocator_t * allocator,
  rcl_network_flow_endpoint_array_t * network_flow_endpoint_array)
{
  if (!rcl_publisher_is_valid(publisher)) {
    return RCL_RET_INVALID_ARGUMENT;
  }

  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rcl_ret = __validate_network_flow_endpoint_array(
    network_flow_endpoint_array);
  if (rcl_ret != RCL_RET_OK) {
    return rcl_ret;
  }

  rmw_error_string_t error_string;
  rmw_ret_t rmw_ret = rmw_publisher_get_network_flow_endpoints(
    rcl_publisher_get_rmw_handle(publisher),
    allocator,
    network_flow_endpoint_array);
  if (rmw_ret != RMW_RET_OK) {
    error_string = rmw_get_error_string();
    rmw_reset_error();
    RCL_SET_ERROR_MSG(error_string.str);
  }
  return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
}

rcl_ret_t
rcl_subscription_get_network_flow_endpoints(
  const rcl_subscription_t * subscription,
  rcutils_allocator_t * allocator,
  rcl_network_flow_endpoint_array_t * network_flow_endpoint_array)
{
  if (!rcl_subscription_is_valid(subscription)) {
    return RCL_RET_INVALID_ARGUMENT;
  }

  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rcl_ret = __validate_network_flow_endpoint_array(
    network_flow_endpoint_array);
  if (rcl_ret != RCL_RET_OK) {
    return rcl_ret;
  }

  rmw_error_string_t error_string;
  rmw_ret_t rmw_ret = rmw_subscription_get_network_flow_endpoints(
    rcl_subscription_get_rmw_handle(subscription),
    allocator,
    network_flow_endpoint_array);
  if (rmw_ret != RMW_RET_OK) {
    error_string = rmw_get_error_string();
    rmw_reset_error();
    RCL_SET_ERROR_MSG(error_string.str);
  }
  return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
}

#ifdef __cplusplus
}
#endif
