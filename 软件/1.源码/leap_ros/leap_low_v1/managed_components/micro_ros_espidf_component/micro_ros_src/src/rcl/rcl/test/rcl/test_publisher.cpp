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

#include "rcl/publisher.h"

#include "rcl/rcl.h"
#include "test_msgs/msg/basic_types.h"
#include "test_msgs/msg/strings.h"
#include "rosidl_runtime_c/string_functions.h"

#include "mimick/mimick.h"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcl/node.h"
#include "rcutils/env.h"
#include "rmw/validate_full_topic_name.h"
#include "rmw/validate_node_name.h"

#include "./failing_allocator_functions.hpp"
#include "./publisher_impl.h"
#include "../mocking_utils/patch.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestPublisherFixture, RMW_IMPLEMENTATION) : public ::testing::Test
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
    constexpr char name[] = "test_publisher_node";
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

class CLASSNAME (TestPublisherFixtureInit, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION)
{
public:
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  const char * topic_name = "chatter";
  rcl_publisher_t publisher;
  rcl_publisher_options_t publisher_options;

  void SetUp() override
  {
    CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION) ::SetUp();
    publisher = rcl_get_zero_initialized_publisher();
    publisher_options = rcl_publisher_get_default_options();
    rcl_ret_t ret = rcl_publisher_init(
      &publisher, this->node_ptr, ts, topic_name, &publisher_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown() override
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION) ::TearDown();
  }
};

/* Basic nominal test of a publisher.
 */
TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_publisher_nominal) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic_name[] = "chatter";
  constexpr char expected_topic_name[] = "/chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  EXPECT_EQ(strcmp(rcl_publisher_get_topic_name(&publisher), expected_topic_name), 0);
  test_msgs__msg__BasicTypes msg;
  test_msgs__msg__BasicTypes__init(&msg);
  msg.int64_value = 42;
  ret = rcl_publish(&publisher, &msg, nullptr);
  test_msgs__msg__BasicTypes__fini(&msg);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

/* Basic nominal test of a publisher with a string.
 */
TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_publisher_nominal_string) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic_name[] = "chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  test_msgs__msg__Strings msg;
  test_msgs__msg__Strings__init(&msg);
  ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, "testing"));
  ret = rcl_publish(&publisher, &msg, nullptr);
  test_msgs__msg__Strings__fini(&msg);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

/* Test two publishers using different message types with the same basename.
 *
 * Regression test for https://github.com/ros2/rmw_connext/issues/234, where rmw_connext_cpp could
 * not support publishers on topics with the same basename (but different namespaces) using
 * different message types, because at the time partitions were used for implementing namespaces.
 */
TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_publishers_different_types) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts_int =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  const char * topic_name = "basename";
  const char * expected_topic_name = "/basename";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts_int, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  EXPECT_EQ(strcmp(rcl_publisher_get_topic_name(&publisher), expected_topic_name), 0);

  rcl_publisher_t publisher_in_namespace = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts_string = ROSIDL_GET_MSG_TYPE_SUPPORT(
    test_msgs, msg, Strings);
  topic_name = "namespace/basename";
  expected_topic_name = "/namespace/basename";
  ret = rcl_publisher_init(
    &publisher_in_namespace, this->node_ptr, ts_string, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher_in_namespace, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  EXPECT_EQ(strcmp(rcl_publisher_get_topic_name(&publisher_in_namespace), expected_topic_name), 0);

  test_msgs__msg__BasicTypes msg_int;
  test_msgs__msg__BasicTypes__init(&msg_int);
  msg_int.int64_value = 42;
  ret = rcl_publish(&publisher, &msg_int, nullptr);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  test_msgs__msg__BasicTypes__fini(&msg_int);

  test_msgs__msg__Strings msg_string;
  test_msgs__msg__Strings__init(&msg_string);
  ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg_string.string_value, "testing"));
  ret = rcl_publish(&publisher_in_namespace, &msg_string, nullptr);
  test_msgs__msg__Strings__fini(&msg_string);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

/* Testing the publisher init and fini functions.
 */
TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_publisher_init_fini) {
  rcl_ret_t ret;
  // Setup valid inputs.
  rcl_publisher_t publisher;
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic_name[] = "chatter";
  rcl_publisher_options_t default_publisher_options = rcl_publisher_get_default_options();

  // Check if null publisher is valid
  EXPECT_FALSE(rcl_publisher_is_valid(nullptr));
  rcl_reset_error();

  // Check if zero initialized node is valid
  publisher = rcl_get_zero_initialized_publisher();
  EXPECT_FALSE(rcl_publisher_is_valid(&publisher));
  rcl_reset_error();

  // Check that valid publisher is valid
  publisher = rcl_get_zero_initialized_publisher();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &default_publisher_options);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_publisher_is_valid(&publisher));
  // Try init a publisher already init
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &default_publisher_options);
  EXPECT_EQ(RCL_RET_ALREADY_INIT, ret) << rcl_get_error_string().str;
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Pass invalid node to fini
  ret = rcl_publisher_fini(&publisher, nullptr);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Pass nullptr publisher to fini
  ret = rcl_publisher_fini(nullptr, this->node_ptr);
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Try passing null for publisher in init.
  ret = rcl_publisher_init(nullptr, this->node_ptr, ts, topic_name, &default_publisher_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Try passing null for a node pointer in init.
  publisher = rcl_get_zero_initialized_publisher();
  ret = rcl_publisher_init(&publisher, nullptr, ts, topic_name, &default_publisher_options);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Try passing an invalid (uninitialized) node in init.
  publisher = rcl_get_zero_initialized_publisher();
  rcl_node_t invalid_node = rcl_get_zero_initialized_node();
  ret = rcl_publisher_init(&publisher, &invalid_node, ts, topic_name, &default_publisher_options);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << rcl_get_error_string().str;
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Try passing null for the type support in init.
  publisher = rcl_get_zero_initialized_publisher();
  ret = rcl_publisher_init(
    &publisher, this->node_ptr, nullptr, topic_name, &default_publisher_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Try passing null for the topic name in init.
  publisher = rcl_get_zero_initialized_publisher();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, nullptr, &default_publisher_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Try passing null for the options in init.
  publisher = rcl_get_zero_initialized_publisher();
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Try passing options with an invalid allocate in allocator with init.
  publisher = rcl_get_zero_initialized_publisher();
  rcl_publisher_options_t publisher_options_with_invalid_allocator;
  publisher_options_with_invalid_allocator = rcl_publisher_get_default_options();
  publisher_options_with_invalid_allocator.allocator.allocate = nullptr;
  ret = rcl_publisher_init(
    &publisher, this->node_ptr, ts, topic_name, &publisher_options_with_invalid_allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Try passing options with an invalid deallocate in allocator with init.
  publisher = rcl_get_zero_initialized_publisher();
  publisher_options_with_invalid_allocator = rcl_publisher_get_default_options();
  publisher_options_with_invalid_allocator.allocator.deallocate = nullptr;
  ret = rcl_publisher_init(
    &publisher, this->node_ptr, ts, topic_name, &publisher_options_with_invalid_allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // An allocator with an invalid realloc will probably work (so we will not test it).

  // Try passing options with a failing allocator with init.
  publisher = rcl_get_zero_initialized_publisher();
  rcl_publisher_options_t publisher_options_with_failing_allocator;
  publisher_options_with_failing_allocator = rcl_publisher_get_default_options();
  publisher_options_with_failing_allocator.allocator.allocate = failing_malloc;
  publisher_options_with_failing_allocator.allocator.reallocate = failing_realloc;
  publisher_options_with_failing_allocator.allocator.zero_allocate = failing_calloc;
  ret = rcl_publisher_init(
    &publisher, this->node_ptr, ts, topic_name, &publisher_options_with_failing_allocator);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_publisher_loan) {
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic_name[] = "chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  rcl_ret_t ret =
    rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  test_msgs__msg__Strings * msg_loaned = nullptr;
  test_msgs__msg__Strings ** msg_loaned_ptr = &msg_loaned;
  if (rcl_publisher_can_loan_messages(&publisher)) {
    EXPECT_EQ(
      RCL_RET_OK, rcl_borrow_loaned_message(
        &publisher,
        ts,
        reinterpret_cast<void **>(msg_loaned_ptr)));
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&(msg_loaned->string_value), "testing"));
    EXPECT_EQ(
      RCL_RET_OK, rcl_publish_loaned_message(
        &publisher,
        msg_loaned,
        nullptr));
  }
}

TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_publisher_loan_disable) {
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic_name[] = "pod_msg";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  rcl_ret_t ret =
    rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  if (rcl_publisher_can_loan_messages(&publisher)) {
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "0"));
    EXPECT_TRUE(rcl_publisher_can_loan_messages(&publisher));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "1"));
    EXPECT_FALSE(rcl_publisher_can_loan_messages(&publisher));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "2"));
    EXPECT_TRUE(rcl_publisher_can_loan_messages(&publisher));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "Unexpected"));
    EXPECT_TRUE(rcl_publisher_can_loan_messages(&publisher));
  } else {
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "0"));
    EXPECT_FALSE(rcl_publisher_can_loan_messages(&publisher));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "1"));
    EXPECT_FALSE(rcl_publisher_can_loan_messages(&publisher));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "2"));
    EXPECT_FALSE(rcl_publisher_can_loan_messages(&publisher));
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "Unexpected"));
    EXPECT_FALSE(rcl_publisher_can_loan_messages(&publisher));
  }
}

TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_invalid_publisher) {
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic_name[] = "chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  rcl_ret_t ret =
    rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  const rcl_publisher_options_t * publisher_options_rcv = rcl_publisher_get_options(&publisher);
  ASSERT_NE(nullptr, publisher_options_rcv);
  EXPECT_EQ(rmw_qos_profile_default.reliability, publisher_options_rcv->qos.reliability);
  EXPECT_EQ(rmw_qos_profile_default.history, publisher_options_rcv->qos.history);
  EXPECT_EQ(rmw_qos_profile_default.depth, publisher_options_rcv->qos.depth);
  EXPECT_EQ(rmw_qos_profile_default.durability, publisher_options_rcv->qos.durability);
  EXPECT_TRUE(rcutils_allocator_is_valid(&(publisher_options_rcv->allocator)));

  rmw_publisher_t * pub_rmw_handle = rcl_publisher_get_rmw_handle(&publisher);
  EXPECT_NE(nullptr, pub_rmw_handle);

  rcl_context_t * pub_context = rcl_publisher_get_context(&publisher);
  EXPECT_TRUE(rcl_context_is_valid(pub_context));
  EXPECT_EQ(rcl_context_get_instance_id(context_ptr), rcl_context_get_instance_id(pub_context));

  EXPECT_EQ(RCL_RET_OK, rcl_publisher_assert_liveliness(&publisher));

  EXPECT_EQ(RCL_RET_OK, rcl_publisher_wait_for_all_acked(&publisher, 0));

  size_t count_size;
  test_msgs__msg__BasicTypes msg;
  rcl_serialized_message_t serialized_msg = rmw_get_zero_initialized_serialized_message();
  rcl_publisher_impl_t * saved_impl = publisher.impl;
  rcl_context_t * saved_context = publisher.impl->context;
  rmw_publisher_t * saved_rmw_handle = publisher.impl->rmw_handle;
  rmw_publisher_allocation_t * null_allocation_is_valid_arg = nullptr;

  // Change internal context to nullptr
  publisher.impl->context = nullptr;
  EXPECT_TRUE(rcl_publisher_is_valid_except_context(&publisher));
  EXPECT_NE(nullptr, rcl_publisher_get_topic_name(&publisher));
  EXPECT_NE(nullptr, rcl_publisher_get_rmw_handle(&publisher));
  EXPECT_NE(nullptr, rcl_publisher_get_actual_qos(&publisher));
  EXPECT_NE(nullptr, rcl_publisher_get_options(&publisher));
  rcl_reset_error();
  EXPECT_FALSE(rcl_publisher_is_valid(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_context(&publisher));
  rcl_reset_error();
  EXPECT_FALSE(rcl_publisher_can_loan_messages(&publisher));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_PUBLISHER_INVALID, rcl_publisher_get_subscription_count(&publisher, &count_size));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publisher_assert_liveliness(&publisher));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publisher_wait_for_all_acked(&publisher, 10000000));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publish(&publisher, &msg, null_allocation_is_valid_arg));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_PUBLISHER_INVALID,
    rcl_publish_serialized_message(&publisher, &serialized_msg, null_allocation_is_valid_arg));
  rcl_reset_error();
  publisher.impl->context = saved_context;

  // nullptr arguments
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_publish(&publisher, nullptr, null_allocation_is_valid_arg));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_publish_serialized_message(&publisher, nullptr, null_allocation_is_valid_arg));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_publisher_get_subscription_count(&publisher, nullptr));
  rcl_reset_error();

  // Change internal rmw_handle to nullptr
  publisher.impl->rmw_handle = nullptr;
  EXPECT_FALSE(rcl_publisher_is_valid_except_context(&publisher));
  rcl_reset_error();
  EXPECT_FALSE(rcl_publisher_is_valid(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_topic_name(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_rmw_handle(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_actual_qos(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_options(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_context(&publisher));
  rcl_reset_error();
  EXPECT_FALSE(rcl_publisher_can_loan_messages(&publisher));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_PUBLISHER_INVALID, rcl_publisher_get_subscription_count(&publisher, &count_size));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publisher_assert_liveliness(&publisher));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publisher_wait_for_all_acked(&publisher, 10000000));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publish(&publisher, &msg, null_allocation_is_valid_arg));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_PUBLISHER_INVALID,
    rcl_publish_serialized_message(&publisher, &serialized_msg, null_allocation_is_valid_arg));
  rcl_reset_error();
  publisher.impl->rmw_handle = saved_rmw_handle;

  // Change internal implementation to nullptr
  publisher.impl = nullptr;
  EXPECT_FALSE(rcl_publisher_is_valid_except_context(&publisher));
  rcl_reset_error();
  EXPECT_FALSE(rcl_publisher_is_valid(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_topic_name(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_rmw_handle(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_actual_qos(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_options(&publisher));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_context(&publisher));
  rcl_reset_error();
  EXPECT_FALSE(rcl_publisher_can_loan_messages(&publisher));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_PUBLISHER_INVALID, rcl_publisher_get_subscription_count(&publisher, &count_size));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publisher_assert_liveliness(&publisher));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publisher_wait_for_all_acked(&publisher, 10000000));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publish(&publisher, &msg, null_allocation_is_valid_arg));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_PUBLISHER_INVALID,
    rcl_publish_serialized_message(&publisher, &serialized_msg, null_allocation_is_valid_arg));
  rcl_reset_error();
  publisher.impl = saved_impl;

  // Null tests
  EXPECT_FALSE(rcl_publisher_is_valid_except_context(nullptr));
  rcl_reset_error();
  EXPECT_FALSE(rcl_publisher_is_valid(nullptr));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_topic_name(nullptr));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_rmw_handle(nullptr));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_actual_qos(nullptr));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_options(nullptr));
  rcl_reset_error();
  EXPECT_EQ(nullptr, rcl_publisher_get_context(nullptr));
  rcl_reset_error();
  EXPECT_FALSE(rcl_publisher_can_loan_messages(nullptr));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_PUBLISHER_INVALID, rcl_publisher_get_subscription_count(nullptr, &count_size));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publisher_assert_liveliness(nullptr));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publisher_wait_for_all_acked(nullptr, 10000000));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_publish(nullptr, &msg, null_allocation_is_valid_arg));
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_PUBLISHER_INVALID,
    rcl_publish_serialized_message(nullptr, &serialized_msg, null_allocation_is_valid_arg));
  rcl_reset_error();
}

