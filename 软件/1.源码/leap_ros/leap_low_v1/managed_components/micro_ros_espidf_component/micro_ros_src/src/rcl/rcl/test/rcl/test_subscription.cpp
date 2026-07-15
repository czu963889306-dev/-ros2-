// Copyright 2015 Open Source Robotics Foundation, Inc.
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
#include <string>
#include <thread>

#include "rcl/subscription.h"
#include "rcl/rcl.h"
#include "rmw/rmw.h"
#include "rmw/validate_full_topic_name.h"

#include "rcutils/strdup.h"
#include "rcutils/testing/fault_injection.h"
#include "test_msgs/msg/basic_types.h"
#include "test_msgs/msg/strings.h"
#include "rosidl_runtime_c/string_functions.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcl/node.h"
#include "rcutils/env.h"
#include "wait_for_entity_helpers.hpp"

#include "./allocator_testing_utils.h"
#include "../mocking_utils/patch.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestSubscriptionFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  rcl_context_t * context_ptr;
  rcl_node_t * node_ptr;
  void SetUp()
  {
    rcl_ret_t ret;
    {
      rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
      ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
      {
        EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
      });
      this->context_ptr = new rcl_context_t;
      *this->context_ptr = rcl_get_zero_initialized_context();
      ret = rcl_init(0, nullptr, &init_options, this->context_ptr);
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    }
    this->node_ptr = new rcl_node_t;
    *this->node_ptr = rcl_get_zero_initialized_node();
    constexpr char name[] = "test_subscription_node";
    rcl_node_options_t node_options = rcl_node_get_default_options();
    ret = rcl_node_init(this->node_ptr, name, "", this->context_ptr, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown()
  {
    rcl_ret_t ret = rcl_node_fini(this->node_ptr);
    delete this->node_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_shutdown(this->context_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(this->context_ptr);
    delete this->context_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
};

class CLASSNAME (TestSubscriptionFixtureInit, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION)
{
public:
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  const char * topic = "/chatter";
  rcl_subscription_options_t subscription_options;
  rcl_subscription_t subscription;
  rcl_subscription_t subscription_zero_init;
  rcl_ret_t ret;
  rcutils_allocator_t allocator;

  void SetUp() override
  {
    CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION) ::SetUp();
    allocator = rcutils_get_default_allocator();
    subscription_options = rcl_subscription_get_default_options();
    subscription = rcl_get_zero_initialized_subscription();
    subscription_zero_init = rcl_get_zero_initialized_subscription();
    ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown() override
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION) ::TearDown();
  }
};

/* Test subscription init, fini and is_valid functions
 */
TEST_F(
  CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION),
  test_subscription_init_fini_and_is_valid)
{
  rcl_ret_t ret;

  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic[] = "chatter";
  constexpr char expected_topic[] = "/chatter";

  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(strcmp(rcl_subscription_get_topic_name(&subscription), expected_topic), 0);
  ret = rcl_subscription_fini(&subscription, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Test is_valid for subscription with nullptr
  EXPECT_FALSE(rcl_subscription_is_valid(nullptr));
  rcl_reset_error();

  // Test is_valid for zero initialized subscription
  subscription = rcl_get_zero_initialized_subscription();
  EXPECT_FALSE(rcl_subscription_is_valid(&subscription));
  rcl_reset_error();
}

// Define dummy comparison operators for rcutils_allocator_t type
// to use with the Mimick mocking library
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, ==)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, !=)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, <)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, >)

// Bad arguments for init and fini
TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_subscription_bad_init) {
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic[] = "/chatter";
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  rcl_node_t invalid_node = rcl_get_zero_initialized_node();

  ASSERT_FALSE(rcl_node_is_valid_except_context(&invalid_node));
  rcl_reset_error();

  EXPECT_EQ(nullptr, rcl_node_get_rmw_handle(&invalid_node));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_NODE_INVALID,
    rcl_subscription_init(&subscription, nullptr, ts, topic, &subscription_options));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_NODE_INVALID,
    rcl_subscription_init(&subscription, &invalid_node, ts, topic, &subscription_options));
  rcl_reset_error();

  rcl_ret_t ret = rcl_subscription_init(
    &subscription, this->node_ptr, ts, "spaced name", &subscription_options);
  EXPECT_EQ(RCL_RET_TOPIC_NAME_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_subscription_init(
    &subscription, this->node_ptr, ts, "sub{ros_not_match}", &subscription_options);
  EXPECT_EQ(RCL_RET_TOPIC_NAME_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  {
    rcutils_ret_t rcutils_string_map_init_returns = RCUTILS_RET_BAD_ALLOC;
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rcutils_string_map_init, rcutils_string_map_init_returns);
    ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
    EXPECT_EQ(RCL_RET_BAD_ALLOC, ret);
    rcl_reset_error();

    rcutils_string_map_init_returns = RCUTILS_RET_ERROR;
    ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }
  {
    auto mock =
      mocking_utils::inject_on_return("lib:rcl", rcutils_string_map_fini, RCUTILS_RET_ERROR);
    ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }
  {
    rmw_ret_t rmw_validate_full_topic_name_returns = RMW_RET_OK;
    auto mock = mocking_utils::patch(
      "lib:rcl", rmw_validate_full_topic_name,
      [&](auto, int * result, auto) {
        *result = RMW_TOPIC_INVALID_TOO_LONG;
        return rmw_validate_full_topic_name_returns;
      });
    ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
    EXPECT_EQ(RCL_RET_TOPIC_NAME_INVALID, ret);
    rcl_reset_error();

    rmw_validate_full_topic_name_returns = RMW_RET_ERROR;
    ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }
  {
    auto mock =
      mocking_utils::patch_and_return("lib:rcl", rmw_create_subscription, nullptr);
    ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }
  {
    auto mock =
      mocking_utils::patch_and_return("lib:rcl", rmw_subscription_get_actual_qos, RMW_RET_ERROR);
    ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }

  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_TRUE(rcl_subscription_is_valid(&subscription));
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  EXPECT_EQ(RCL_RET_ALREADY_INIT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  EXPECT_EQ(RCL_RET_SUBSCRIPTION_INVALID, rcl_subscription_fini(nullptr, this->node_ptr));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_NODE_INVALID, rcl_subscription_fini(&subscription, nullptr));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_NODE_INVALID, rcl_subscription_fini(&subscription, &invalid_node));
  rcl_reset_error();

  auto mock =
    mocking_utils::inject_on_return("lib:rcl", rmw_destroy_subscription, RMW_RET_ERROR);
  EXPECT_EQ(RCL_RET_ERROR, rcl_subscription_fini(&subscription, this->node_ptr));
  rcl_reset_error();

  // Make sure finalization completed anyways.
  ASSERT_EQ(NULL, subscription.impl);
}

