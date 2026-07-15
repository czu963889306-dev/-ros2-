// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include <chrono>
#include <thread>

#include "osrf_testing_tools_cpp/memory_tools/gtest_quickstart.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcutils/allocator.h"
#include "rcutils/strdup.h"

#include "rmw/rmw.h"
#include "rmw/error_handling.h"
#include "rmw/subscription_content_filter_options.h"

#include "test_msgs/msg/basic_types.h"
#include "test_msgs/msg/strings.h"
#include "./config.hpp"
#include "./testing_macros.hpp"

#include "rmw_dds_common/gid_utils.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using rmw_dds_common::operator==;

class CLASSNAME (TestSubscription, RMW_IMPLEMENTATION) : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rmw_ret_t ret = rmw_init_options_init(&init_options, rcutils_get_default_allocator());
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    init_options.enclave = rcutils_strdup("/", rcutils_get_default_allocator());
    ASSERT_STREQ("/", init_options.enclave);
    ret = rmw_init(&init_options, &context);
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    constexpr char node_name[] = "my_test_node";
    constexpr char node_namespace[] = "/my_test_ns";
    node = rmw_create_node(&context, node_name, node_namespace);
    ASSERT_NE(nullptr, node) << rcutils_get_error_string().str;
  }

  void TearDown() override
  {
    rmw_ret_t ret = rmw_destroy_node(node);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_shutdown(&context);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_context_fini(&context);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_init_options_fini(&init_options);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  }

  rmw_init_options_t init_options{rmw_get_zero_initialized_init_options()};
  rmw_context_t context{rmw_get_zero_initialized_context()};
  rmw_node_t * node{nullptr};
};

