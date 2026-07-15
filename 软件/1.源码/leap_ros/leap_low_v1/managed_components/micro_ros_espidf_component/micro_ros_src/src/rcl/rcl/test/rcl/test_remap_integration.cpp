// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#include "rcl/rcl.h"
#include "rcl/remap.h"
#include "rcl/error_handling.h"

#include "test_msgs/msg/basic_types.h"
#include "test_msgs/srv/basic_types.h"

#include "./arg_macros.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestRemapIntegrationFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
  }

  void TearDown()
  {
  }
};

TEST_F(CLASSNAME(TestRemapIntegrationFixture, RMW_IMPLEMENTATION), remap_using_global_rule) {
  int argc;
  char ** argv;
  SCOPE_GLOBAL_ARGS(
    argc, argv,
    "process_name",
    "--ros-args",
    "-r", "__node:=new_name",
    "-r", "__ns:=/new_ns",
    "-r", "/foo/bar:=/bar/foo");

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_node_options_t default_options = rcl_node_get_default_options();
  ASSERT_EQ(
    RCL_RET_OK,
    rcl_node_init(&node, "original_name", "/original_ns", &context, &default_options));

  {  // Node name gets remapped
    EXPECT_STREQ("new_name", rcl_node_get_name(&node));
  }
  {  // Node namespace gets remapped
    EXPECT_STREQ("/new_ns", rcl_node_get_namespace(&node));
  }
  {  // Logger name gets remapped
    EXPECT_STREQ("new_ns.new_name", rcl_node_get_logger_name(&node));
  }
  {  // Publisher topic gets remapped
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
    rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
    rcl_ret_t ret = rcl_publisher_init(&publisher, &node, ts, "/foo/bar", &publisher_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/bar/foo", rcl_publisher_get_topic_name(&publisher));
    EXPECT_EQ(RCL_RET_OK, rcl_publisher_fini(&publisher, &node));
  }
  {  // Subscription topic gets remapped
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
    rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
    rcl_ret_t ret = rcl_subscription_init(
      &subscription, &node, ts, "/foo/bar", &subscription_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/bar/foo", rcl_subscription_get_topic_name(&subscription));
    EXPECT_EQ(RCL_RET_OK, rcl_subscription_fini(&subscription, &node));
  }
  {  // Client service name gets remapped
    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    rcl_client_options_t client_options = rcl_client_get_default_options();
    rcl_client_t client = rcl_get_zero_initialized_client();
    rcl_ret_t ret = rcl_client_init(&client, &node, ts, "/foo/bar", &client_options);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/bar/foo", rcl_client_get_service_name(&client));
    EXPECT_EQ(RCL_RET_OK, rcl_client_fini(&client, &node));
  }
  {  // Server service name gets remapped
    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    rcl_service_options_t service_options = rcl_service_get_default_options();
    rcl_service_t service = rcl_get_zero_initialized_service();
    rcl_ret_t ret = rcl_service_init(&service, &node, ts, "/foo/bar", &service_options);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/bar/foo", rcl_service_get_service_name(&service));
    EXPECT_EQ(RCL_RET_OK, rcl_service_fini(&service, &node));
  }

  EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node));
}

TEST_F(CLASSNAME(TestRemapIntegrationFixture, RMW_IMPLEMENTATION), ignore_global_rules) {
  int argc;
  char ** argv;
  SCOPE_GLOBAL_ARGS(
    argc, argv,
    "process_name",
    "--ros-args",
    "-r", "__node:=new_name",
    "-r", "__ns:=/new_ns",
    "-r", "/foo/bar:=/bar/foo");
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "local_process_name");

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_node_options_t options = rcl_node_get_default_options();
  options.use_global_arguments = false;
  options.arguments = local_arguments;
  ASSERT_EQ(RCL_RET_OK, rcl_node_init(&node, "original_name", "/original_ns", &context, &options));

  {  // Node name does not get remapped
    EXPECT_STREQ("original_name", rcl_node_get_name(&node));
  }
  {  // Node namespace does not get remapped
    EXPECT_STREQ("/original_ns", rcl_node_get_namespace(&node));
  }
  {  // Logger name gets remapped
    EXPECT_STREQ("original_ns.original_name", rcl_node_get_logger_name(&node));
  }
  {  // Publisher topic does not get remapped
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
    rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
    rcl_ret_t ret = rcl_publisher_init(&publisher, &node, ts, "/foo/bar", &publisher_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/foo/bar", rcl_publisher_get_topic_name(&publisher));
    EXPECT_EQ(RCL_RET_OK, rcl_publisher_fini(&publisher, &node));
  }
  {  // Subscription topic does not get remapped
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
    rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
    rcl_ret_t ret = rcl_subscription_init(
      &subscription, &node, ts, "/foo/bar", &subscription_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/foo/bar", rcl_subscription_get_topic_name(&subscription));
    EXPECT_EQ(RCL_RET_OK, rcl_subscription_fini(&subscription, &node));
  }
  {  // Client service name does not get remapped
    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    rcl_client_options_t client_options = rcl_client_get_default_options();
    rcl_client_t client = rcl_get_zero_initialized_client();
    rcl_ret_t ret = rcl_client_init(&client, &node, ts, "/foo/bar", &client_options);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/foo/bar", rcl_client_get_service_name(&client));
    EXPECT_EQ(RCL_RET_OK, rcl_client_fini(&client, &node));
  }
  {  // Server service name does not get remapped
    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    rcl_service_options_t service_options = rcl_service_get_default_options();
    rcl_service_t service = rcl_get_zero_initialized_service();
    rcl_ret_t ret = rcl_service_init(&service, &node, ts, "/foo/bar", &service_options);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/foo/bar", rcl_service_get_service_name(&service));
    EXPECT_EQ(RCL_RET_OK, rcl_service_fini(&service, &node));
  }

  EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node));
}