/* Basic nominal test of a subscription
 */
TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_subscription_nominal) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic[] = "/chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  rcl_reset_error();

  ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 100));
#ifdef RMW_TIMESTAMPS_SUPPORTED
  rcl_time_point_value_t pre_publish_time;
  EXPECT_EQ(
    RCUTILS_RET_OK,
    rcutils_system_time_now(&pre_publish_time)) << " could not get system time failed";
#endif
  {
    test_msgs__msg__BasicTypes msg;
    test_msgs__msg__BasicTypes__init(&msg);
    msg.int64_value = 42;
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__BasicTypes__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 100));
  {
    test_msgs__msg__BasicTypes msg;
    test_msgs__msg__BasicTypes__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&msg);
    });
    rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
    ret = rcl_take(&subscription, &msg, &message_info, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_EQ(42, msg.int64_value);
  #ifdef RMW_TIMESTAMPS_SUPPORTED
    EXPECT_NE(0u, message_info.source_timestamp);
    EXPECT_TRUE(pre_publish_time <= message_info.source_timestamp) <<
      pre_publish_time << " > " << message_info.source_timestamp;
  #ifdef RMW_RECEIVED_TIMESTAMP_SUPPORTED
    EXPECT_NE(0u, message_info.received_timestamp);
    EXPECT_TRUE(pre_publish_time <= message_info.received_timestamp);
    EXPECT_TRUE(message_info.source_timestamp <= message_info.received_timestamp);
  #else
    EXPECT_EQ(0u, message_info.received_timestamp);
  #endif
  #else
    EXPECT_EQ(0u, message_info.source_timestamp);
    EXPECT_EQ(0u, message_info.received_timestamp);
  #endif
  }
}

/* Basic nominal test of a publisher with a string.
 */
TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_subscription_nominal_string) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic[] = "rcl_test_subscription_nominal_string_chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 100));
  constexpr char test_string[] = "testing";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 100));
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_EQ(std::string(test_string), std::string(msg.string_value.data, msg.string_value.size));
  }
}

/* Basic nominal test of a subscription taking a sequence.
 */
