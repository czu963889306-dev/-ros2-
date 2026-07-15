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

#include "osrf_testing_tools_cpp/memory_tools/gtest_quickstart.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcutils/allocator.h"
#include "rcutils/macros.h"
#include "rcutils/strdup.h"

#include "rmw/rmw.h"
#include "rmw/error_handling.h"

#include "test_msgs/msg/basic_types.h"

#include "./config.hpp"
#include "./testing_macros.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestPublisher, RMW_IMPLEMENTATION) : public ::testing::Test
{
protected:
  void SetUp() override
  {
    init_options = rmw_get_zero_initialized_init_options();
    rmw_ret_t ret = rmw_init_options_init(&init_options, rcutils_get_default_allocator());
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    init_options.enclave = rcutils_strdup("/", rcutils_get_default_allocator());
    ASSERT_STREQ("/", init_options.enclave);
    context = rmw_get_zero_initialized_context();
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

  rmw_init_options_t init_options;
  rmw_context_t context;
  rmw_node_t * node;
};

TEST_F(CLASSNAME(TestPublisher, RMW_IMPLEMENTATION), create_and_destroy) {
  rmw_publisher_options_t options = rmw_get_default_publisher_options();
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_publisher_t * pub =
    rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_default, &options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_publisher(node, pub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestPublisher, RMW_IMPLEMENTATION), create_with_internal_errors) {
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rmw_publisher_t * pub = nullptr;
    rmw_publisher_options_t options = rmw_get_default_publisher_options();
    pub = rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_default, &options);
    if (pub) {
      RCUTILS_NO_FAULT_INJECTION(
      {
        rmw_ret_t ret = rmw_destroy_publisher(node, pub);
        EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
      });
    } else {
      rmw_reset_error();
    }
  });
}

