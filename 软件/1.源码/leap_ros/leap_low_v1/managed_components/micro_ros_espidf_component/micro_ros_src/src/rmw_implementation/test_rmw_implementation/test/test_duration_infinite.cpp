// Copyright 2021 Amazon.com Inc or its affiliates. All rights reserved.
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

class CLASSNAME (TestDurationInfinite, RMW_IMPLEMENTATION) : public ::testing::Test
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
    constexpr char node_name[] = "infinite_duration_test_node";
    constexpr char node_namespace[] = "/inifinite_duration_test_ns";
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

TEST_F(CLASSNAME(TestDurationInfinite, RMW_IMPLEMENTATION), create_publisher)
{
  rmw_ret_t ret = RMW_RET_ERROR;
  size_t match_count = 0;
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  constexpr char topic_name[] = "/test";

  rmw_publisher_options_t pub_options = rmw_get_default_publisher_options();
  rmw_qos_profile_t offer_qos = rmw_qos_profile_default;
  offer_qos.deadline = RMW_DURATION_INFINITE;
  offer_qos.lifespan = RMW_DURATION_INFINITE;
  offer_qos.liveliness_lease_duration = RMW_DURATION_INFINITE;

  rmw_subscription_options_t sub_options = rmw_get_default_subscription_options();
  rmw_qos_profile_t request_qos = rmw_qos_profile_default;
  request_qos.deadline = RMW_DURATION_INFINITE;
  request_qos.lifespan = RMW_DURATION_INFINITE;
  request_qos.liveliness_lease_duration = RMW_DURATION_INFINITE;

  // Must be able to successfully create a publisher with these offered values
  rmw_publisher_t * pub =
    rmw_create_publisher(node, ts, topic_name, &offer_qos, &pub_options);
  ASSERT_NE(nullptr, pub) << rmw_get_error_string().str;

  // Must be able to match with a subscription requesting infinite durations
  rmw_subscription_t * sub =
    rmw_create_subscription(node, ts, topic_name, &request_qos, &sub_options);
  ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;

  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    ret = rmw_publisher_count_matched_subscriptions(pub, &match_count);
    if (RMW_RET_OK == ret && 1u == match_count) {
      break;
    }
  }
  ret = rmw_publisher_count_matched_subscriptions(pub, &match_count);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(1u, match_count);

  ret = rmw_destroy_subscription(node, sub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  ret = rmw_destroy_publisher(node, pub);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}