TEST_F(
  CLASSNAME(
    TestSubscriptionFixture,
    RMW_IMPLEMENTATION), test_subscription_nominal_string_sequence) {
  using namespace std::chrono_literals;
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic[] = "rcl_test_subscription_nominal_string_sequence_chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 100));
  constexpr char test_string[] = "testing";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_publish(&publisher, &msg, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 100));
  auto allocator = rcutils_get_default_allocator();
  {
    size_t size = 1;
    rmw_message_info_sequence_t message_infos;
    rmw_message_info_sequence_init(&message_infos, size, &allocator);

    rmw_message_sequence_t messages;
    rmw_message_sequence_init(&messages, size, &allocator);

    auto seq = test_msgs__msg__Strings__Sequence__create(size);

    for (size_t ii = 0; ii < size; ++ii) {
      messages.data[ii] = &seq->data[ii];
    }

    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      rmw_message_info_sequence_fini(&message_infos);
      rmw_message_sequence_fini(&messages);
      test_msgs__msg__Strings__Sequence__destroy(seq);
    });

    // Attempt to take more than capacity allows.
    ret = rcl_take_sequence(&subscription, 5, &messages, &message_infos, nullptr);
    ASSERT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;

    ASSERT_EQ(0u, messages.size);
    ASSERT_EQ(0u, message_infos.size);
  }

  {
    size_t size = 5;
    rmw_message_info_sequence_t message_infos;
    rmw_message_info_sequence_init(&message_infos, size, &allocator);

    rmw_message_sequence_t messages;
    rmw_message_sequence_init(&messages, size, &allocator);

    auto seq = test_msgs__msg__Strings__Sequence__create(size);

    for (size_t ii = 0; ii < size; ++ii) {
      messages.data[ii] = &seq->data[ii];
    }

    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      rmw_message_info_sequence_fini(&message_infos);
      rmw_message_sequence_fini(&messages);
      test_msgs__msg__Strings__Sequence__destroy(seq);
    });

    auto start = std::chrono::steady_clock::now();
    size_t total_messages_taken = 0u;
    do {
      // `wait_for_subscription_to_be_ready` only ensures there's one message ready,
      // so we need to loop to guarantee that we get the three published messages.
      ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 1, 100));
      ret = rcl_take_sequence(&subscription, 5, &messages, &message_infos, nullptr);
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      total_messages_taken += messages.size;
      EXPECT_EQ(messages.size, message_infos.size);
    } while (total_messages_taken < 3 && std::chrono::steady_clock::now() < start + 10s);

    EXPECT_EQ(3u, total_messages_taken);
  }

  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_publish(&publisher, &msg, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_publish(&publisher, &msg, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_publish(&publisher, &msg, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  // Give a brief moment for publications to go through.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // Take fewer messages than are available in the subscription
  {
    size_t size = 3;
    rmw_message_info_sequence_t message_infos;
    rmw_message_info_sequence_init(&message_infos, size, &allocator);

    rmw_message_sequence_t messages;
    rmw_message_sequence_init(&messages, size, &allocator);

    auto seq = test_msgs__msg__Strings__Sequence__create(size);

    for (size_t ii = 0; ii < size; ++ii) {
      messages.data[ii] = &seq->data[ii];
    }

    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      rmw_message_info_sequence_fini(&message_infos);
      rmw_message_sequence_fini(&messages);
      test_msgs__msg__Strings__Sequence__destroy(seq);
    });

    auto start = std::chrono::steady_clock::now();
    size_t total_messages_taken = 0u;
    do {
      // `wait_for_subscription_to_be_ready` only ensures there's one message ready,
      // so we need to loop to guarantee that we get the three published messages.
      ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 1, 100));
      ret = rcl_take_sequence(&subscription, 3, &messages, &message_infos, nullptr);
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      total_messages_taken += messages.size;
      EXPECT_EQ(messages.size, message_infos.size);
    } while (total_messages_taken < 3 && std::chrono::steady_clock::now() < start + 10s);

    EXPECT_EQ(3u, total_messages_taken);
    EXPECT_EQ(
      std::string(test_string),
      std::string(seq->data[0].string_value.data, seq->data[0].string_value.size));
  }
}

/* Basic nominal test of a subscription with take_serialize msg
 */
TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_subscription_serialized) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  rcutils_allocator_t allocator = rcl_get_default_allocator();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic[] = "/chatterSer";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  rcl_serialized_message_t serialized_msg = rmw_get_zero_initialized_serialized_message();
  auto initial_capacity_ser = 0u;
  ASSERT_EQ(
    RCL_RET_OK, rmw_serialized_message_init(
      &serialized_msg, initial_capacity_ser, &allocator)) << rcl_get_error_string().str;
  constexpr char test_string[] = "testing";
  test_msgs__msg__Strings msg;
  test_msgs__msg__Strings__init(&msg);
  ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
  ASSERT_STREQ(msg.string_value.data, test_string);
  ret = rmw_serialize(&msg, ts, &serialized_msg);
  ASSERT_EQ(RMW_RET_OK, ret);

  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(
      RMW_RET_OK,
      rmw_serialized_message_fini(&serialized_msg)) << rcl_get_error_string().str;
  });
  rcl_reset_error();

  ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 100));
  {
    ret = rcl_publish_serialized_message(&publisher, &serialized_msg, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 100));
  {
    rcl_serialized_message_t serialized_msg_rcv = rmw_get_zero_initialized_serialized_message();
    initial_capacity_ser = 0u;
    ASSERT_EQ(
      RCL_RET_OK, rmw_serialized_message_init(
        &serialized_msg_rcv, initial_capacity_ser, &allocator)) << rcl_get_error_string().str;
    ret = rcl_take_serialized_message(&subscription, &serialized_msg_rcv, nullptr, nullptr);
    ASSERT_EQ(RMW_RET_OK, ret);

    test_msgs__msg__Strings msg_rcv;
    test_msgs__msg__Strings__init(&msg_rcv);
    ret = rmw_deserialize(&serialized_msg_rcv, ts, &msg_rcv);
    ASSERT_EQ(RMW_RET_OK, ret);
    ASSERT_EQ(
      std::string(test_string), std::string(msg_rcv.string_value.data, msg_rcv.string_value.size));

    test_msgs__msg__Strings__fini(&msg_rcv);
    ASSERT_EQ(
      RMW_RET_OK,
      rmw_serialized_message_fini(&serialized_msg_rcv)) << rcl_get_error_string().str;
  }
}

/* Basic test for subscription loan functions
 */
TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_subscription_loaned) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic[] = "rcl_loan";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 100));
  const char * test_string = "testing";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 100));
  {
    auto patch_take =
      mocking_utils::prepare_patch("lib:rcl", rmw_take_loaned_message_with_info);
    auto patch_return =
      mocking_utils::prepare_patch("lib:rcl", rmw_return_loaned_message_from_subscription);

    if (!rcl_subscription_can_loan_messages(&subscription)) {
      // If rcl (and ultimately rmw) does not support message loaning,
      // mock it so that a unit test can still be constructed.
      patch_take.then_call(
        [](auto sub, void ** loaned_message, auto taken, auto msg_info, auto allocation) {
          auto msg = new(std::nothrow) test_msgs__msg__Strings{};
          if (!msg) {
            return RMW_RET_BAD_ALLOC;
          }
          test_msgs__msg__Strings__init(msg);
          *loaned_message = static_cast<void *>(msg);
          rmw_ret_t ret = rmw_take_with_info(sub, *loaned_message, taken, msg_info, allocation);
          if (RMW_RET_OK != ret) {
            delete msg;
          }
          return ret;
        });
      patch_return.then_call(
        [](auto, void * loaned_message) {
          auto msg = reinterpret_cast<test_msgs__msg__Strings *>(loaned_message);
          test_msgs__msg__Strings__fini(msg);
          delete msg;

          return RMW_RET_OK;
        });
    }

    test_msgs__msg__Strings * msg_loaned = nullptr;
    ret = rcl_take_loaned_message(
      &subscription, reinterpret_cast<void **>(&msg_loaned), nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(
      std::string(test_string),
      std::string(msg_loaned->string_value.data, msg_loaned->string_value.size));
    ret = rcl_return_loaned_message_from_subscription(&subscription, msg_loaned);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
}

TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_subscription_loan_disable) {
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic[] = "pod_msg";
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  rcl_ret_t ret =
    rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  if (rcl_subscription_can_loan_messages(&subscription)) {
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "0"));
    EXPECT_TRUE(rcl_subscription_can_loan_messages(&subscription));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "1"));
    EXPECT_FALSE(rcl_subscription_can_loan_messages(&subscription));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "2"));
    EXPECT_TRUE(rcl_subscription_can_loan_messages(&subscription));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "Unexpected"));
    EXPECT_TRUE(rcl_subscription_can_loan_messages(&subscription));
  } else {
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "0"));
    EXPECT_FALSE(rcl_subscription_can_loan_messages(&subscription));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "1"));
    EXPECT_FALSE(rcl_subscription_can_loan_messages(&subscription));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "2"));
    EXPECT_FALSE(rcl_subscription_can_loan_messages(&subscription));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "Unexpected"));
    EXPECT_FALSE(rcl_subscription_can_loan_messages(&subscription));
  }
}

/* Test for all failure modes in subscription take with loaned messages function.
 */
TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_bad_take_loaned_message) {
  constexpr char topic[] = "rcl_loan";
  const rosidl_message_type_support_t * ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();

  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  rmw_ret_t ret = rcl_subscription_init(
    &subscription, this->node_ptr, ts, topic,
    &subscription_options);
  ASSERT_EQ(RMW_RET_OK, ret) << rcl_get_error_string().str;

  test_msgs__msg__Strings * loaned_message = nullptr;
  void ** type_erased_loaned_message_pointer = reinterpret_cast<void **>(&loaned_message);
  rmw_message_info_t * message_info = nullptr;  // is a valid argument
  rmw_subscription_allocation_t * allocation = nullptr;  // is a valid argument
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID, rcl_take_loaned_message(
      nullptr, type_erased_loaned_message_pointer, message_info, allocation));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_take_loaned_message(&subscription, nullptr, message_info, allocation));
  rcl_reset_error();

  test_msgs__msg__Strings dummy_message;
  loaned_message = &dummy_message;
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_take_loaned_message(
      &subscription, type_erased_loaned_message_pointer, message_info, allocation));
  rcl_reset_error();
  loaned_message = nullptr;

  {
    rmw_ret_t rmw_take_loaned_message_with_info_returns = RMW_RET_OK;
    auto mock = mocking_utils::patch(
      "lib:rcl", rmw_take_loaned_message_with_info,
      [&](auto, auto, bool * taken, auto && ...) {
        *taken = false;
        return rmw_take_loaned_message_with_info_returns;
      });

    EXPECT_EQ(
      RCL_RET_SUBSCRIPTION_TAKE_FAILED, rcl_take_loaned_message(
        &subscription, type_erased_loaned_message_pointer, message_info, allocation));
    rcl_reset_error();

    rmw_take_loaned_message_with_info_returns = RMW_RET_BAD_ALLOC;
    EXPECT_EQ(
      RCL_RET_BAD_ALLOC, rcl_take_loaned_message(
        &subscription, type_erased_loaned_message_pointer, message_info, allocation));
    rcl_reset_error();

    rmw_take_loaned_message_with_info_returns = RMW_RET_UNSUPPORTED;
    EXPECT_EQ(
      RCL_RET_UNSUPPORTED, rcl_take_loaned_message(
        &subscription, type_erased_loaned_message_pointer, message_info, allocation));
    rcl_reset_error();

    rmw_take_loaned_message_with_info_returns = RMW_RET_ERROR;
    EXPECT_EQ(
      RCL_RET_ERROR, rcl_take_loaned_message(
        &subscription, type_erased_loaned_message_pointer, message_info, allocation));
    rcl_reset_error();
  }

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_subscription_fini(&subscription, this->node_ptr)) << rcl_get_error_string().str;
}

/* Test for all failure modes in subscription return loaned messages function.
 */
TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_bad_return_loaned_message) {
  constexpr char topic[] = "rcl_loan";
  const rosidl_message_type_support_t * ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  test_msgs__msg__Strings dummy_message;
  test_msgs__msg__Strings__init(&dummy_message);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__Strings__fini(&dummy_message);
  });
  void * loaned_message = &dummy_message;

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_return_loaned_message_from_subscription(nullptr, loaned_message));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_return_loaned_message_from_subscription(&subscription, loaned_message));
  rcl_reset_error();

  rmw_ret_t ret = rcl_subscription_init(
    &subscription, this->node_ptr, ts, topic,
    &subscription_options);
  ASSERT_EQ(RMW_RET_OK, ret) << rcl_get_error_string().str;

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_return_loaned_message_from_subscription(&subscription, nullptr));
  rcl_reset_error();

  {
    rmw_ret_t rmw_return_loaned_message_from_subscription_returns = RMW_RET_OK;
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_return_loaned_message_from_subscription,
      rmw_return_loaned_message_from_subscription_returns);

    EXPECT_EQ(
      RCL_RET_OK, rcl_return_loaned_message_from_subscription(&subscription, loaned_message)) <<
      rcl_get_error_string().str;

    rmw_return_loaned_message_from_subscription_returns = RMW_RET_UNSUPPORTED;
    EXPECT_EQ(
      RCL_RET_UNSUPPORTED, rcl_return_loaned_message_from_subscription(
        &subscription,
        loaned_message));
    rcl_reset_error();

    rmw_return_loaned_message_from_subscription_returns = RMW_RET_ERROR;
    EXPECT_EQ(
      RCL_RET_ERROR, rcl_return_loaned_message_from_subscription(&subscription, loaned_message));
    rcl_reset_error();
  }

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_subscription_fini(&subscription, this->node_ptr)) << rcl_get_error_string().str;
}

/* A subscription with a content filtered topic setting.
 */
TEST_F(
  CLASSNAME(
    TestSubscriptionFixture,
    RMW_IMPLEMENTATION), test_subscription_content_filtered) {
  const char * filter_expression1 = "string_value = 'FilteredData'";
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic[] = "rcl_test_subscription_content_filtered_chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_subscription_options_set_content_filter_options(
      filter_expression1, 0, nullptr, &subscription_options)
  );

  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  bool is_cft_support = rcl_subscription_is_cft_enabled(&subscription);
  ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 1000));

  // publish with a non-filtered data
  constexpr char test_string[] = "NotFilteredData";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  if (is_cft_support) {
    ASSERT_FALSE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));
  } else {
    ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));

    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_EQ(
      std::string(test_string),
      std::string(msg.string_value.data, msg.string_value.size));
  }

  constexpr char test_filtered_string[] = "FilteredData";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_filtered_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));

  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_EQ(
      std::string(test_filtered_string),
      std::string(msg.string_value.data, msg.string_value.size));
  }

  // set filter
  const char * filter_expression2 = "string_value = %0";
  const char * expression_parameters2[] = {"'FilteredOtherData'"};
  size_t expression_parameters2_count = sizeof(expression_parameters2) / sizeof(char *);
  {
    rcl_subscription_content_filter_options_t options =
      rcl_get_zero_initialized_subscription_content_filter_options();

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_init(
        &subscription,
        filter_expression2, expression_parameters2_count, expression_parameters2,
        &options)
    );

    ret = rcl_subscription_set_content_filter(
      &subscription, &options);
    if (is_cft_support) {
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      // waiting to allow for filter propagation
      std::this_thread::sleep_for(std::chrono::seconds(10));
    } else {
      ASSERT_EQ(RCL_RET_UNSUPPORTED, ret);
    }

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_fini(
        &subscription, &options)
    );
  }

  // publish FilteredData again
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_filtered_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  if (is_cft_support) {
    ASSERT_FALSE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));
  } else {
    ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));

    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_EQ(
      std::string(test_filtered_string),
      std::string(msg.string_value.data, msg.string_value.size));
  }

  constexpr char test_filtered_other_string[] = "FilteredOtherData";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_filtered_other_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));

  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_EQ(
      std::string(test_filtered_other_string),
      std::string(msg.string_value.data, msg.string_value.size));
  }

  // get filter
  {
    rcl_subscription_content_filter_options_t content_filter_options =
      rcl_get_zero_initialized_subscription_content_filter_options();

    ret = rcl_subscription_get_content_filter(
      &subscription, &content_filter_options);
    if (is_cft_support) {
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

      rmw_subscription_content_filter_options_t * options =
        &content_filter_options.rmw_subscription_content_filter_options;
      ASSERT_STREQ(filter_expression2, options->filter_expression);
      ASSERT_EQ(expression_parameters2_count, options->expression_parameters.size);
      for (size_t i = 0; i < expression_parameters2_count; ++i) {
        EXPECT_STREQ(
          options->expression_parameters.data[i],
          expression_parameters2[i]);
      }
      EXPECT_EQ(
        RCL_RET_OK,
        rcl_subscription_content_filter_options_fini(
          &subscription,
          &content_filter_options)
      );
    } else {
      ASSERT_EQ(RCL_RET_UNSUPPORTED, ret);
    }
  }

  // reset filter
  {
    rcl_subscription_content_filter_options_t options =
      rcl_get_zero_initialized_subscription_content_filter_options();

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_init(
        &subscription,
        "", 0, nullptr,
        &options)
    );

    ret = rcl_subscription_set_content_filter(
      &subscription, &options);
    if (is_cft_support) {
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      // waiting to allow for filter propagation
      std::this_thread::sleep_for(std::chrono::seconds(10));
      ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 1000));
      ASSERT_FALSE(rcl_subscription_is_cft_enabled(&subscription));
    } else {
      ASSERT_EQ(RCL_RET_UNSUPPORTED, ret);
    }

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_fini(
        &subscription, &options)
    );
  }

  // publish with a non-filtered data again
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));

  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_EQ(
      std::string(test_string),
      std::string(msg.string_value.data, msg.string_value.size));
  }
}

