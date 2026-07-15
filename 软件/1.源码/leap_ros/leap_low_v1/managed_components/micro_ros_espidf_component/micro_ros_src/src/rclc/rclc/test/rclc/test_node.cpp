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
#include <gtest/gtest.h>
#include "rclc/node.h"

TEST(Test, rclc_node_init_default) {
  rclc_support_t support;
  rcl_ret_t rc;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_OK, rc);
  const char * my_name = "test_node";
  const char * my_namespace = "test_namespace";
  rcl_node_t node = rcl_get_zero_initialized_node();

  // test with valid arguments
  rc = rclc_node_init_default(&node, my_name, my_namespace, &support);
  EXPECT_EQ(RCL_RET_OK, rc);

  // tests with invalid arguments

  // test case: null pointer for node
  rc = rclc_node_init_default(nullptr, my_name, my_namespace, &support);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // test case: null pointer for name
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);

  rc = rclc_node_init_default(&node, nullptr, my_namespace, &support);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // test case: null pointer for namespace
  rc = rclc_node_init_default(&node, my_name, nullptr, &support);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // test case: null pointer for support obj
  rc = rclc_node_init_default(&node, my_name, my_namespace, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // clean up
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rcutils_reset_error();

  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}

TEST(Test, rclc_node_init_with_options) {
  rclc_support_t support;
  rcl_ret_t rc;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_OK, rc);
  const char * my_name = "test_node";
  const char * my_namespace = "test_namespace";
  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_node_options_t node_options = rcl_node_get_default_options();

  // test with invalid arguments
  rc = rclc_node_init_with_options(
    NULL, my_name, my_namespace, &support, &node_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_node_init_with_options(
    &node, NULL, my_namespace, &support, &node_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_node_init_with_options(
    &node, my_name, NULL, &support, &node_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_node_init_with_options(
    &node, my_name, my_namespace, NULL, &node_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_node_init_with_options(
    &node, my_name, my_namespace, &support, NULL);
  rcutils_reset_error();

  // test with valid arguments
  rc = rclc_node_init_with_options(
    &node, my_name, my_namespace, &support, &node_options);
  EXPECT_EQ(RCL_RET_OK, rc);

  // clean up
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rcutils_reset_error();

  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}