TEST_F(CLASSNAME(TestSubscription, RMW_IMPLEMENTATION), create_and_destroy) {
  rmw_subscription_options_t options = rmw_get_default_subscription_options();
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_subscription_t * sub =
    rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_default, &options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_subscription(node, sub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestSubscription, RMW_IMPLEMENTATION), create_and_destroy_native) {
  rmw_subscription_options_t options = rmw_get_default_subscription_options();
  constexpr char topic_name[] = "test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_qos_profile_t native_qos_profile = rmw_qos_profile_default;
  native_qos_profile.avoid_ros_namespace_conventions = true;
  rmw_subscription_t * sub =
    rmw_create_subscription(node, ts, topic_name, &native_qos_profile, &options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_subscription(node, sub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestSubscription, RMW_IMPLEMENTATION), create_with_bad_arguments) {
  rmw_subscription_options_t options = rmw_get_default_subscription_options();
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_subscription_t * sub =
    rmw_create_subscription(nullptr, ts, topic_name, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  sub = rmw_create_subscription(node, nullptr, topic_name, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  sub = rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_default, &options);
  node->implementation_identifier = implementation_identifier;
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  sub = rmw_create_subscription(node, ts, nullptr, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  sub = rmw_create_subscription(node, ts, "", &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  constexpr char topic_name_with_spaces[] = "/foo bar";
  sub =
    rmw_create_subscription(node, ts, topic_name_with_spaces, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  constexpr char relative_topic_name[] = "foo";
  sub = rmw_create_subscription(node, ts, relative_topic_name, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  sub = rmw_create_subscription(node, ts, topic_name, nullptr, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  sub = rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_unknown, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  sub = rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_default, nullptr);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();

  rosidl_message_type_support_t * non_const_ts =
    const_cast<rosidl_message_type_support_t *>(ts);
  const char * typesupport_identifier = non_const_ts->typesupport_identifier;
  non_const_ts->typesupport_identifier = "not-a-typesupport-identifier";
  sub = rmw_create_subscription(
    node, non_const_ts, topic_name,
    &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, sub);
  rmw_reset_error();
  non_const_ts->typesupport_identifier = typesupport_identifier;

  // Creating and destroying a subscription still succeeds.
  sub = rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_default, &options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_subscription(node, sub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestSubscription, RMW_IMPLEMENTATION), create_with_internal_errors) {
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rmw_subscription_t * sub = nullptr;
    rmw_subscription_options_t options = rmw_get_default_subscription_options();
    sub = rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_default, &options);
    if (sub) {
      RCUTILS_NO_FAULT_INJECTION(
      {
        rmw_ret_t ret = rmw_destroy_subscription(node, sub);
        EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
      });
    } else {
      rmw_reset_error();
    }
  });
}

TEST_F(CLASSNAME(TestSubscription, RMW_IMPLEMENTATION), destroy_with_bad_arguments) {
  rmw_subscription_options_t options = rmw_get_default_subscription_options();
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_subscription_t * sub =
    rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_default, &options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;

  // Destroying subscription with invalid arguments fails.
  rmw_ret_t ret = rmw_destroy_subscription(nullptr, sub);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  ret = rmw_destroy_subscription(node, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_destroy_subscription(node, sub);
  node->implementation_identifier = implementation_identifier;
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  rmw_reset_error();

  // Destroying subscription still succeeds.
  ret = rmw_destroy_subscription(node, sub);
  EXPECT_EQ(RMW_RET_OK, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestSubscription, RMW_IMPLEMENTATION), destroy_with_internal_errors) {
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rmw_subscription_t * sub = nullptr;
    RCUTILS_NO_FAULT_INJECTION(
    {
      rmw_subscription_options_t options = rmw_get_default_subscription_options();
      sub = rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_default, &options);
      ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;
    });
    if (RMW_RET_OK != rmw_destroy_subscription(node, sub)) {
      rmw_reset_error();
    }
  });
}

TEST_F(CLASSNAME(TestSubscription, RMW_IMPLEMENTATION), get_actual_qos_from_system_defaults) {
  rmw_subscription_options_t options = rmw_get_default_subscription_options();
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_subscription_t * sub =
    rmw_create_subscription(node, ts, topic_name, &rmw_qos_profile_system_default, &options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;
  rmw_qos_profile_t qos_profile = rmw_qos_profile_unknown;
  rmw_ret_t ret = rmw_subscription_get_actual_qos(sub, &qos_profile);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  // Check that a valid QoS policy has been put in place for each system default one.
  EXPECT_NE(rmw_qos_profile_system_default.history, qos_profile.history);
  EXPECT_NE(rmw_qos_profile_unknown.history, qos_profile.history);
  EXPECT_NE(rmw_qos_profile_system_default.reliability, qos_profile.reliability);
  EXPECT_NE(rmw_qos_profile_unknown.reliability, qos_profile.reliability);
  EXPECT_NE(rmw_qos_profile_system_default.durability, qos_profile.durability);
  EXPECT_NE(rmw_qos_profile_unknown.durability, qos_profile.durability);
  EXPECT_NE(rmw_qos_profile_system_default.liveliness, qos_profile.liveliness);
  EXPECT_NE(rmw_qos_profile_unknown.liveliness, qos_profile.liveliness);
  EXPECT_NE(rmw_qos_profile_system_default.liveliness, qos_profile.liveliness);
  EXPECT_NE(rmw_qos_profile_unknown.liveliness, qos_profile.liveliness);
  ret = rmw_destroy_subscription(node, sub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

class CLASSNAME (TestSubscriptionUse, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestSubscription, RMW_IMPLEMENTATION)
{
protected:
  using Base = CLASSNAME(TestSubscription, RMW_IMPLEMENTATION);

  void SetUp() override
  {
    Base::SetUp();
    // Tighten QoS policies to force mismatch.
    qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
    rmw_subscription_options_t options = rmw_get_default_subscription_options();
    sub = rmw_create_subscription(node, ts, topic_name, &qos_profile, &options);
    ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;
  }

  void TearDown() override
  {
    rmw_ret_t ret = rmw_destroy_subscription(node, sub);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    Base::TearDown();
  }

  rmw_subscription_t * sub{nullptr};
  const char * const topic_name = "/test";
  const rosidl_message_type_support_t * ts{
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes)};
  rmw_qos_profile_t qos_profile{rmw_qos_profile_default};
};

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), get_actual_qos_with_bad_arguments) {
  rmw_qos_profile_t actual_qos_profile = rmw_qos_profile_unknown;
  rmw_ret_t ret = rmw_subscription_get_actual_qos(nullptr, &actual_qos_profile);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  ret = rmw_subscription_get_actual_qos(sub, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_subscription_get_actual_qos(sub, &actual_qos_profile);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), get_actual_qos) {
  rmw_qos_profile_t actual_qos_profile = rmw_qos_profile_unknown;
  rmw_ret_t ret = rmw_subscription_get_actual_qos(sub, &actual_qos_profile);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(qos_profile.history, actual_qos_profile.history);
  EXPECT_EQ(qos_profile.depth, actual_qos_profile.depth);
  EXPECT_EQ(qos_profile.reliability, actual_qos_profile.reliability);
  EXPECT_EQ(qos_profile.durability, actual_qos_profile.durability);
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), count_matched_publishers_with_bad_args) {
  size_t publisher_count = 0u;
  rmw_ret_t ret = rmw_subscription_count_matched_publishers(nullptr, &publisher_count);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  ret = rmw_subscription_count_matched_publishers(sub, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
  sub->implementation_identifier = implementation_identifier;
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), count_matched_subscriptions) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest sqg;

  rmw_ret_t ret;
  size_t publisher_count = 0u;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, publisher_count);

  rmw_publisher_options_t options = rmw_get_default_publisher_options();
  rmw_publisher_t * pub = rmw_create_publisher(node, ts, topic_name, &qos_profile, &options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;

  // TODO(hidmic): revisit when https://github.com/ros2/rmw/issues/264 is resolved.
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
    if (RMW_RET_OK == ret && 1u == publisher_count) {
      break;
    }
  }

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(1u, publisher_count);

  ret = rmw_destroy_publisher(node, pub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // TODO(hidmic): revisit when https://github.com/ros2/rmw/issues/264 is resolved.
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
    if (RMW_RET_OK == ret && 0u == publisher_count) {
      break;
    }
  }

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, publisher_count);
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), count_mismatched_subscriptions) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest sqg;

  rmw_ret_t ret;
  size_t publisher_count = 0u;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, publisher_count);

  // Relax QoS policies to force mismatch.
  rmw_qos_profile_t other_qos_profile = qos_profile;
  other_qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  rmw_publisher_options_t options = rmw_get_default_publisher_options();
  rmw_publisher_t * pub =
    rmw_create_publisher(node, ts, topic_name, &other_qos_profile, &options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;

  // TODO(hidmic): revisit when https://github.com/ros2/rmw/issues/264 is resolved.
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
    if (RMW_RET_OK == ret && 0u != publisher_count) {  // Early return on failure.
      break;
    }
  }

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, publisher_count);

  ret = rmw_destroy_publisher(node, pub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_subscription_count_matched_publishers(sub, &publisher_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, publisher_count);
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), take_with_bad_args) {
  bool taken = false;
  test_msgs__msg__BasicTypes output_message{};
  output_message.bool_value = true;
  output_message.char_value = 'a';
  output_message.float32_value = 0.42f;
  test_msgs__msg__BasicTypes original_message = output_message;
  rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation

  rmw_ret_t ret = rmw_take(nullptr, &output_message, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(taken, false);
  EXPECT_EQ(output_message, original_message);
  rmw_reset_error();

  ret = rmw_take(sub, nullptr, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(taken, false);
  rmw_reset_error();

  ret = rmw_take(sub, &output_message, nullptr, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(output_message, original_message);
  rmw_reset_error();

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_take(sub, &output_message, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  EXPECT_EQ(taken, false);
  EXPECT_EQ(output_message, original_message);
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), take_with_info_with_bad_args) {
  bool taken = false;
  test_msgs__msg__BasicTypes output_message{};
  output_message.bool_value = true;
  output_message.char_value = 'a';
  output_message.float32_value = 0.42f;
  test_msgs__msg__BasicTypes original_message = output_message;
  rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
  rmw_message_info_t original_info = rmw_get_zero_initialized_message_info();
  rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation

  rmw_ret_t ret = rmw_take_with_info(
    nullptr, &output_message, &taken, &message_info,
    null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(output_message, original_message);
  EXPECT_EQ(taken, false);
  EXPECT_EQ(message_info, original_info);
  rmw_reset_error();

  ret = rmw_take_with_info(sub, nullptr, &taken, &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(message_info, original_info);
  EXPECT_EQ(taken, false);
  rmw_reset_error();

  ret = rmw_take_with_info(sub, &output_message, nullptr, &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(output_message, original_message);
  EXPECT_EQ(message_info, original_info);
  rmw_reset_error();

  ret = rmw_take_with_info(sub, &output_message, &taken, nullptr, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(output_message, original_message);
  EXPECT_EQ(taken, false);
  rmw_reset_error();

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_take_with_info(sub, &output_message, &taken, &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  EXPECT_EQ(output_message, original_message);
  EXPECT_EQ(taken, false);
  EXPECT_EQ(message_info, original_info);
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), ignore_local_publications) {
  rmw_ret_t ret;
  bool taken = false;

  // Create publisher
  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_publisher_t * pub = rmw_create_publisher(node, ts, topic_name, &qos_profile, &pub_options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_destroy_publisher(node, pub)) << rmw_get_error_string().str;
  });

  // Create subscription with ignore_local_publications = true
  rmw_subscription_options_t sub_options_ignorelocal = rmw_get_default_subscription_options();
  sub_options_ignorelocal.ignore_local_publications = true;
  rmw_subscription_t * sub_ignorelocal =
    rmw_create_subscription(node, ts, topic_name, &qos_profile, &sub_options_ignorelocal);
  ASSERT_NE(nullptr, sub_ignorelocal) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_destroy_subscription(node, sub_ignorelocal)) << rmw_get_error_string().str;
  });

  size_t subscription_count = 0u;
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 2u == subscription_count) {  // Early return on failure.
      break;
    }
  }

  // Roundtrip message from publisher to both subscriptions
  test_msgs__msg__BasicTypes original_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&original_message));
  original_message.bool_value = true;
  original_message.char_value = 'k';
  original_message.float32_value = 3.14159f;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BasicTypes__fini(&original_message);
  });

  rmw_publisher_allocation_t * null_allocation_p{nullptr};
  rmw_subscription_allocation_t * null_allocation_s{nullptr};

  ret = rmw_publish(pub, &original_message, null_allocation_p);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  rmw_subscriptions_t subscriptions;
  void * subscriptions_storage[2];
  subscriptions_storage[0] = sub_ignorelocal->data;
  subscriptions_storage[1] = sub->data;
  subscriptions.subscribers = subscriptions_storage;
  subscriptions.subscriber_count = 2;

  rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 2);
  ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
  });
  rmw_time_t timeout = {1, 0};  // 1000ms
  ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  // Subscriptions that ignore local publications may or may not be awaken by locally sent messages.
  // ASSERT_NE(nullptr, subscriptions.subscribers[0]);
  ASSERT_NE(nullptr, subscriptions.subscribers[1]);

  // ignore_local_publications = true
  {
    test_msgs__msg__BasicTypes output_message{};
    ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&output_message);
    });

    ret = rmw_take(sub_ignorelocal, &output_message, &taken, null_allocation_s);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    EXPECT_FALSE(taken);
  }

  // ignore_local_publications = false
  {
    test_msgs__msg__BasicTypes output_message{};
    ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&output_message);
    });

    ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    EXPECT_TRUE(taken);
    EXPECT_EQ(original_message, output_message);
  }
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), ignore_local_publications_serialized) {
  rmw_ret_t ret;

  // Create publisher
  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_publisher_t * pub = rmw_create_publisher(node, ts, topic_name, &qos_profile, &pub_options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_destroy_publisher(node, pub)) << rmw_get_error_string().str;
  });

  // Create subscription with ignore_local_publications = true
  rmw_subscription_options_t sub_options_ignorelocal = rmw_get_default_subscription_options();
  sub_options_ignorelocal.ignore_local_publications = true;
  rmw_subscription_t * sub_ignorelocal =
    rmw_create_subscription(node, ts, topic_name, &qos_profile, &sub_options_ignorelocal);
  ASSERT_NE(nullptr, sub_ignorelocal) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_destroy_subscription(node, sub_ignorelocal)) << rmw_get_error_string().str;
  });

  size_t subscription_count = 0u;
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 2u == subscription_count) {  // Early return on failure.
      break;
    }
  }

  // Roundtrip message from publisher to both subscriptions
  test_msgs__msg__BasicTypes original_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&original_message));
  original_message.bool_value = true;
  original_message.char_value = 'k';
  original_message.float32_value = 3.14159f;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BasicTypes__fini(&original_message);
  });

  rmw_publisher_allocation_t * null_allocation_p{nullptr};
  rmw_subscription_allocation_t * null_allocation_s{nullptr};

  ret = rmw_publish(pub, &original_message, null_allocation_p);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  rmw_subscriptions_t subscriptions;
  void * subscriptions_storage[2];
  subscriptions_storage[0] = sub_ignorelocal->data;
  subscriptions_storage[1] = sub->data;
  subscriptions.subscribers = subscriptions_storage;
  subscriptions.subscriber_count = 2;

  rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 2);
  ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
  });
  rmw_time_t timeout = {1, 0};  // 1000ms
  ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ASSERT_NE(nullptr, subscriptions.subscribers[1]);

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  // ignore_local_publications = true
  {
    bool taken_serialized = false;
    rmw_serialized_message_t msg = rmw_get_zero_initialized_serialized_message();
    ASSERT_EQ(
      RMW_RET_OK, rmw_serialized_message_init(
        &msg, 0, &allocator)) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_serialized_message_fini(&msg)) << rmw_get_error_string().str;
    });

    ret = rmw_take_serialized_message(sub_ignorelocal, &msg, &taken_serialized, null_allocation_s);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    EXPECT_FALSE(taken_serialized);
  }

  // ignore_local_publications = false
  {
    bool taken_serialized = false;
    rmw_serialized_message_t msg = rmw_get_zero_initialized_serialized_message();
    ASSERT_EQ(
      RMW_RET_OK, rmw_serialized_message_init(
        &msg, 0, &allocator)) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_serialized_message_fini(&msg)) << rmw_get_error_string().str;
    });

    ret = rmw_take_serialized_message(sub, &msg, &taken_serialized, null_allocation_s);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    EXPECT_TRUE(taken_serialized);
    // Optionally, could deserialize and compare, but not needed for this test.
  }
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), take_sequence) {
  size_t count = 1u;
  size_t taken = 10u;  // Non-zero value to check variable update
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_message_sequence_t sequence = rmw_get_zero_initialized_message_sequence();
  rmw_ret_t ret = rmw_message_sequence_init(&sequence, count, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  auto seq = test_msgs__msg__Strings__Sequence__create(count);
  for (size_t ii = 0; ii < count; ++ii) {
    sequence.data[ii] = &seq->data[ii];
  }

  rmw_message_info_sequence_t info_sequence = rmw_get_zero_initialized_message_info_sequence();
  ret = rmw_message_info_sequence_init(&info_sequence, count, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation

  ret = rmw_take_sequence(sub, count, &sequence, &info_sequence, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(taken, 0u);

  ret = rmw_message_info_sequence_fini(&info_sequence);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  ret = rmw_message_sequence_fini(&sequence);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  test_msgs__msg__Strings__Sequence__destroy(seq);
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), take_sequence_with_bad_args) {
  size_t count = 1u;
  size_t taken = 0u;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_message_sequence_t sequence = rmw_get_zero_initialized_message_sequence();
  rmw_message_sequence_t original_sequence = sequence;
  rmw_ret_t ret = rmw_message_sequence_init(&sequence, count, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_message_sequence_init(&original_sequence, count, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  rmw_message_info_sequence_t info_sequence = rmw_get_zero_initialized_message_info_sequence();
  rmw_message_info_sequence_t original_info = info_sequence;
  ret = rmw_message_info_sequence_init(&info_sequence, count, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_message_info_sequence_init(&original_info, count, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation

  ret = rmw_take_sequence(nullptr, count, &sequence, &info_sequence, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(sequence.size, original_sequence.size);
  EXPECT_EQ(info_sequence.size, original_info.size);
  EXPECT_EQ(taken, 0u);
  rmw_reset_error();

  ret = rmw_take_sequence(sub, 0u, &sequence, &info_sequence, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(sequence.size, original_sequence.size);
  EXPECT_EQ(info_sequence.size, original_info.size);
  EXPECT_EQ(taken, 0u);
  rmw_reset_error();

  ret = rmw_take_sequence(sub, count, nullptr, &info_sequence, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(info_sequence.size, original_info.size);
  EXPECT_EQ(taken, 0u);
  rmw_reset_error();

  ret = rmw_take_sequence(sub, count, &sequence, nullptr, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(sequence.size, original_sequence.size);
  EXPECT_EQ(taken, 0u);
  rmw_reset_error();

  ret = rmw_take_sequence(sub, count, &sequence, &info_sequence, nullptr, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(sequence.size, original_sequence.size);
  EXPECT_EQ(info_sequence.size, original_info.size);
  rmw_reset_error();

  rmw_message_sequence_t sequence_zero_count = rmw_get_zero_initialized_message_sequence();
  rmw_message_sequence_t original_zero_count = sequence_zero_count;
  ret = rmw_message_sequence_init(&sequence_zero_count, 0u, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_message_sequence_init(&original_zero_count, 0u, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret =
    rmw_take_sequence(sub, count, &sequence_zero_count, &info_sequence, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(sequence_zero_count.size, original_zero_count.size);
  EXPECT_EQ(info_sequence.size, original_info.size);
  EXPECT_EQ(taken, 0u);
  rmw_reset_error();
  ret = rmw_message_sequence_fini(&sequence_zero_count);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_message_sequence_fini(&original_zero_count);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  rmw_message_info_sequence_t info_sequence_zero_count =
    rmw_get_zero_initialized_message_info_sequence();
  rmw_message_info_sequence_t original_info_zero_count = info_sequence_zero_count;
  ret = rmw_message_info_sequence_init(&info_sequence_zero_count, 0u, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_message_info_sequence_init(&original_info_zero_count, 0u, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret =
    rmw_take_sequence(sub, count, &sequence, &info_sequence_zero_count, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(sequence.size, original_sequence.size);
  EXPECT_EQ(info_sequence_zero_count.size, original_info_zero_count.size);
  EXPECT_EQ(taken, 0u);
  rmw_reset_error();
  ret = rmw_message_info_sequence_fini(&info_sequence_zero_count);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_message_info_sequence_fini(&original_info_zero_count);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_take_sequence(sub, count, &sequence, &info_sequence, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  EXPECT_EQ(sequence.size, original_sequence.size);
  EXPECT_EQ(info_sequence.size, original_info.size);
  EXPECT_EQ(taken, 0u);
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;

  ret = rmw_message_sequence_fini(&sequence);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  ret = rmw_message_sequence_fini(&original_sequence);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  ret = rmw_message_info_sequence_fini(&info_sequence);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  ret = rmw_message_info_sequence_fini(&original_info);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), take_serialized_with_bad_args) {
  rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  bool taken = false;
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  rmw_serialized_message_t original_message = serialized_message;
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 1lu, &default_allocator)) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) << rmw_get_error_string().str;
  });
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &original_message, 1lu, &default_allocator)) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_serialized_message_fini(&original_message)) << rmw_get_error_string().str;
  });

  rmw_ret_t ret =
    rmw_take_serialized_message(nullptr, &serialized_message, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(serialized_message.buffer_length, original_message.buffer_length);
  EXPECT_EQ(taken, false);
  rmw_reset_error();

  ret = rmw_take_serialized_message(sub, nullptr, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(taken, false);
  rmw_reset_error();

  ret = rmw_take_serialized_message(sub, &serialized_message, nullptr, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  EXPECT_EQ(serialized_message.buffer_length, original_message.buffer_length);
  rmw_reset_error();

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_take_serialized_message(sub, &serialized_message, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  EXPECT_EQ(serialized_message.buffer_length, original_message.buffer_length);
  EXPECT_EQ(taken, false);
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;

  EXPECT_EQ(
    RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) << rmw_get_error_string().str;
  EXPECT_EQ(
    RMW_RET_OK, rmw_serialized_message_fini(&original_message)) << rmw_get_error_string().str;
}

TEST_F(
  CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION),
  take_serialized_with_info_with_bad_args) {
  rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  bool taken = false;
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &default_allocator)) << rmw_get_error_string().str;

  rmw_ret_t ret = rmw_take_serialized_message_with_info(
    nullptr, &serialized_message, &taken,
    &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  ret = rmw_take_serialized_message_with_info(
    sub, nullptr, &taken, &message_info,
    null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  ret = rmw_take_serialized_message_with_info(
    sub, &serialized_message, nullptr, &message_info,
    null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  ret = rmw_take_serialized_message_with_info(
    sub, &serialized_message, &taken, nullptr,
    null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_take_serialized_message_with_info(
    sub, &serialized_message, &taken, &message_info,
    null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;

  EXPECT_EQ(
    RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), no_content_filter_get) {
  rmw_subscription_content_filter_options_t options;
  auto allocator = rcutils_get_default_allocator();
  rmw_ret_t ret = rmw_subscription_get_content_filter(sub, &allocator, &options);
  EXPECT_NE(RMW_RET_OK, ret);
}

TEST_F(CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION), no_content_filter_set) {
  rmw_ret_t ret;
  bool taken = false;

  // Create publisher
  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_publisher_t * pub = rmw_create_publisher(node, ts, topic_name, &qos_profile, &pub_options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_destroy_publisher(node, pub)) << rmw_get_error_string().str;
  });

  size_t subscription_count = 0u;
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 1u == subscription_count) {  // Early return on failure.
      break;
    }
  }

  // Publish message with float (3.14159) from publisher to subscription
  test_msgs__msg__BasicTypes original_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&original_message));
  original_message.float32_value = 3.14159f;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BasicTypes__fini(&original_message);
  });

  rmw_publisher_allocation_t * null_allocation_p{nullptr};
  rmw_subscription_allocation_t * null_allocation_s{nullptr};

  ret = rmw_publish(pub, &original_message, null_allocation_p);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // expect to get message because no content filter set
  {
    rmw_subscriptions_t subscriptions;
    void * subscriptions_storage[1];
    subscriptions_storage[0] = sub->data;
    subscriptions.subscribers = subscriptions_storage;
    subscriptions.subscriber_count = 1;

    rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
    ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
    });
    rmw_time_t timeout = {1, 0};  // 1000ms
    ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ASSERT_NE(nullptr, subscriptions.subscribers[0]);

    test_msgs__msg__BasicTypes output_message{};
    ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&output_message);
    });

    ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
    // whatever the cft is supported or not
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

    EXPECT_TRUE(taken);
    EXPECT_EQ(original_message, output_message);
  }

  // set content filter with 3.14
  rmw_subscription_content_filter_options_t options =
    rmw_get_zero_initialized_content_filter_options();
  auto allocator = rcutils_get_default_allocator();
  EXPECT_EQ(
    RMW_RET_OK, rmw_subscription_content_filter_options_init(
      "float32_value=3.14",
      0,
      nullptr,
      &allocator,
      &options));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK,
      rmw_subscription_content_filter_options_fini(&options, &allocator));
  });

  ret = rmw_subscription_set_content_filter(sub, &options);
  if (ret != RMW_RET_UNSUPPORTED) {
    ASSERT_EQ(RMW_RET_OK, ret);
    // waiting to allow for filter propagation
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ret = rmw_publish(pub, &original_message, null_allocation_p);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

    rmw_subscriptions_t subscriptions;
    void * subscriptions_storage[1];
    subscriptions_storage[0] = sub->data;
    subscriptions.subscribers = subscriptions_storage;
    subscriptions.subscriber_count = 1;

    rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
    ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
    });
    rmw_time_t timeout = {1, 0};  // 1000ms
    ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);
    EXPECT_EQ(RMW_RET_TIMEOUT, ret) << rmw_get_error_string().str;
    ASSERT_EQ(nullptr, subscriptions.subscribers[0]);

    // content filter subscription with 3.14 that is not equal with 3.14159
    {
      test_msgs__msg__BasicTypes output_message{};
      ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
      OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
      {
        test_msgs__msg__BasicTypes__fini(&output_message);
      });

      ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

      EXPECT_FALSE(taken);
    }
  }
}