/* A subscription without a content filtered topic setting at beginning.
 */
TEST_F(
  CLASSNAME(
    TestSubscriptionFixture,
    RMW_IMPLEMENTATION), test_subscription_not_initialized_with_content_filtering) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic[] = "rcl_test_subscription_not_begin_content_filtered_chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  // not to set filter expression
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ASSERT_FALSE(rcl_subscription_is_cft_enabled(&subscription));

  // failed to get filter
  {
    rcl_subscription_content_filter_options_t content_filter_options =
      rcl_get_zero_initialized_subscription_content_filter_options();

    ret = rcl_subscription_get_content_filter(
      &subscription, &content_filter_options);
    ASSERT_NE(RCL_RET_OK, ret);
  }

  ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 1000));

  // publish with a non-filtered data
  int32_t test_value = 3;
  {
    test_msgs__msg__BasicTypes msg;
    test_msgs__msg__BasicTypes__init(&msg);
    msg.int32_value = test_value;
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__BasicTypes__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));

  {
    test_msgs__msg__BasicTypes msg;
    test_msgs__msg__BasicTypes__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_TRUE(test_value == msg.int32_value);
  }

  // set filter
  const char * filter_expression2 = "int32_value = %0";
  const char * expression_parameters2[] = {"4"};
  size_t expression_parameters2_count = sizeof(expression_parameters2) / sizeof(char *);
  bool is_cft_support =
    (std::string(rmw_get_implementation_identifier()).find("rmw_connextdds") == 0 ||
    std::string(rmw_get_implementation_identifier()).find("rmw_fastrtps_cpp") == 0);
  {
    rcl_subscription_content_filter_options_t options =
      rcl_get_zero_initialized_subscription_content_filter_options();

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_init(
        &subscription,
        filter_expression2, expression_parameters2_count, expression_parameters2,
        &options)
    );

    ret = rcl_subscription_set_content_filter(
      &subscription, &options);
    if (!is_cft_support) {
      ASSERT_EQ(RCL_RET_UNSUPPORTED, ret);
    } else {
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      // waiting to allow for filter propagation
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_fini(
        &subscription, &options)
    );
  }

  // publish no filtered data again
  {
    test_msgs__msg__BasicTypes msg;
    test_msgs__msg__BasicTypes__init(&msg);
    msg.int32_value = test_value;
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__BasicTypes__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  if (is_cft_support) {
    ASSERT_FALSE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));
  } else {
    ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));

    test_msgs__msg__BasicTypes msg;
    test_msgs__msg__BasicTypes__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_TRUE(test_value == msg.int32_value);
  }

  // publish filtered data
  int32_t test_filtered_value = 4;
  {
    test_msgs__msg__BasicTypes msg;
    test_msgs__msg__BasicTypes__init(&msg);
    msg.int32_value = test_filtered_value;
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__BasicTypes__fini(&msg);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription, context_ptr, 10, 1000));

  {
    test_msgs__msg__BasicTypes msg;
    test_msgs__msg__BasicTypes__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__BasicTypes__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ASSERT_TRUE(test_filtered_value == msg.int32_value);
  }
}

TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_get_options) {
  rcl_ret_t ret;
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic[] = "test_get_options";
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  ret = rcl_subscription_init(&subscription, this->node_ptr, ts, topic, &subscription_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  const rcl_subscription_options_t * get_sub_options = rcl_subscription_get_options(&subscription);
  ASSERT_EQ(subscription_options.qos.history, get_sub_options->qos.history);
  ASSERT_EQ(subscription_options.qos.depth, get_sub_options->qos.depth);
  ASSERT_EQ(subscription_options.qos.durability, get_sub_options->qos.durability);

  ASSERT_EQ(NULL, rcl_subscription_get_options(nullptr));
}

/* bad take()
*/
TEST_F(CLASSNAME(TestSubscriptionFixtureInit, RMW_IMPLEMENTATION), test_subscription_bad_take) {
  test_msgs__msg__BasicTypes msg;
  rmw_message_info_t message_info = rmw_get_zero_initialized_message_info();
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&msg));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BasicTypes__fini(&msg);
  });
  EXPECT_EQ(RCL_RET_SUBSCRIPTION_INVALID, rcl_take(nullptr, &msg, &message_info, nullptr));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID, rcl_take(&subscription_zero_init, &msg, &message_info, nullptr));
  rcl_reset_error();

  rmw_ret_t rmw_take_with_info_returns = RMW_RET_OK;
  auto mock = mocking_utils::patch(
    "lib:rcl", rmw_take_with_info,
    [&](auto, auto, bool * taken, auto...) {
      *taken = false;
      return rmw_take_with_info_returns;
    });

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_TAKE_FAILED, rcl_take(&subscription, &msg, &message_info, nullptr));
  rcl_reset_error();

  rmw_take_with_info_returns = RMW_RET_BAD_ALLOC;
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC, rcl_take(&subscription, &msg, &message_info, nullptr));
  rcl_reset_error();

  rmw_take_with_info_returns = RMW_RET_ERROR;
  EXPECT_EQ(
    RCL_RET_ERROR, rcl_take(&subscription, &msg, &message_info, nullptr));
  rcl_reset_error();
}