TEST_F(CLASSNAME(TestRemapIntegrationFixture, RMW_IMPLEMENTATION), local_rules_before_global) {
  int argc;
  char ** argv;
  SCOPE_GLOBAL_ARGS(
    argc, argv,
    "process_name",
    "--ros-args",
    "-r", "__node:=global_name",
    "-r", "__ns:=/global_ns",
    "-r", "/foo/bar:=/bar/global");
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(
    local_arguments,
    "process_name",
    "--ros-args",
    "-r", "__node:=local_name",
    "-r", "__ns:=/local_ns",
    "-r", "/foo/bar:=/bar/local");

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_node_options_t options = rcl_node_get_default_options();
  options.arguments = local_arguments;
  ASSERT_EQ(RCL_RET_OK, rcl_node_init(&node, "original_name", "/original_ns", &context, &options));

  {  // Node name
    EXPECT_STREQ("local_name", rcl_node_get_name(&node));
  }
  {  // Node namespace
    EXPECT_STREQ("/local_ns", rcl_node_get_namespace(&node));
  }
  {  // Logger name
    EXPECT_STREQ("local_ns.local_name", rcl_node_get_logger_name(&node));
  }
  {  // Publisher topic
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
    rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
    rcl_ret_t ret = rcl_publisher_init(&publisher, &node, ts, "/foo/bar", &publisher_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/bar/local", rcl_publisher_get_topic_name(&publisher));
    EXPECT_EQ(RCL_RET_OK, rcl_publisher_fini(&publisher, &node));
  }
  {  // Subscription topic
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
    rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
    rcl_ret_t ret = rcl_subscription_init(
      &subscription, &node, ts, "/foo/bar", &subscription_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/bar/local", rcl_subscription_get_topic_name(&subscription));
    EXPECT_EQ(RCL_RET_OK, rcl_subscription_fini(&subscription, &node));
  }
  {  // Client service name
    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    rcl_client_options_t client_options = rcl_client_get_default_options();
    rcl_client_t client = rcl_get_zero_initialized_client();
    rcl_ret_t ret = rcl_client_init(&client, &node, ts, "/foo/bar", &client_options);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/bar/local", rcl_client_get_service_name(&client));
    EXPECT_EQ(RCL_RET_OK, rcl_client_fini(&client, &node));
  }
  {  // Server service name
    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    rcl_service_options_t service_options = rcl_service_get_default_options();
    rcl_service_t service = rcl_get_zero_initialized_service();
    rcl_ret_t ret = rcl_service_init(&service, &node, ts, "/foo/bar", &service_options);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/bar/local", rcl_service_get_service_name(&service));
    EXPECT_EQ(RCL_RET_OK, rcl_service_fini(&service, &node));
  }

  EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node));
}

TEST_F(CLASSNAME(TestRemapIntegrationFixture, RMW_IMPLEMENTATION), remap_relative_topic) {
  int argc;
  char ** argv;
  SCOPE_GLOBAL_ARGS(argc, argv, "process_name", "--ros-args", "-r", "/foo/bar:=remap/global");

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_node_options_t default_options = rcl_node_get_default_options();
  ASSERT_EQ(RCL_RET_OK, rcl_node_init(&node, "original_name", "/foo", &context, &default_options));

  {  // Publisher topic
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
    rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
    rcl_ret_t ret = rcl_publisher_init(&publisher, &node, ts, "bar", &publisher_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/foo/remap/global", rcl_publisher_get_topic_name(&publisher));
    EXPECT_EQ(RCL_RET_OK, rcl_publisher_fini(&publisher, &node));
  }
  {  // Subscription topic
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
    rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
    rcl_ret_t ret = rcl_subscription_init(
      &subscription, &node, ts, "bar", &subscription_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/foo/remap/global", rcl_subscription_get_topic_name(&subscription));
    EXPECT_EQ(RCL_RET_OK, rcl_subscription_fini(&subscription, &node));
  }
  {  // Client service name
    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    rcl_client_options_t client_options = rcl_client_get_default_options();
    rcl_client_t client = rcl_get_zero_initialized_client();
    rcl_ret_t ret = rcl_client_init(&client, &node, ts, "bar", &client_options);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/foo/remap/global", rcl_client_get_service_name(&client));
    EXPECT_EQ(RCL_RET_OK, rcl_client_fini(&client, &node));
  }
  {  // Server service name
    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    rcl_service_options_t service_options = rcl_service_get_default_options();
    rcl_service_t service = rcl_get_zero_initialized_service();
    rcl_ret_t ret = rcl_service_init(&service, &node, ts, "bar", &service_options);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_STREQ("/foo/remap/global", rcl_service_get_service_name(&service));
    EXPECT_EQ(RCL_RET_OK, rcl_service_fini(&service, &node));
  }

  EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node));
}

TEST_F(CLASSNAME(TestRemapIntegrationFixture, RMW_IMPLEMENTATION), remap_using_node_rules) {
  int argc;
  char ** argv;
  SCOPE_GLOBAL_ARGS(
    argc, argv, "process_name", "--ros-args", "-r", "original_name:__ns:=/new_ns");

  rcl_node_t node = rcl_get_zero_initialized_node();
  rcl_node_options_t default_options = rcl_node_get_default_options();
  ASSERT_EQ(RCL_RET_OK, rcl_node_init(&node, "original_name", "", &context, &default_options));

  {  // Node namespace gets remapped
    EXPECT_STREQ("/new_ns", rcl_node_get_namespace(&node));
  }
  EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node));
}