class CLASSNAME (TestSubscriptionUseLoan, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION)
{
protected:
  using Base = CLASSNAME(TestSubscriptionUse, RMW_IMPLEMENTATION);

  void SetUp() override
  {
    Base::SetUp();
    // Check if loaning is supported by the implementation
    if (!sub->can_loan_messages) {
      bool taken = false;
      void * loaned_message = nullptr;
      rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
      rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation
      rmw_ret_t ret = rmw_take_loaned_message(sub, &loaned_message, &taken, null_allocation);
      EXPECT_EQ(RMW_RET_UNSUPPORTED, ret) << rmw_get_error_string().str;
      rmw_reset_error();
      EXPECT_EQ(nullptr, loaned_message);
      ret = rmw_take_loaned_message_with_info(
        sub, &loaned_message, &taken, &message_info, null_allocation);
      EXPECT_EQ(RMW_RET_UNSUPPORTED, ret) << rmw_get_error_string().str;
      rmw_reset_error();
      EXPECT_EQ(nullptr, loaned_message);
      ret = rmw_return_loaned_message_from_subscription(sub, loaned_message);
      EXPECT_EQ(RMW_RET_UNSUPPORTED, ret) << rmw_get_error_string().str;
      rmw_reset_error();
      EXPECT_EQ(nullptr, loaned_message);
      GTEST_SKIP();
    }
  }