// Mocking rmw_publisher_count_matched_subscriptions to make
// rcl_publisher_get_subscription_count fail
TEST_F(
  CLASSNAME(TestPublisherFixtureInit, RMW_IMPLEMENTATION),
  test_mock_publisher_get_subscription_count)
{
  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rmw_publisher_count_matched_subscriptions, RMW_RET_BAD_ALLOC);

  // Now normal usage of the function rcl_publisher_get_subscription_count returning
  // unexpected RMW_RET_BAD_ALLOC
  size_t count_size = 2u;
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC, rcl_publisher_get_subscription_count(&publisher, &count_size));
  EXPECT_EQ(2u, count_size);
  rcl_reset_error();
}

// Mocking rmw_publisher_assert_liveliness to make
// rcl_publisher_assert_liveliness fail
TEST_F(CLASSNAME(TestPublisherFixtureInit, RMW_IMPLEMENTATION), test_mock_assert_liveliness) {
  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rmw_publisher_assert_liveliness, RMW_RET_ERROR);

  // Now normal usage of the function rcl_publisher_assert_liveliness returning
  // unexpected RMW_RET_ERROR
  EXPECT_EQ(
    RCL_RET_ERROR, rcl_publisher_assert_liveliness(&publisher));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}

// Mocking rmw_publisher_wait_for_all_acked to make
// rcl_publisher_wait_for_all_acked fail
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rmw_time_t, ==)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rmw_time_t, !=)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rmw_time_t, <)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rmw_time_t, >)

