// Copyright 2021 Open Source Robotics Foundation, Inc.
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

#include <thread>
#include <chrono>
#include <string>

#include "rcl/allocator.h"
#include "rcl/publisher.h"
#include "rcl/subscription.h"
#include "rcpputils/filesystem_helper.hpp"
#include "rcutils/env.h"

#include "rcl/rcl.h"
#include "test_msgs/msg/strings.h"
#include "test_msgs/msg/basic_types.h"
#include "rosidl_runtime_c/string_functions.h"
#include "wait_for_entity_helpers.hpp"

#include "mimick/mimick.h"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif


/* This class is used for test_wait_for_all_acked
 */
class CLASSNAME (TestPublisherFixtureSpecial, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  rcl_context_t * context_ptr;
  rcl_node_t * node_ptr;

  void SetUp()
  {
    bool is_fastdds = (std::string(rmw_get_implementation_identifier()).find("rmw_fastrtps") == 0);

    if (is_fastdds) {
      // By default, fastdds use intraprocess mode in this scenario. But this leads to high-speed
      // data transmission. test_wait_for_all_acked need low data transmission. So disable this
      // mode via fastdds profile file.
      rcpputils::fs::path fastdds_profile(TEST_RESOURCES_DIRECTORY);
      fastdds_profile /= "test_profile/disable_intraprocess.xml";
      ASSERT_EQ(
        rcutils_set_env("FASTRTPS_DEFAULT_PROFILES_FILE", fastdds_profile.string().c_str()),
        true);
    }

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
    constexpr char name[] = "test_publisher_node2";
    rcl_node_options_t node_options = rcl_node_get_default_options();
    ret = rcl_node_init(this->node_ptr, name, "", this->context_ptr, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown()
  {
    rcutils_set_env("FASTRTPS_DEFAULT_PROFILES_FILE", NULL);
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

#define INIT_SUBSCRIPTION(idx) \
  rcl_subscription_t subscription ## idx = rcl_get_zero_initialized_subscription(); \
  ret = rcl_subscription_init( \
    &subscription ## idx, \
    this->node_ptr, \
    ts, \
    topic_name, \
    &subscription_options); \
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT( \
  { \
    ret = rcl_subscription_fini(&subscription ## idx, this->node_ptr); \
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
  });

#define ONE_MEGABYTE (1024 * 1024)

TEST_F(CLASSNAME(TestPublisherFixtureSpecial, RMW_IMPLEMENTATION), test_wait_for_all_acked) {
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic_name[] = "test_wait_for_all_acked";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  publisher_options.qos.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  publisher_options.qos.depth = 10000;
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher, this->node_ptr);
    if (ret != RCL_RET_OK) {
      FAIL() << rcl_get_error_string().str;
    }
  });

  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
  subscription_options.qos.depth = 1;
  subscription_options.qos.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;

  INIT_SUBSCRIPTION(1)
  INIT_SUBSCRIPTION(2)
  INIT_SUBSCRIPTION(3)

  ASSERT_TRUE(wait_for_established_subscription(&publisher, 10, 100));

  rcl_allocator_t allocator = rcl_get_default_allocator();
  char * test_string = static_cast<char *>(allocator.allocate(ONE_MEGABYTE, allocator.state));
  ASSERT_TRUE(test_string != NULL);
  memset(test_string, 'a', ONE_MEGABYTE);
  test_string[ONE_MEGABYTE - 1] = '\0';
  test_msgs__msg__Strings msg;
  test_msgs__msg__Strings__init(&msg);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_string, allocator.state);
    test_msgs__msg__Strings__fini(&msg);
  });
  ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));

  ret = rcl_publish(&publisher, &msg, nullptr);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription1, context_ptr, 10, 100));
  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription2, context_ptr, 10, 100));
  ASSERT_TRUE(wait_for_subscription_to_be_ready(&subscription3, context_ptr, 10, 100));

  int i = 0;
  for (; i < 500; i++) {
    ret = rcl_publish(&publisher, &msg, nullptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  ret = rcl_publisher_wait_for_all_acked(
    &publisher,
    RCL_MS_TO_NS(500));
  EXPECT_TRUE(ret == RCL_RET_OK || ret == RCL_RET_TIMEOUT);

  ret = rcl_publisher_wait_for_all_acked(&publisher, -1);
  EXPECT_EQ(RCL_RET_OK, ret);
}

TEST_F(
  CLASSNAME(TestPublisherFixtureSpecial, RMW_IMPLEMENTATION),
  test_wait_for_all_acked_with_best_effort)
{
  rcl_ret_t ret;
  rcl_publisher_t publisher = rcl_get_zero_initialized_publisher();
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic_name[] = "test_wait_for_all_acked_with_best_effort";
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  publisher_options.qos.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  publisher_options.qos.depth = 10000;
  ret = rcl_publisher_init(&publisher, this->node_ptr, ts, topic_name, &publisher_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_publisher_wait_for_all_acked(
    &publisher,
    RCL_MS_TO_NS(500));
  EXPECT_EQ(RCL_RET_OK, ret);

  ret = rcl_publisher_fini(&publisher, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}