  void TearDown() override
  {
    Base::TearDown();
  }
};

TEST_F(CLASSNAME(TestSubscriptionUseLoan, RMW_IMPLEMENTATION), rmw_take_loaned_message) {
  bool taken = false;
  void * loaned_message = nullptr;
  rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation
  rmw_ret_t ret = rmw_take_loaned_message(nullptr, &loaned_message, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  ret = rmw_take_loaned_message(sub, nullptr, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  ret = rmw_take_loaned_message(sub, &loaned_message, nullptr, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  ret = rmw_take_loaned_message(sub, &loaned_message, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_take_loaned_message(sub, &loaned_message, &taken, null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;
}

TEST_F(
  CLASSNAME(TestSubscriptionUseLoan, RMW_IMPLEMENTATION), rmw_take_loaned_message_with_info) {
  bool taken = false;
  void * loaned_message = nullptr;
  rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
  rmw_subscription_allocation_t * null_allocation{nullptr};  // still valid allocation
  rmw_ret_t ret = rmw_take_loaned_message_with_info(
    nullptr, &loaned_message, &taken, &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  ret = rmw_take_loaned_message_with_info(
    sub, nullptr, &taken, &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  ret = rmw_take_loaned_message_with_info(
    sub, &loaned_message, nullptr, &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  ret = rmw_take_loaned_message_with_info(
    sub, &loaned_message, &taken, nullptr, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  ret = rmw_take_loaned_message_with_info(
    sub, &loaned_message, &taken, &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(nullptr, loaned_message);
  EXPECT_FALSE(taken);

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_take_loaned_message_with_info(
    sub, &loaned_message, &taken, &message_info, null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;
}

TEST_F(
  CLASSNAME(TestSubscriptionUseLoan, RMW_IMPLEMENTATION),
  rmw_return_loaned_message_from_subscription) {
  test_msgs__msg__BasicTypes msg{};
  rmw_ret_t ret = rmw_return_loaned_message_from_subscription(nullptr, &msg);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  ret = rmw_return_loaned_message_from_subscription(sub, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  // Returning a sample that was not loaned
  ret = rmw_return_loaned_message_from_subscription(sub, &msg);
  EXPECT_EQ(RMW_RET_ERROR, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  const char * implementation_identifier = sub->implementation_identifier;
  sub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_return_loaned_message_from_subscription(sub, &msg);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  sub->implementation_identifier = implementation_identifier;
}

bool operator==(const test_msgs__msg__BasicTypes & m1, const test_msgs__msg__BasicTypes & m2)
{
  return m1.bool_value == m2.bool_value &&
         m1.byte_value == m2.byte_value &&
         m1.char_value == m2.char_value &&
         m1.float32_value == m2.float32_value &&
         m1.float64_value == m2.float64_value &&
         m1.int8_value == m2.int8_value &&
         m1.uint8_value == m2.uint8_value &&
         m1.int16_value == m2.int16_value &&
         m1.uint16_value == m2.uint16_value &&
         m1.int32_value == m2.int32_value &&
         m1.uint32_value == m2.uint32_value &&
         m1.int64_value == m2.int64_value &&
         m1.uint64_value == m2.uint64_value;
}

bool operator==(const rmw_message_info_t & m1, const rmw_message_info_t & m2)
{
  return m1.publisher_gid == m2.publisher_gid &&
         m1.source_timestamp == m2.source_timestamp &&
         m1.received_timestamp == m2.received_timestamp &&
         m1.from_intra_process == m2.from_intra_process;
}

class CLASSNAME (TestContentFilterSubscriptionUse, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestSubscription, RMW_IMPLEMENTATION)
{
protected:
  using Base = CLASSNAME(TestSubscription, RMW_IMPLEMENTATION);

  void SetUp() override
  {
    Base::SetUp();
    // Tighten QoS policies to force mismatch.
    qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
    rmw_subscription_options_t options = rmw_get_default_subscription_options();

    // Create subscription with content filter
    auto allocator = rcutils_get_default_allocator();

    rmw_subscription_content_filter_options_t * content_filter_options =
      static_cast<rmw_subscription_content_filter_options_t *>(
      allocator.allocate(
        sizeof(rmw_subscription_content_filter_options_t), allocator.state));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      allocator.deallocate(content_filter_options, allocator.state);
    });
    *content_filter_options = rmw_get_zero_initialized_content_filter_options();

    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_init(
        filter_expression,
        expression_parameters_count,
        expression_parameters,
        &allocator,
        content_filter_options));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK,
        rmw_subscription_content_filter_options_fini(content_filter_options, &allocator));
    });
    options.content_filter_options = content_filter_options;

    sub = rmw_create_subscription(node, ts, topic_name, &qos_profile, &options);
    ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;
  }

  void TearDown() override
  {
    rmw_ret_t ret = rmw_destroy_subscription(node, sub);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    Base::TearDown();
  }

  rmw_subscription_t * sub{nullptr};
  const char * const topic_name = "/test";
  const rosidl_message_type_support_t * ts{
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes)};
  rmw_qos_profile_t qos_profile{rmw_qos_profile_default};
  const char * filter_expression = "float32_value=%0";
  enum { expression_parameters_count = 1 };
  const char * expression_parameters[expression_parameters_count] = {
    "3.14159"
  };
  const char * expression_parameters_2[expression_parameters_count] = {
    "3.14"
  };
};

TEST_F(CLASSNAME(TestContentFilterSubscriptionUse, RMW_IMPLEMENTATION), get_content_filter) {
  rmw_subscription_content_filter_options_t options;
  auto allocator = rcutils_get_default_allocator();
  rmw_ret_t ret = rmw_subscription_get_content_filter(sub, &allocator, &options);
  if (sub->is_cft_enabled) {
    EXPECT_EQ(RMW_RET_OK, ret);
    EXPECT_STREQ(options.filter_expression, filter_expression);
    ASSERT_EQ(expression_parameters_count, options.expression_parameters.size);
    for (size_t i = 0; i < options.expression_parameters.size; ++i) {
      EXPECT_STREQ(
        options.expression_parameters.data[i],
        expression_parameters[i]);
    }
    EXPECT_EQ(
      RMW_RET_OK,
      rmw_subscription_content_filter_options_fini(&options, &allocator));
  } else {
    EXPECT_EQ(RMW_RET_UNSUPPORTED, ret);
  }
}

TEST_F(CLASSNAME(TestContentFilterSubscriptionUse, RMW_IMPLEMENTATION), set_content_filter) {
  rmw_subscription_content_filter_options_t options =
    rmw_get_zero_initialized_content_filter_options();
  auto allocator = rcutils_get_default_allocator();
  EXPECT_EQ(
    RMW_RET_OK, rmw_subscription_content_filter_options_init(
      filter_expression,
      expression_parameters_count,
      expression_parameters_2,
      &allocator,
      &options));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK,
      rmw_subscription_content_filter_options_fini(&options, &allocator));
  });

  bool is_cft_enabled = sub->is_cft_enabled;
  rmw_ret_t ret = rmw_subscription_set_content_filter(sub, &options);
  if (is_cft_enabled) {
    EXPECT_EQ(RMW_RET_OK, ret);
  } else {
    EXPECT_EQ(RMW_RET_UNSUPPORTED, ret);
  }
}

TEST_F(CLASSNAME(TestContentFilterSubscriptionUse, RMW_IMPLEMENTATION), content_filter_get_begin) {
  rmw_ret_t ret;
  bool taken = false;

  // Create publisher
  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_publisher_t * pub = rmw_create_publisher(node, ts, topic_name, &qos_profile, &pub_options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_destroy_publisher(node, pub)) << rmw_get_error_string().str;
  });

  bool is_cft_enabled = sub->is_cft_enabled;

  size_t subscription_count = 0u;
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 1u == subscription_count) {  // Early return on failure.
      break;
    }
  }

  // Publish message with float (3.14159) from publisher to subscription
  test_msgs__msg__BasicTypes original_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&original_message));
  original_message.float32_value = 3.14159f;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BasicTypes__fini(&original_message);
  });

  rmw_publisher_allocation_t * null_allocation_p{nullptr};
  rmw_subscription_allocation_t * null_allocation_s{nullptr};

  ret = rmw_publish(pub, &original_message, null_allocation_p);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // expect to get message because content filter is 3.14159
  {
    rmw_subscriptions_t subscriptions;
    void * subscriptions_storage[1];
    subscriptions_storage[0] = sub->data;
    subscriptions.subscribers = subscriptions_storage;
    subscriptions.subscriber_count = 1;

    rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
    ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
    });
    rmw_time_t timeout = {1, 0};  // 1000ms
    ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ASSERT_NE(nullptr, subscriptions.subscribers[0]);

    test_msgs__msg__BasicTypes output_message{};
    ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&output_message);
    });

    ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
    // whatever the cft is supported or not
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

    EXPECT_TRUE(taken);
    EXPECT_EQ(original_message, output_message);
  }

  // set content filter with 3.14
  if (is_cft_enabled) {
    rmw_subscription_content_filter_options_t options =
      rmw_get_zero_initialized_content_filter_options();
    auto allocator = rcutils_get_default_allocator();
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_init(
        filter_expression,
        expression_parameters_count,
        expression_parameters_2,
        &allocator,
        &options));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK,
        rmw_subscription_content_filter_options_fini(&options, &allocator));
    });

    EXPECT_EQ(
      RMW_RET_OK,
      rmw_subscription_set_content_filter(sub, &options));
    // waiting to allow for filter propagation
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ret = rmw_publish(pub, &original_message, null_allocation_p);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

    rmw_subscriptions_t subscriptions;
    void * subscriptions_storage[1];
    subscriptions_storage[0] = sub->data;
    subscriptions.subscribers = subscriptions_storage;
    subscriptions.subscriber_count = 1;

    rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
    ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
    });
    rmw_time_t timeout = {1, 0};  // 1000ms
    ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);
    EXPECT_EQ(RMW_RET_TIMEOUT, ret) << rmw_get_error_string().str;
    ASSERT_EQ(nullptr, subscriptions.subscribers[0]);

    // content filter subscription with 3.14 that is not equal with 3.14159
    {
      test_msgs__msg__BasicTypes output_message{};
      ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
      OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
      {
        test_msgs__msg__BasicTypes__fini(&output_message);
      });

      ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

      EXPECT_FALSE(taken);
    }
  }
}

