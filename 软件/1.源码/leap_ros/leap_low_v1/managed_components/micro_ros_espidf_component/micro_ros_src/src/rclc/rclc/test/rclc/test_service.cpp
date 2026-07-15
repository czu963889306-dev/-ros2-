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
#include <gtest/gtest.h>
#include <rclc/rclc.h>
#include "test_msgs/srv/basic_types.h"
#include "rcl/error_handling.h"

TEST(Test, rclc_service_init_default) {
  rclc_support_t support;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  const char * my_name = "test_service_node";
  const char * my_namespace = "";
  const char * topic_name = "add_two_ints";
  const char * expected_topic_name = "/add_two_ints";
  rcl_node_t node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&node, my_name, my_namespace, &support);

  // test with valid arguments
  rcl_service_t service = rcl_get_zero_initialized_service();
  const rosidl_service_type_support_t * type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  rc = rclc_service_init_default(&service, &node, type_support, topic_name);
  EXPECT_EQ(RCL_RET_OK, rc);

  // check service topic name
  EXPECT_EQ(strcmp(rcl_service_get_service_name(&service), expected_topic_name), 0);

  // check that valid service is valid
  EXPECT_TRUE(rcl_service_is_valid(&service));
  rcl_reset_error();

  // tests with invalid arguments
  rc = rclc_service_init_default(nullptr, &node, type_support, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init_default(&service, nullptr, type_support, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init_default(&service, &node, nullptr, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init_default(&service, &node, type_support, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // clean up
  rc = rcl_service_fini(&service, &node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}


TEST(Test, rclc_service_init_best_effort) {
  rclc_support_t support;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  const char * my_name = "test_service_node_be";
  const char * my_namespace = "";
  const char * topic_name = "add_two_ints";
  const char * expected_topic_name = "/add_two_ints";
  rcl_node_t node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&node, my_name, my_namespace, &support);

  // test with valid arguments
  rcl_service_t service = rcl_get_zero_initialized_service();
  const rosidl_service_type_support_t * type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  rc = rclc_service_init_best_effort(&service, &node, type_support, topic_name);
  EXPECT_EQ(RCL_RET_OK, rc);

  // check for qos-option best effort
  const rcl_service_options_t * srv_options = rcl_service_get_options(&service);
  EXPECT_EQ(srv_options->qos.reliability, RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT);

  // check service topic name
  EXPECT_EQ(strcmp(rcl_service_get_service_name(&service), expected_topic_name), 0);

  // check that valid service is valid
  EXPECT_TRUE(rcl_service_is_valid(&service));
  rcl_reset_error();

  // tests with invalid arguments
  rc = rclc_service_init_default(nullptr, &node, type_support, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init_default(&service, nullptr, type_support, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init_default(&service, &node, nullptr, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init_default(&service, &node, type_support, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // clean up
  rc = rcl_service_fini(&service, &node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}

TEST(Test, rclc_service_init_qos) {
  rclc_support_t support;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  const char * my_name = "test_service_node_be";
  const char * my_namespace = "";
  const char * topic_name = "add_two_ints";
  const char * expected_topic_name = "/add_two_ints";
  rcl_node_t node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&node, my_name, my_namespace, &support);

  // test with valid arguments
  rcl_service_t service = rcl_get_zero_initialized_service();
  const rmw_qos_profile_t * qos_profile = &rmw_qos_profile_default;
  const rosidl_service_type_support_t * type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  rc = rclc_service_init(&service, &node, type_support, topic_name, qos_profile);
  EXPECT_EQ(RCL_RET_OK, rc);

  // check for qos-option best effort
  const rcl_service_options_t * srv_options = rcl_service_get_options(&service);
  EXPECT_EQ(srv_options->qos.reliability, rmw_qos_profile_default.reliability);

  // check service topic name
  EXPECT_EQ(strcmp(rcl_service_get_service_name(&service), expected_topic_name), 0);

  // check that valid service is valid
  EXPECT_TRUE(rcl_service_is_valid(&service));
  rcl_reset_error();

  // tests with invalid arguments
  rc = rclc_service_init(nullptr, &node, type_support, topic_name, qos_profile);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init(&service, nullptr, type_support, topic_name, qos_profile);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init(&service, &node, nullptr, topic_name, qos_profile);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init(&service, &node, type_support, nullptr, qos_profile);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_service_init(&service, &node, type_support, topic_name, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // clean up
  rc = rcl_service_fini(&service, &node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}