/* bad take_serialized
*/
TEST_F(
  CLASSNAME(TestSubscriptionFixtureInit, RMW_IMPLEMENTATION),
  test_subscription_bad_take_serialized) {
  rcl_serialized_message_t serialized_msg = rmw_get_zero_initialized_serialized_message();
  size_t initial_serialization_capacity = 0u;
  ASSERT_EQ(
    RCL_RET_OK, rmw_serialized_message_init(
      &serialized_msg, initial_serialization_capacity, &allocator)) <<
    rcl_get_error_string().str;

  rmw_message_info_t * message_info = nullptr;  // is a valid argument
  rmw_subscription_allocation_t * allocation = nullptr;  // is a valid argument
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_take_serialized_message(nullptr, &serialized_msg, message_info, allocation));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_take_serialized_message(
      &subscription_zero_init, &serialized_msg,
      message_info, allocation));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_take_serialized_message(&subscription, nullptr, message_info, allocation));
  rcl_reset_error();

  rmw_ret_t rmw_take_serialized_message_with_info_returns = RMW_RET_OK;
  auto mock = mocking_utils::patch(
    "lib:rcl", rmw_take_serialized_message_with_info,
    [&](auto, auto, bool * taken, auto...) {
      *taken = false;
      return rmw_take_serialized_message_with_info_returns;
    });

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_TAKE_FAILED,
    rcl_take_serialized_message(&subscription, &serialized_msg, message_info, allocation));
  rcl_reset_error();

  rmw_take_serialized_message_with_info_returns = RMW_RET_BAD_ALLOC;
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC,
    rcl_take_serialized_message(&subscription, &serialized_msg, message_info, allocation));
  rcl_reset_error();

  rmw_take_serialized_message_with_info_returns = RMW_RET_ERROR;
  EXPECT_EQ(
    RCL_RET_ERROR,
    rcl_take_serialized_message(&subscription, &serialized_msg, message_info, allocation));
  rcl_reset_error();
}

/* Bad arguments take_sequence
 */
TEST_F(
  CLASSNAME(TestSubscriptionFixtureInit, RMW_IMPLEMENTATION), test_subscription_bad_take_sequence)
{
  size_t seq_size = 3u;
  rmw_message_sequence_t messages;
  ASSERT_EQ(RMW_RET_OK, rmw_message_sequence_init(&messages, seq_size, &allocator));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_message_sequence_fini(&messages));
  });
  rmw_message_info_sequence_t message_infos_short;
  ASSERT_EQ(
    RMW_RET_OK, rmw_message_info_sequence_init(&message_infos_short, seq_size - 1u, &allocator));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_message_info_sequence_fini(&message_infos_short));
  });
  rmw_message_info_sequence_t message_infos;
  ASSERT_EQ(
    RMW_RET_OK, rmw_message_info_sequence_init(&message_infos, seq_size, &allocator));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RMW_RET_OK, rmw_message_info_sequence_fini(&message_infos));
  });
  rmw_subscription_allocation_t * allocation = nullptr;  // is a valid argument

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_take_sequence(nullptr, seq_size, &messages, &message_infos, allocation));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_take_sequence(&subscription_zero_init, seq_size, &messages, &message_infos, allocation));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_take_sequence(&subscription, seq_size + 1, &messages, &message_infos, allocation));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_take_sequence(&subscription, seq_size, &messages, &message_infos_short, allocation));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_take_sequence(&subscription, seq_size, nullptr, &message_infos, allocation));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_take_sequence(&subscription, seq_size, &messages, nullptr, allocation));
  rcl_reset_error();

  rmw_ret_t rmw_take_sequence_returns = RMW_RET_OK;
  auto mock = mocking_utils::patch(
    "lib:rcl", rmw_take_sequence,
    [&](auto, auto, auto, auto, size_t * taken, auto) {
      *taken = 0u;
      return rmw_take_sequence_returns;
    });

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_TAKE_FAILED,
    rcl_take_sequence(&subscription, seq_size, &messages, &message_infos, allocation));
  rcl_reset_error();

  rmw_take_sequence_returns = RMW_RET_BAD_ALLOC;
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC,
    rcl_take_sequence(&subscription, seq_size, &messages, &message_infos, allocation));
  rcl_reset_error();

  rmw_take_sequence_returns = RMW_RET_ERROR;
  EXPECT_EQ(
    RCL_RET_ERROR,
    rcl_take_sequence(&subscription, seq_size, &messages, &message_infos, allocation));
  rcl_reset_error();
}

/* Test for all failure modes in subscription get_publisher_count function.
 */