TEST_F(CLASSNAME(TestContentFilterSubscriptionUse, RMW_IMPLEMENTATION), content_filter_get_later) {
  rmw_ret_t ret;
  bool taken = false;

  // Create publisher
  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_publisher_t * pub = rmw_create_publisher(node, ts, topic_name, &qos_profile, &pub_options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_destroy_publisher(node, pub)) << rmw_get_error_string().str;
  });

  bool is_cft_enabled = sub->is_cft_enabled;

  size_t subscription_count = 0u;
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 1u == subscription_count) {  // Early return on failure.
      break;
    }
  }

  // Publish message with float (3.14) from publisher to subscription
  test_msgs__msg__BasicTypes original_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&original_message));
  original_message.float32_value = 3.14f;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BasicTypes__fini(&original_message);
  });

  rmw_publisher_allocation_t * null_allocation_p{nullptr};
  rmw_subscription_allocation_t * null_allocation_s{nullptr};

  ret = rmw_publish(pub, &original_message, null_allocation_p);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  {
    rmw_subscriptions_t subscriptions;
    void * subscriptions_storage[1];
    subscriptions_storage[0] = sub->data;
    subscriptions.subscribers = subscriptions_storage;
    subscriptions.subscriber_count = 1;

    rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
    ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
    });
    rmw_time_t timeout = {1, 0};  // 1000ms
    ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);

    // if cft is supported, not to expect to get the message because content filter is 3.14159
    if (is_cft_enabled) {
      EXPECT_EQ(RMW_RET_TIMEOUT, ret) << rmw_get_error_string().str;
      ASSERT_EQ(nullptr, subscriptions.subscribers[0]);
    } else {
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
      ASSERT_NE(nullptr, subscriptions.subscribers[0]);
    }

    test_msgs__msg__BasicTypes output_message{};
    ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&output_message);
    });

    ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

    if (is_cft_enabled) {
      EXPECT_FALSE(taken);
    } else {
      EXPECT_TRUE(taken);
      EXPECT_EQ(original_message, output_message);
    }
  }

  // set content filter with 3.14
  if (is_cft_enabled) {
    rmw_subscription_content_filter_options_t options =
      rmw_get_zero_initialized_content_filter_options();
    auto allocator = rcutils_get_default_allocator();
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_init(
        filter_expression,
        expression_parameters_count,
        expression_parameters_2,
        &allocator,
        &options));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK,
        rmw_subscription_content_filter_options_fini(&options, &allocator));
    });

    EXPECT_EQ(
      RMW_RET_OK,
      rmw_subscription_set_content_filter(sub, &options));
    // waiting to allow for filter propagation
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ret = rmw_publish(pub, &original_message, null_allocation_p);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

    rmw_subscriptions_t subscriptions;
    void * subscriptions_storage[1];
    subscriptions_storage[0] = sub->data;
    subscriptions.subscribers = subscriptions_storage;
    subscriptions.subscriber_count = 1;

    rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
    ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
    });
    rmw_time_t timeout = {5, 0};  // 5000ms
    ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ASSERT_NE(nullptr, subscriptions.subscribers[0]);

    // content filter subscription with 3.14 that is equal with 3.14
    {
      test_msgs__msg__BasicTypes output_message{};
      ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
      OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
      {
        test_msgs__msg__BasicTypes__fini(&output_message);
      });

      ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

      EXPECT_TRUE(taken);
      EXPECT_EQ(original_message, output_message);
    }
  }
}

