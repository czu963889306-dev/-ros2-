// Copyright (c) 2019 - for information on the respective copyright owner
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

#include "rclc/publisher.h"

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>
#include <rmw/qos_profiles.h>

rcl_ret_t
rclc_publisher_init_default(
  rcl_publisher_t * publisher,
  const rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name)
{
  return rclc_publisher_init(
    publisher, node, type_support, topic_name,
    &rmw_qos_profile_default);
}

rcl_ret_t
rclc_publisher_init_best_effort(
  rcl_publisher_t * publisher,
  const rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name)
{
  return rclc_publisher_init(
    publisher, node, type_support, topic_name,
    &rmw_qos_profile_sensor_data);
}

rcl_ret_t
rclc_publisher_init(
  rcl_publisher_t * publisher,
  const rcl_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name,
  const rmw_qos_profile_t * qos_profile)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    publisher, "publisher is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    type_support, "type_support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    topic_name, "topic_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    qos_profile, "qos_profile is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  (*publisher) = rcl_get_zero_initialized_publisher();
  rcl_publisher_options_t pub_opt = rcl_publisher_get_default_options();
  pub_opt.qos = *qos_profile;
  rcl_ret_t rc = rcl_publisher_init(
    publisher,
    node,
    type_support,
    topic_name,
    &pub_opt);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(rclc_publisher_init_best_effort, rcl_publisher_init);
  }
  return rc;
}
