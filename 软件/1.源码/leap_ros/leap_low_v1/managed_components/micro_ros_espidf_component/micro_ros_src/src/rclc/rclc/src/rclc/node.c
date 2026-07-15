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

#include "rclc/node.h"

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>

rcl_ret_t
rclc_node_init_default(
  rcl_node_t * node,
  const char * name,
  const char * namespace_,
  rclc_support_t * support)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    name, "name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    namespace_, "namespace_ is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    support, "support is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rc = RCL_RET_OK;
  (*node) = rcl_get_zero_initialized_node();
  rcl_node_options_t node_ops = rcl_node_get_default_options();
  rc = rclc_node_init_with_options(
    node,
    name,
    namespace_,
    support,
    &node_ops);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_WARN(rclc_node_init_default, rclc_node_init_with_options);
  }
  return rc;
}

rcl_ret_t
rclc_node_init_with_options(
  rcl_node_t * node,
  const char * name,
  const char * namespace_,
  rclc_support_t * support,
  rcl_node_options_t * node_ops)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    name, "name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    namespace_, "namespace_ is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    support, "support is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node_ops, "support is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rc = RCL_RET_OK;
  (*node) = rcl_get_zero_initialized_node();
  rc = rcl_node_init(
    node,
    name,
    namespace_,
    &support->context,
    node_ops);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_WARN(rclc_node_init_with_options, rcl_node_init);
  }
  return rc;
}