TEST_F(CLASSNAME(TestContentFilterSubscriptionUse, RMW_IMPLEMENTATION), content_filter_reset) {
  rmw_ret_t ret;
  bool taken = false;

  // Create publisher
  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_publisher_t * pub = rmw_create_publisher(node, ts, topic_name, &qos_profile, &pub_options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_destroy_publisher(node, pub)) << rmw_get_error_string().str;
  });

  bool is_cft_enabled = sub->is_cft_enabled;

  size_t subscription_count = 0u;
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 1u == subscription_count) {  // Early return on failure.
      break;
    }
  }

  // Publish message with float (3.14) from publisher to subscription
  test_msgs__msg__BasicTypes original_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&original_message));
  original_message.float32_value = 3.14f;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BasicTypes__fini(&original_message);
  });

  rmw_publisher_allocation_t * null_allocation_p{nullptr};
  rmw_subscription_allocation_t * null_allocation_s{nullptr};

  ret = rmw_publish(pub, &original_message, null_allocation_p);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  {
    rmw_subscriptions_t subscriptions;
    void * subscriptions_storage[1];
    subscriptions_storage[0] = sub->data;
    subscriptions.subscribers = subscriptions_storage;
    subscriptions.subscriber_count = 1;

    rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
    ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
    });
    rmw_time_t timeout = {1, 0};  // 1000ms
    ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);

    // if cft is supported, not to expect to get the message because content filter is 3.14159
    if (is_cft_enabled) {
      EXPECT_EQ(RMW_RET_TIMEOUT, ret) << rmw_get_error_string().str;
      ASSERT_EQ(nullptr, subscriptions.subscribers[0]);
    } else {
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
      ASSERT_NE(nullptr, subscriptions.subscribers[0]);
    }

    test_msgs__msg__BasicTypes output_message{};
    ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&output_message);
    });

    ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

    if (is_cft_enabled) {
      EXPECT_FALSE(taken);
    } else {
      EXPECT_TRUE(taken);
      EXPECT_EQ(original_message, output_message);
    }
  }

  // set content filter with empty("")
  if (is_cft_enabled) {
    rmw_subscription_content_filter_options_t options =
      rmw_get_zero_initialized_content_filter_options();
    auto allocator = rcutils_get_default_allocator();
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_init(
        "",
        0,
        nullptr,
        &allocator,
        &options));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK,
        rmw_subscription_content_filter_options_fini(&options, &allocator));
    });

    EXPECT_EQ(
      RMW_RET_OK,
      rmw_subscription_set_content_filter(sub, &options));
    // waiting to allow for filter propagation
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ret = rmw_publish(pub, &original_message, null_allocation_p);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

    rmw_subscriptions_t subscriptions;
    void * subscriptions_storage[1];
    subscriptions_storage[0] = sub->data;
    subscriptions.subscribers = subscriptions_storage;
    subscriptions.subscriber_count = 1;

    rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
    ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK, rmw_destroy_wait_set(wait_set)) << rmw_get_error_string().str;
    });
    rmw_time_t timeout = {5, 0};  // 5000ms
    ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ASSERT_NE(nullptr, subscriptions.subscribers[0]);

    // content filter subscription is reset
    {
      test_msgs__msg__BasicTypes output_message{};
      ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
      OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
      {
        test_msgs__msg__BasicTypes__fini(&output_message);
      });

      ret = rmw_take(sub, &output_message, &taken, null_allocation_s);
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

      EXPECT_TRUE(taken);
      EXPECT_EQ(original_message, output_message);
    }
  }
}