TEST_F(
  CLASSNAME(TestPublisherFixtureInit, RMW_IMPLEMENTATION),
  test_mock_assert_wait_for_all_acked)
{
#define CHECK_PUBLISHER_WAIT_FOR_ALL_ACKED_RETURN(RMW_RET_RESULT, EXPECT_RET)   do { \
    rmw_publisher_wait_for_all_acked_return = RMW_RET_RESULT; \
    ret = rcl_publisher_wait_for_all_acked(&publisher, 1000000); \
    EXPECT_EQ(EXPECT_RET, ret); \
    rcl_reset_error(); \
} while (0)

  rcl_ret_t ret;
  rmw_ret_t rmw_publisher_wait_for_all_acked_return;
  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rmw_publisher_wait_for_all_acked, rmw_publisher_wait_for_all_acked_return);

  {
    // Now normal usage of the function rcl_publisher_wait_for_all_acked returning
    // unexpected RMW_RET_TIMEOUT
    SCOPED_TRACE("Check RCL return failed !");
    CHECK_PUBLISHER_WAIT_FOR_ALL_ACKED_RETURN(RMW_RET_TIMEOUT, RCL_RET_TIMEOUT);
  }

  {
    // Now normal usage of the function rcl_publisher_wait_for_all_acked returning
    // unexpected RMW_RET_UNSUPPORTED
    SCOPED_TRACE("Check RCL return failed !");
    CHECK_PUBLISHER_WAIT_FOR_ALL_ACKED_RETURN(RMW_RET_UNSUPPORTED, RCL_RET_UNSUPPORTED);
  }

  {
    // Now normal usage of the function rcl_publisher_wait_for_all_acked returning
    // unexpected RMW_RET_INVALID_ARGUMENT
    SCOPED_TRACE("Check RCL return failed !");
    CHECK_PUBLISHER_WAIT_FOR_ALL_ACKED_RETURN(RMW_RET_INVALID_ARGUMENT, RCL_RET_ERROR);
  }

  {
    // Now normal usage of the function rcl_publisher_wait_for_all_acked returning
    // unexpected RMW_RET_INCORRECT_RMW_IMPLEMENTATION
    SCOPED_TRACE("Check RCL return failed !");
    CHECK_PUBLISHER_WAIT_FOR_ALL_ACKED_RETURN(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, RCL_RET_ERROR);
  }

  {
    // Now normal usage of the function rcl_publisher_wait_for_all_acked returning
    // unexpected RMW_RET_ERROR
    SCOPED_TRACE("Check RCL return failed !");
    CHECK_PUBLISHER_WAIT_FOR_ALL_ACKED_RETURN(RMW_RET_ERROR, RCL_RET_ERROR);
  }
}