TEST_F(CLASSNAME(TestPublisher, RMW_IMPLEMENTATION), create_and_destroy_native) {
  rmw_publisher_options_t options = rmw_get_default_publisher_options();
  constexpr char topic_name[] = "test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_qos_profile_t native_qos_profile = rmw_qos_profile_default;
  native_qos_profile.avoid_ros_namespace_conventions = true;
  rmw_publisher_t * pub =
    rmw_create_publisher(node, ts, topic_name, &native_qos_profile, &options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_publisher(node, pub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestPublisher, RMW_IMPLEMENTATION), create_with_bad_arguments) {
  rmw_publisher_options_t options = rmw_get_default_publisher_options();
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_publisher_t * pub =
    rmw_create_publisher(nullptr, ts, topic_name, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  pub = rmw_create_publisher(node, nullptr, topic_name, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  pub = rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_default, &options);
  node->implementation_identifier = implementation_identifier;
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  pub = rmw_create_publisher(node, ts, nullptr, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  pub = rmw_create_publisher(node, ts, "", &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  constexpr char topic_name_with_spaces[] = "/foo bar";
  pub = rmw_create_publisher(node, ts, topic_name_with_spaces, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  constexpr char relative_topic_name[] = "foo";
  pub = rmw_create_publisher(node, ts, relative_topic_name, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  pub = rmw_create_publisher(node, ts, topic_name, nullptr, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  pub = rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_unknown, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  pub = rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_default, nullptr);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();

  rosidl_message_type_support_t * non_const_ts =
    const_cast<rosidl_message_type_support_t *>(ts);
  const char * typesupport_identifier = non_const_ts->typesupport_identifier;
  non_const_ts->typesupport_identifier = "not-a-typesupport-identifier";
  pub = rmw_create_publisher(node, non_const_ts, topic_name, &rmw_qos_profile_default, &options);
  EXPECT_EQ(nullptr, pub);
  rmw_reset_error();
  non_const_ts->typesupport_identifier = typesupport_identifier;

  // Creating and destroying a publisher still succeeds.
  pub = rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_default, &options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_publisher(node, pub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestPublisher, RMW_IMPLEMENTATION), destroy_with_bad_arguments) {
  rmw_publisher_options_t options = rmw_get_default_publisher_options();
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_publisher_t * pub =
    rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_default, &options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;

  // Destroying publisher with invalid arguments fails.
  rmw_ret_t ret = rmw_destroy_publisher(nullptr, pub);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  ret = rmw_destroy_publisher(node, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_destroy_publisher(node, pub);
  node->implementation_identifier = implementation_identifier;
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  rmw_reset_error();

  // Destroying publisher still succeeds.
  ret = rmw_destroy_publisher(node, pub);
  EXPECT_EQ(RMW_RET_OK, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestPublisher, RMW_IMPLEMENTATION), destroy_with_internal_errors) {
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rmw_publisher_t * pub = nullptr;
    RCUTILS_NO_FAULT_INJECTION(
    {
      rmw_publisher_options_t options = rmw_get_default_publisher_options();
      pub = rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_default, &options);
      ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
    });
    if (RMW_RET_OK != rmw_destroy_publisher(node, pub)) {
      rmw_reset_error();
    }
  });
}

TEST_F(CLASSNAME(TestPublisher, RMW_IMPLEMENTATION), get_actual_qos_from_system_defaults) {
  rmw_publisher_options_t options = rmw_get_default_publisher_options();
  constexpr char topic_name[] = "/test";
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  rmw_publisher_t * pub =
    rmw_create_publisher(node, ts, topic_name, &rmw_qos_profile_system_default, &options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  rmw_qos_profile_t qos_profile = rmw_qos_profile_unknown;
  rmw_ret_t ret = rmw_publisher_get_actual_qos(pub, &qos_profile);
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
  ret = rmw_destroy_publisher(node, pub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

class CLASSNAME (TestPublisherUse, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestPublisher, RMW_IMPLEMENTATION)
{
protected:
  using Base = CLASSNAME(TestPublisher, RMW_IMPLEMENTATION);

  void SetUp() override
  {
    Base::SetUp();
    // Relax QoS policies to force mismatch.
    qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
    rmw_publisher_options_t options = rmw_get_default_publisher_options();
    pub = rmw_create_publisher(node, ts, topic_name, &qos_profile, &options);
    ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;
  }

  void TearDown() override
  {
    rmw_ret_t ret = rmw_destroy_publisher(node, pub);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    Base::TearDown();
  }

  rmw_publisher_t * pub{nullptr};
  const char * const topic_name = "/test";
  const rosidl_message_type_support_t * ts{
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes)};
  rmw_qos_profile_t qos_profile{rmw_qos_profile_default};
};

TEST_F(CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION), get_actual_qos_with_bad_arguments) {
  rmw_qos_profile_t actual_qos_profile = rmw_qos_profile_unknown;
  rmw_ret_t ret = rmw_publisher_get_actual_qos(nullptr, &actual_qos_profile);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  ret = rmw_publisher_get_actual_qos(pub, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  const char * implementation_identifier = pub->implementation_identifier;
  pub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_publisher_get_actual_qos(pub, &actual_qos_profile);
  pub->implementation_identifier = implementation_identifier;
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION), get_actual_qos) {
  rmw_qos_profile_t actual_qos_profile = rmw_qos_profile_unknown;
  rmw_ret_t ret = rmw_publisher_get_actual_qos(pub, &actual_qos_profile);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(qos_profile.history, actual_qos_profile.history);
  EXPECT_EQ(qos_profile.depth, actual_qos_profile.depth);
  EXPECT_EQ(qos_profile.reliability, actual_qos_profile.reliability);
  EXPECT_EQ(qos_profile.durability, actual_qos_profile.durability);
}

TEST_F(CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION), count_matched_subscriptions_with_bad_args) {
  size_t subscription_count = 0u;
  rmw_ret_t ret = rmw_publisher_count_matched_subscriptions(nullptr, &subscription_count);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  ret = rmw_publisher_count_matched_subscriptions(pub, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  const char * implementation_identifier = pub->implementation_identifier;
  pub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
  pub->implementation_identifier = implementation_identifier;
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION), count_matched_subscriptions) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest sqg;

  rmw_ret_t ret;
  size_t subscription_count = 0u;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, subscription_count);

  rmw_subscription_options_t options = rmw_get_default_subscription_options();
  rmw_subscription_t * sub = rmw_create_subscription(node, ts, topic_name, &qos_profile, &options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;

  // TODO(hidmic): revisit when https://github.com/ros2/rmw/issues/264 is resolved.
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 1u == subscription_count) {
      break;
    }
  }

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(1u, subscription_count);

  ret = rmw_destroy_subscription(node, sub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // TODO(hidmic): revisit when https://github.com/ros2/rmw/issues/264 is resolved.
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 0u == subscription_count) {
      break;
    }
  }

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, subscription_count);
}

TEST_F(CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION), count_mismatched_subscriptions) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest sqg;

  rmw_ret_t ret;
  size_t subscription_count = 0u;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, subscription_count);

  // Tighten QoS policies to force mismatch.
  rmw_qos_profile_t other_qos_profile = qos_profile;
  other_qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  rmw_subscription_options_t options = rmw_get_default_subscription_options();
  rmw_subscription_t * sub =
    rmw_create_subscription(node, ts, topic_name, &other_qos_profile, &options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;

  // TODO(hidmic): revisit when https://github.com/ros2/rmw/issues/264 is resolved.
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
    if (RMW_RET_OK == ret && 0u != subscription_count) {  // Early return on failure.
      break;
    }
  }
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, subscription_count);

  ret = rmw_destroy_subscription(node, sub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_publisher_count_matched_subscriptions(pub, &subscription_count);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(0u, subscription_count);
}

