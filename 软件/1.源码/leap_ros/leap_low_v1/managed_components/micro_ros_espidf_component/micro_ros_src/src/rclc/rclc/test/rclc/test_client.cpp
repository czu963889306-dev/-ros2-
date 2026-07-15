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

TEST(Test, rclc_client_init_default) {
  rclc_support_t support;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  const char * my_name = "test_client_node";
  const char * my_namespace = "";
  const char * topic_name = "add_two_ints";
  const char * expected_topic_name = "/add_two_ints";
  rcl_node_t node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&node, my_name, my_namespace, &support);

  // test with valid arguments
  rcl_client_t client = rcl_get_zero_initialized_client();
  const rosidl_service_type_support_t * type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  rc = rclc_client_init_default(&client, &node, type_support, topic_name);
  EXPECT_EQ(RCL_RET_OK, rc);
  EXPECT_EQ(strcmp(rcl_client_get_service_name(&client), expected_topic_name), 0);

  // Initialize the client request.
  test_msgs__srv__BasicTypes_Request req;
  test_msgs__srv__BasicTypes_Request__init(&req);
  req.uint8_value = 1;
  req.uint32_value = 2;

  // Check that there were no errors while sending the request.
  int64_t sequence_number = 0;
  rc = rcl_send_request(&client, &req, &sequence_number);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  test_msgs__srv__BasicTypes_Request__fini(&req);

  // tests with invalid arguments
  rc = rclc_client_init_default(nullptr, &node, type_support, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init_default(&client, nullptr, type_support, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init_default(&client, &node, nullptr, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init_default(&client, &node, type_support, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // clean up
  rc = rcl_client_fini(&client, &node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}

TEST(Test, rclc_client_init_best_effort) {
  rclc_support_t support;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  const char * my_name = "test_client_node_be";
  const char * my_namespace = "";
  const char * topic_name = "add_two_ints";
  const char * expected_topic_name = "/add_two_ints";
  rcl_node_t node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&node, my_name, my_namespace, &support);

  // test with valid arguments
  rcl_client_t client = rcl_get_zero_initialized_client();
  const rosidl_service_type_support_t * type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  rc = rclc_client_init_best_effort(&client, &node, type_support, topic_name);
  EXPECT_EQ(RCL_RET_OK, rc);

  // test client topic name
  EXPECT_EQ(strcmp(rcl_client_get_service_name(&client), expected_topic_name), 0);

  // Initialize the client request.
  test_msgs__srv__BasicTypes_Request req;
  test_msgs__srv__BasicTypes_Request__init(&req);
  req.uint8_value = 1;
  req.uint32_value = 2;

  // Check that there were no errors while sending the request.
  int64_t sequence_number = 0;
  rc = rcl_send_request(&client, &req, &sequence_number);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  test_msgs__srv__BasicTypes_Request__fini(&req);

  // check for qos-option best effort
  const rcl_client_options_t * cli_options = rcl_client_get_options(&client);
  EXPECT_EQ(cli_options->qos.reliability, RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT);

  // tests with invalid arguments
  rc = rclc_client_init_best_effort(nullptr, &node, type_support, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init_best_effort(&client, nullptr, type_support, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init_best_effort(&client, &node, nullptr, topic_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init_best_effort(&client, &node, type_support, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // clean up
  rc = rcl_client_fini(&client, &node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}

TEST(Test, rclc_client_init_qos) {
  rclc_support_t support;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  const char * my_name = "test_client_node_be";
  const char * my_namespace = "";
  const char * topic_name = "add_two_ints";
  const char * expected_topic_name = "/add_two_ints";
  rcl_node_t node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&node, my_name, my_namespace, &support);

  // test with valid arguments
  rcl_client_t client = rcl_get_zero_initialized_client();
  const rmw_qos_profile_t * qos_profile = &rmw_qos_profile_default;
  const rosidl_service_type_support_t * type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  rc = rclc_client_init(&client, &node, type_support, topic_name, qos_profile);
  EXPECT_EQ(RCL_RET_OK, rc);

  // test client topic name
  EXPECT_EQ(strcmp(rcl_client_get_service_name(&client), expected_topic_name), 0);

  // Initialize the client request.
  test_msgs__srv__BasicTypes_Request req;
  test_msgs__srv__BasicTypes_Request__init(&req);
  req.uint8_value = 1;
  req.uint32_value = 2;

  // Check that there were no errors while sending the request.
  int64_t sequence_number = 0;
  rc = rcl_send_request(&client, &req, &sequence_number);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  test_msgs__srv__BasicTypes_Request__fini(&req);

  // check for qos-option best effort
  const rcl_client_options_t * cli_options = rcl_client_get_options(&client);
  EXPECT_EQ(cli_options->qos.reliability, rmw_qos_profile_default.reliability);

  // tests with invalid arguments
  rc = rclc_client_init(nullptr, &node, type_support, topic_name, qos_profile);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init(&client, nullptr, type_support, topic_name, qos_profile);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init(&client, &node, nullptr, topic_name, qos_profile);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init(&client, &node, type_support, nullptr, qos_profile);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_client_init(&client, &node, type_support, topic_name, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // clean up
  rc = rcl_client_fini(&client, &node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}