// Mocking rmw_publish to make rcl_publish fail
TEST_F(CLASSNAME(TestPublisherFixtureInit, RMW_IMPLEMENTATION), test_mock_publish) {
  auto mock = mocking_utils::patch_and_return("lib:rcl", rmw_publish, RMW_RET_ERROR);

  // Test normal usage of the function rcl_publish returning unexpected RMW_RET_ERROR
  test_msgs__msg__BasicTypes msg;
  test_msgs__msg__BasicTypes__init(&msg);
  msg.int64_value = 42;
  rcl_ret_t ret = rcl_publish(&publisher, &msg, nullptr);
  test_msgs__msg__BasicTypes__fini(&msg);
  EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}

// Mocking rmw_publish_serialized_message to make rcl_publish_serialized_message fail
TEST_F(
  CLASSNAME(TestPublisherFixtureInit, RMW_IMPLEMENTATION), test_mock_publish_serialized_message)
{
  rcl_serialized_message_t serialized_msg = rmw_get_zero_initialized_serialized_message();
  size_t initial_size_serialized = 0u;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  ASSERT_EQ(
    RCL_RET_OK, rmw_serialized_message_init(
      &serialized_msg, initial_size_serialized, &allocator)) << rcl_get_error_string().str;
  constexpr char test_string[] = "testing";
  test_msgs__msg__Strings msg;
  test_msgs__msg__Strings__init(&msg);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__Strings__fini(&msg);
    ASSERT_EQ(
      RMW_RET_OK,
      rmw_serialized_message_fini(&serialized_msg)) << rcl_get_error_string().str;
  });

  ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
  ASSERT_STREQ(msg.string_value.data, test_string);
  rcl_ret_t ret = rmw_serialize(&msg, ts, &serialized_msg);
  ASSERT_EQ(RMW_RET_OK, ret);

  rmw_ret_t rmw_publish_serialized_return = RMW_RET_ERROR;
  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rmw_publish_serialized_message, rmw_publish_serialized_return);
  {
    // Test normal usage of the function rcl_publish_serialized_message
    // returning unexpected RMW_RET_ERROR
    ret = rcl_publish_serialized_message(&publisher, &serialized_msg, nullptr);
    EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    // Repeat, but now returning BAD_ALLOC
    rmw_publish_serialized_return = RMW_RET_BAD_ALLOC;
    ret = rcl_publish_serialized_message(&publisher, &serialized_msg, nullptr);
    EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
}

// Define dummy comparison operators for rcutils_allocator_t type for use with the Mimick Library
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, ==)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, <)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, >)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, !=)

TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_mock_publisher_init) {
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic_name[] = "chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  rcl_ret_t ret = RCL_RET_OK;

  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rcutils_string_map_init, RCUTILS_RET_ERROR);
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

TEST_F(
  CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_mock_publisher_init_fail_qos)
{
  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rmw_publisher_get_actual_qos, RMW_RET_ERROR);

  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic_name[] = "chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();

  rcl_ret_t ret =
    rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