TEST_F(
  CLASSNAME(TestContentFilterSubscriptionUse, RMW_IMPLEMENTATION),
  create_two_filters_with_same_topic_name_and_destroy) {
  if (sub->is_cft_enabled) {
    auto allocator = rcutils_get_default_allocator();
    rmw_subscription_options_t options = rmw_get_default_subscription_options();

    rmw_subscription_content_filter_options_t * content_filter_options =
      static_cast<rmw_subscription_content_filter_options_t *>(
      allocator.allocate(
        sizeof(rmw_subscription_content_filter_options_t), allocator.state));

    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      allocator.deallocate(content_filter_options, allocator.state);
    });

    *content_filter_options = rmw_get_zero_initialized_content_filter_options();

    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_init(
        filter_expression,
        expression_parameters_count,
        expression_parameters,
        &allocator,
        content_filter_options));

    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(
        RMW_RET_OK,
        rmw_subscription_content_filter_options_fini(content_filter_options, &allocator));
    });

    options.content_filter_options = content_filter_options;

    // Create another subscription with content filter in the same topic
    auto sub_2 = rmw_create_subscription(node, ts, topic_name, &qos_profile, &options);
    ASSERT_NE(nullptr, sub_2) << rmw_get_error_string().str;

    rmw_ret_t ret = rmw_destroy_subscription(node, sub_2);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  }
}