TEST_F(CLASSNAME(TestSubscriptionFixtureInit, RMW_IMPLEMENTATION), test_bad_get_publisher_count) {
  size_t publisher_count = 0;
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_get_publisher_count(nullptr, &publisher_count));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_get_publisher_count(&subscription_zero_init, &publisher_count));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_get_publisher_count(&subscription, nullptr));
  rcl_reset_error();

  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rmw_subscription_count_matched_publishers, RMW_RET_ERROR);
  EXPECT_EQ(
    RCL_RET_ERROR,
    rcl_subscription_get_publisher_count(&subscription, &publisher_count));
  rcl_reset_error();
}

/* Using bad arguments subscription methods
 */
TEST_F(CLASSNAME(TestSubscriptionFixtureInit, RMW_IMPLEMENTATION), test_subscription_bad_argument) {
  EXPECT_EQ(NULL, rcl_subscription_get_actual_qos(nullptr));
  rcl_reset_error();
  EXPECT_FALSE(rcl_subscription_can_loan_messages(nullptr));
  rcl_reset_error();
  EXPECT_EQ(NULL, rcl_subscription_get_rmw_handle(nullptr));
  rcl_reset_error();
  EXPECT_EQ(NULL, rcl_subscription_get_topic_name(nullptr));
  rcl_reset_error();
  EXPECT_EQ(NULL, rcl_subscription_get_options(nullptr));
  rcl_reset_error();
  EXPECT_FALSE(rcl_subscription_is_cft_enabled(nullptr));
  rcl_reset_error();

  EXPECT_EQ(NULL, rcl_subscription_get_actual_qos(&subscription_zero_init));
  rcl_reset_error();
  EXPECT_FALSE(rcl_subscription_can_loan_messages(&subscription_zero_init));
  rcl_reset_error();
  EXPECT_EQ(NULL, rcl_subscription_get_rmw_handle(&subscription_zero_init));
  rcl_reset_error();
  EXPECT_EQ(NULL, rcl_subscription_get_topic_name(&subscription_zero_init));
  rcl_reset_error();
  EXPECT_EQ(NULL, rcl_subscription_get_options(&subscription_zero_init));
  rcl_reset_error();
  EXPECT_FALSE(rcl_subscription_is_cft_enabled(&subscription_zero_init));
  rcl_reset_error();
}

/* Test for all failure modes in rcl_subscription_set_content_filter function.
 */
TEST_F(
  CLASSNAME(
    TestSubscriptionFixtureInit,
    RMW_IMPLEMENTATION), test_bad_rcl_subscription_set_content_filter) {
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_set_content_filter(nullptr, nullptr));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_set_content_filter(&subscription_zero_init, nullptr));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_set_content_filter(&subscription, nullptr));
  rcl_reset_error();

  // an options used later
  rcl_subscription_content_filter_options_t options =
    rcl_get_zero_initialized_subscription_content_filter_options();
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_subscription_content_filter_options_init(
      &subscription,
      "data = '0'",
      0,
      nullptr,
      &options
    )
  );
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_fini(&subscription, &options)
    );
  });

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_subscription_set_content_filter, RMW_RET_UNSUPPORTED);
    EXPECT_EQ(
      RMW_RET_UNSUPPORTED,
      rcl_subscription_set_content_filter(
        &subscription, &options));
    rcl_reset_error();
  }

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_subscription_set_content_filter, RMW_RET_ERROR);
    EXPECT_EQ(
      RMW_RET_ERROR,
      rcl_subscription_set_content_filter(
        &subscription, &options));
    rcl_reset_error();
  }
}

/* Test for all failure modes in rcl_subscription_get_content_filter function.
 */
TEST_F(
  CLASSNAME(
    TestSubscriptionFixtureInit,
    RMW_IMPLEMENTATION), test_bad_rcl_subscription_get_content_filter) {
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_get_content_filter(nullptr, nullptr));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_get_content_filter(&subscription_zero_init, nullptr));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_get_content_filter(&subscription, nullptr));
  rcl_reset_error();

  rcl_subscription_content_filter_options_t options =
    rcl_get_zero_initialized_subscription_content_filter_options();

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_subscription_get_content_filter, RMW_RET_UNSUPPORTED);
    EXPECT_EQ(
      RMW_RET_UNSUPPORTED,
      rcl_subscription_get_content_filter(
        &subscription, &options));
    rcl_reset_error();
  }

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_subscription_get_content_filter, RMW_RET_ERROR);
    EXPECT_EQ(
      RMW_RET_ERROR,
      rcl_subscription_get_content_filter(
        &subscription, &options));
    rcl_reset_error();
  }
}

TEST_F(CLASSNAME(TestSubscriptionFixture, RMW_IMPLEMENTATION), test_init_fini_maybe_fail)
{
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic[] = "chatter";
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  rcl_subscription_t subscription = rcl_get_zero_initialized_subscription();

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_ret_t ret = rcl_subscription_init(
      &subscription, this->node_ptr, ts, topic, &subscription_options);

    if (RCL_RET_OK == ret) {
      EXPECT_TRUE(rcl_subscription_is_valid(&subscription));
      ret = rcl_subscription_fini(&subscription, this->node_ptr);
      if (RCL_RET_OK != ret) {
        // If fault injection caused fini to fail, we should try it again.
        EXPECT_EQ(RCL_RET_OK, rcl_subscription_fini(&subscription, this->node_ptr));
      }
    } else {
      EXPECT_TRUE(rcl_error_is_set());
      rcl_reset_error();
    }
  });
}