// Tests for loaned msgs functions. Mocked as the rmw tier1 vendors don't support it
TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_mock_loaned_functions) {
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  rcl_publisher_t not_init_publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic_name[] = "chatter";
  constexpr char expected_topic_name[] = "/chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();

  rcl_ret_t ret = rcl_publisher_init(
    &publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  EXPECT_EQ(strcmp(rcl_publisher_get_topic_name(&publisher), expected_topic_name), 0);
  test_msgs__msg__BasicTypes msg;
  test_msgs__msg__BasicTypes__init(&msg);
  msg.int64_value = 42;
  void * msg_pointer = &msg;
  rmw_publisher_allocation_t * null_allocation_is_valid_arg = nullptr;

  {
    // mocked, publish nominal usage
    auto mock = mocking_utils::patch_and_return("lib:rcl", rmw_publish_loaned_message, RMW_RET_OK);
    EXPECT_EQ(RCL_RET_OK, rcl_publish_loaned_message(&publisher, &msg, nullptr));
  }
  {
    // bad params publish
    EXPECT_EQ(
      RCL_RET_PUBLISHER_INVALID,
      rcl_publish_loaned_message(nullptr, &msg, null_allocation_is_valid_arg));
    EXPECT_EQ(
      RCL_RET_PUBLISHER_INVALID,
      rcl_publish_loaned_message(&not_init_publisher, &msg, null_allocation_is_valid_arg));
    EXPECT_EQ(
      RCL_RET_INVALID_ARGUMENT,
      rcl_publish_loaned_message(&publisher, nullptr, null_allocation_is_valid_arg));
  }
  {
    // mocked, failure publish
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_publish_loaned_message, RMW_RET_ERROR);
    EXPECT_EQ(RCL_RET_ERROR, rcl_publish_loaned_message(&publisher, &msg, nullptr));
  }
  {
    // mocked, borrow loaned nominal usage
    auto mock = mocking_utils::patch_and_return("lib:rcl", rmw_borrow_loaned_message, RMW_RET_OK);
    EXPECT_EQ(RCL_RET_OK, rcl_borrow_loaned_message(&publisher, ts, &msg_pointer));
  }
  {
    // bad params borrow loaned
    EXPECT_EQ(RCL_RET_PUBLISHER_INVALID, rcl_borrow_loaned_message(nullptr, ts, &msg_pointer));
    EXPECT_EQ(
      RCL_RET_PUBLISHER_INVALID, rcl_borrow_loaned_message(&not_init_publisher, ts, &msg_pointer));
  }
  {
    // mocked, nominal return loaned message
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_return_loaned_message_from_publisher, RMW_RET_OK);
    EXPECT_EQ(RCL_RET_OK, rcl_return_loaned_message_from_publisher(&publisher, &msg));
  }
  {
    // bad params return loaned message
    EXPECT_EQ(
      RCL_RET_PUBLISHER_INVALID,
      rcl_return_loaned_message_from_publisher(nullptr, &msg));
    EXPECT_EQ(
      RCL_RET_PUBLISHER_INVALID,
      rcl_return_loaned_message_from_publisher(&not_init_publisher, &msg));
    EXPECT_EQ(
      RCL_RET_INVALID_ARGUMENT,
      rcl_return_loaned_message_from_publisher(&publisher, nullptr));
  }

  test_msgs__msg__BasicTypes__fini(&msg);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

// Tests mocking ini/fini functions for specific failures
TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_mocks_fail_publisher_init) {
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  constexpr char topic_name[] = "chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  rcl_ret_t ret = RCL_RET_OK;

  {
    // Internal rmw failure validating node name
    auto mock = mocking_utils::patch_and_return("lib:rcl", rmw_validate_node_name, RMW_RET_ERROR);
    ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
    EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
    rcl_reset_error();
  }
  {
    // Internal rmw failure validating node name
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_validate_node_name, RMW_RET_INVALID_ARGUMENT);
    ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
    EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
    rcl_reset_error();
  }
  {
    // Internal failure when fini rcutils_string_map returns error, targets substitution_map fini
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rcutils_string_map_fini, RCUTILS_RET_ERROR);
    ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
    EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
    rcl_reset_error();
  }
  {
    // Internal rmw failure validating topic name
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_validate_full_topic_name, RMW_RET_ERROR);
    ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
    EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
    rcl_reset_error();
  }
  {
    // Internal rmw failure validating node name, returns OK but the result is set to error
    auto mock = mocking_utils::patch(
      "lib:rcl", rmw_validate_full_topic_name, [](auto, int * result, auto) {
        *result = RMW_TOPIC_INVALID_NOT_ABSOLUTE;
        return RMW_RET_OK;
      });
    ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
    EXPECT_EQ(RCL_RET_TOPIC_NAME_INVALID, ret) << rcl_get_error_string().str;
    rcl_reset_error();
  }
}

// Test mocked fail fini publisher
TEST_F(CLASSNAME(TestPublisherFixture, RMW_IMPLEMENTATION), test_mock_publisher_fini_fail) {
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic_name[] = "chatter";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  rcl_ret_t ret = rcl_publisher_init(
    &publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Internal rmw failure destroying publisher
  auto mock = mocking_utils::patch_and_return("lib:rcl", rmw_destroy_publisher, RMW_RET_ERROR);
  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
}
