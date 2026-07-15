// Copyright (c) 2020 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
// Copyright 2014 Open Source Robotics Foundation, Inc.
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

#include "rclc/client.h"

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>
#include <rmw/qos_profiles.h>

rcl_ret_t
rclc_client_init_default(
  rcl_client_t * client,
  const rcl_node_t * node,
  const rosidl_service_type_support_t * type_support,
  const char * service_name)
{
  return rclc_client_init(
    client, node, type_support, service_name,
    &rmw_qos_profile_services_default);
}

rcl_ret_t
rclc_client_init_best_effort(
  rcl_client_t * client,
  const rcl_node_t * node,
  const rosidl_service_type_support_t * type_support,
  const char * service_name)
{
  rmw_qos_profile_t rmw_qos_profile_services_best_effort = rmw_qos_profile_services_default;
  rmw_qos_profile_services_best_effort.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;

  return rclc_client_init(
    client, node, type_support, service_name,
    &rmw_qos_profile_services_best_effort);
}

rcl_ret_t
rclc_client_init(
  rcl_client_t * client,
  const rcl_node_t * node,
  const rosidl_service_type_support_t * type_support,
  const char * service_name,
  const rmw_qos_profile_t * qos_profile)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    client, "client is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    type_support, "type_support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    service_name, "service_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    qos_profile, "qos_profile is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  (*client) = rcl_get_zero_initialized_client();
  rcl_client_options_t client_opt = rcl_client_get_default_options();
  client_opt.qos = *qos_profile;
  rcl_ret_t rc = rcl_client_init(
    client,
    node,
    type_support,
    service_name,
    &client_opt);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_client_init_best_effort, rcl_client_init);
  }
  return rc;
}