TEST_F(
  CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION),
  publish_message_with_bad_arguments) {
  test_msgs__msg__BasicTypes input_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&input_message));
  rmw_publisher_allocation_t * null_allocation{nullptr};  // still valid allocation

  rmw_ret_t ret = rmw_publish(nullptr, &input_message, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  ret = rmw_publish(pub, nullptr, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  const char * implementation_identifier = pub->implementation_identifier;
  pub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_publish(pub, &input_message, null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  pub->implementation_identifier = implementation_identifier;

  test_msgs__msg__BasicTypes__fini(&input_message);
}

TEST_F(CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION), publish_with_internal_errors) {
  test_msgs__msg__BasicTypes message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&message));
  rmw_publisher_allocation_t * null_allocation{nullptr};  // still a valid allocation

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rmw_ret_t ret = rmw_publish(pub, &message, null_allocation);
    if (RMW_RET_OK != ret) {
      rmw_reset_error();
    }
  });
}

TEST_F(
  CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION),
  publish_serialized_message_with_bad_arguments) {
  rmw_publisher_allocation_t * null_allocation{nullptr};  // still valid allocation
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &default_allocator)) << rmw_get_error_string().str;

  rmw_ret_t ret = rmw_publish_serialized_message(nullptr, &serialized_message, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  ret = rmw_publish_serialized_message(pub, nullptr, null_allocation);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();

  const char * implementation_identifier = pub->implementation_identifier;
  pub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_publish_serialized_message(pub, &serialized_message, null_allocation);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  pub->implementation_identifier = implementation_identifier;

  EXPECT_EQ(
    RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION), publish_serialized_with_internal_errors) {
  test_msgs__msg__BasicTypes message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&message));
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  rmw_ret_t ret = rmw_serialized_message_init(&serialized_message, 0lu, &default_allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rmw_ret_t ret = rmw_serialized_message_fini(&serialized_message);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  });
  ret = rmw_serialize(&message, ts, &serialized_message);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  rmw_publisher_allocation_t * null_allocation{nullptr};  // still a valid allocation

  RCUTILS_FAULT_INJECTION_TEST(
  {
    ret = rmw_publish_serialized_message(
      pub, &serialized_message, null_allocation);
    if (RMW_RET_OK != ret) {
      rmw_reset_error();
    }
  });
}

TEST_F(CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION), wait_for_all_acked_with_best_effort) {
  // For best effort, alway return RMW_RET_OK immediately
  rmw_ret_t ret = rmw_publisher_wait_for_all_acked(pub, {0, 0});
  EXPECT_EQ(ret, RMW_RET_OK);

  ret = rmw_publisher_wait_for_all_acked(nullptr, {0, 0});
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT);
}


class CLASSNAME (TestPublisherUseLoan, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION)
{
protected:
  using Base = CLASSNAME(TestPublisherUse, RMW_IMPLEMENTATION);

  void SetUp() override
  {
    Base::SetUp();
    // Check if loaning is supported by the implementation
    if (!pub->can_loan_messages) {
      void * msg_pointer = nullptr;
      rmw_publisher_allocation_t * null_allocation{nullptr};
      rmw_ret_t ret = rmw_borrow_loaned_message(pub, ts, &msg_pointer);
      EXPECT_EQ(RMW_RET_UNSUPPORTED, ret) << rmw_get_error_string().str;
      rmw_reset_error();
      EXPECT_EQ(nullptr, msg_pointer);
      ret = rmw_return_loaned_message_from_publisher(pub, &msg_pointer);
      EXPECT_EQ(RMW_RET_UNSUPPORTED, ret) << rmw_get_error_string().str;
      rmw_reset_error();
      EXPECT_EQ(nullptr, msg_pointer);
      ret = rmw_publish_loaned_message(pub, &msg_pointer, null_allocation);
      EXPECT_EQ(RMW_RET_UNSUPPORTED, ret) << rmw_get_error_string().str;
      rmw_reset_error();
      EXPECT_EQ(nullptr, msg_pointer);
      GTEST_SKIP();
    }
  }

  void TearDown() override
  {
    Base::TearDown();
  }
};

TEST_F(
  CLASSNAME(TestPublisherUseLoan, RMW_IMPLEMENTATION),
  borrow_loaned_message_with_bad_arguments) {
  void * msg_pointer = nullptr;
  rmw_ret_t ret = rmw_borrow_loaned_message(nullptr, ts, &msg_pointer);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, msg_pointer);

  ret = rmw_borrow_loaned_message(pub, nullptr, &msg_pointer);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, msg_pointer);

  ret = rmw_borrow_loaned_message(pub, ts, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  EXPECT_EQ(nullptr, msg_pointer);

  ret = rmw_borrow_loaned_message(pub, ts, &msg_pointer);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  // Not null msg_pointer invalid to borrow message
  ret = rmw_borrow_loaned_message(pub, ts, &msg_pointer);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  ret = rmw_return_loaned_message_from_publisher(pub, msg_pointer);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  msg_pointer = nullptr;
  const char * implementation_identifier = pub->implementation_identifier;
  pub->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_borrow_loaned_message(pub, ts, &msg_pointer);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  pub->implementation_identifier = implementation_identifier;
}
