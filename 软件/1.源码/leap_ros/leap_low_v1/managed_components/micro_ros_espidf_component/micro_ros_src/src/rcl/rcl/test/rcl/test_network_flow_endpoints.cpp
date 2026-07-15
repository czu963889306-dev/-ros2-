// Copyright 2020 Ericsson AB
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

#include "mimick/mimick.h"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcl/network_flow_endpoints.h"
#include "rcl/publisher.h"
#include "rcl/rcl.h"
#include "rcl/subscription.h"
#include "test_msgs/msg/basic_types.h"

#include "./allocator_testing_utils.h"
#include "../mocking_utils/patch.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestNetworkFlowEndpointsNode, RMW_IMPLEMENTATION) : public ::testing::Test
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
    constexpr char name[] = "test_network_flow_endpoints_node";
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

class CLASSNAME (TestPublisherNetworkFlowEndpoints, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestNetworkFlowEndpointsNode, RMW_IMPLEMENTATION)
{
public:
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  const char * topic_1 = "chatter";
  const char * topic_2 = "mutter";
  const char * topic_3 = "sing";

  rcl_publisher_t publisher_1;
  rcl_publisher_t publisher_2;
  rcl_publisher_t publisher_3;

  rcl_publisher_options_t publisher_1_options;
  rcl_publisher_options_t publisher_2_options;
  rcl_publisher_options_t publisher_3_options;

  void SetUp() override
  {
    CLASSNAME(TestNetworkFlowEndpointsNode, RMW_IMPLEMENTATION) ::SetUp();

    publisher_1 = rcl_get_zero_initialized_publisher();
    publisher_1_options = rcl_publisher_get_default_options();
    rcl_ret_t ret = rcl_publisher_init(
      &publisher_1, this->node_ptr, ts, topic_1, &publisher_1_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    publisher_2 = rcl_get_zero_initialized_publisher();
    publisher_2_options = rcl_publisher_get_default_options();
    publisher_2_options.rmw_publisher_options.require_unique_network_flow_endpoints =
      RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_STRICTLY_REQUIRED;
    ret = rcl_publisher_init(
      &publisher_2, this->node_ptr, ts, topic_2,
      &publisher_2_options);
    ASSERT_TRUE(
      ret == RCL_RET_OK || ret == RCL_RET_UNSUPPORTED ||
      ret == RCL_RET_ERROR) << rcl_get_error_string().str;
    rcl_reset_error();

    publisher_3 = rcl_get_zero_initialized_publisher();
    publisher_3_options = rcl_publisher_get_default_options();
    publisher_3_options.rmw_publisher_options.require_unique_network_flow_endpoints =
      RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_OPTIONALLY_REQUIRED;
    ret = rcl_publisher_init(
      &publisher_3, this->node_ptr, ts, topic_3,
      &publisher_3_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown() override
  {
    rcl_ret_t ret = rcl_publisher_fini(&publisher_1, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_publisher_fini(&publisher_2, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_publisher_fini(&publisher_3, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    CLASSNAME(TestNetworkFlowEndpointsNode, RMW_IMPLEMENTATION) ::TearDown();
  }
};

class CLASSNAME (TestSubscriptionNetworkFlowEndpoints, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestNetworkFlowEndpointsNode, RMW_IMPLEMENTATION)
{
public:
  const rosidl_message_type_support_t * ts =
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  const char * topic_1 = "chatter";
  const char * topic_2 = "mutter";
  const char * topic_3 = "sing";

  rcl_subscription_t subscription_1;
  rcl_subscription_t subscription_2;
  rcl_subscription_t subscription_3;

  rcl_subscription_options_t subscription_1_options;
  rcl_subscription_options_t subscription_2_options;
  rcl_subscription_options_t subscription_3_options;

  void SetUp() override
  {
    CLASSNAME(TestNetworkFlowEndpointsNode, RMW_IMPLEMENTATION) ::SetUp();

    subscription_1 = rcl_get_zero_initialized_subscription();
    subscription_1_options = rcl_subscription_get_default_options();
    rcl_ret_t ret = rcl_subscription_init(
      &subscription_1, this->node_ptr, ts, topic_1, &subscription_1_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    subscription_2 = rcl_get_zero_initialized_subscription();
    subscription_2_options = rcl_subscription_get_default_options();
    subscription_2_options.rmw_subscription_options.require_unique_network_flow_endpoints =
      RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_STRICTLY_REQUIRED;
    ret = rcl_subscription_init(
      &subscription_2, this->node_ptr, ts, topic_2,
      &subscription_2_options);
    ASSERT_TRUE(
      ret == RCL_RET_OK || ret == RCL_RET_UNSUPPORTED ||
      ret == RCL_RET_ERROR) << rcl_get_error_string().str;
    rcl_reset_error();

    subscription_3 = rcl_get_zero_initialized_subscription();
    subscription_3_options = rcl_subscription_get_default_options();
    subscription_3_options.rmw_subscription_options.require_unique_network_flow_endpoints =
      RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_OPTIONALLY_REQUIRED;
    ret = rcl_subscription_init(
      &subscription_3, this->node_ptr, ts, topic_3,
      &subscription_3_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown() override
  {
    rcl_ret_t ret = rcl_subscription_fini(&subscription_1, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_subscription_fini(&subscription_2, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_subscription_fini(&subscription_3, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    CLASSNAME(TestNetworkFlowEndpointsNode, RMW_IMPLEMENTATION) ::TearDown();
  }
};

TEST_F(
  CLASSNAME(
    TestPublisherNetworkFlowEndpoints,
    RMW_IMPLEMENTATION), test_publisher_get_network_flow_endpoints_errors) {
  rcl_ret_t ret;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_allocator_t failing_allocator = get_failing_allocator();
  rcl_network_flow_endpoint_array_t network_flow_endpoint_array =
    rcl_get_zero_initialized_network_flow_endpoint_array();

  // Invalid publisher
  ret = rcl_publisher_get_network_flow_endpoints(
    nullptr, &allocator, &network_flow_endpoint_array);
  EXPECT_TRUE(ret == RCL_RET_INVALID_ARGUMENT || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();

  // Invalid allocator
  ret = rcl_publisher_get_network_flow_endpoints(
    &this->publisher_1, nullptr, &network_flow_endpoint_array);
  EXPECT_TRUE(ret == RCL_RET_INVALID_ARGUMENT || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();

  // Invalid network_flow_endpoint_array
  ret = rcl_publisher_get_network_flow_endpoints(
    &this->publisher_1, &allocator, nullptr);
  EXPECT_TRUE(ret == RCL_RET_INVALID_ARGUMENT || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();

  // Failing allocator
  set_failing_allocator_is_failing(failing_allocator, true);
  ret = rcl_publisher_get_network_flow_endpoints(
    &this->publisher_1, &failing_allocator, &network_flow_endpoint_array);
  EXPECT_TRUE(ret == RCL_RET_BAD_ALLOC || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();

  // Non-zero network_flow_endpoint_array
  network_flow_endpoint_array.size = 1;
  ret = rcl_publisher_get_network_flow_endpoints(
    &this->publisher_1, &allocator, &network_flow_endpoint_array);
  EXPECT_TRUE(ret == RCL_RET_ERROR || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();
}

TEST_F(
  CLASSNAME(
    TestPublisherNetworkFlowEndpoints,
    RMW_IMPLEMENTATION), test_publisher_get_network_flow_endpoints) {
  rcl_ret_t ret_1;
  rcl_ret_t ret_2;
  rcl_allocator_t allocator = rcl_get_default_allocator();

  // Get network flow endpoints of ordinary publisher
  rcl_network_flow_endpoint_array_t network_flow_endpoint_array_1 =
    rcl_get_zero_initialized_network_flow_endpoint_array();
  ret_1 = rcl_publisher_get_network_flow_endpoints(
    &this->publisher_1, &allocator, &network_flow_endpoint_array_1);
  EXPECT_TRUE(ret_1 == RCL_RET_OK || ret_1 == RCL_RET_UNSUPPORTED);

  // Get network flow endpoints of publisher with unique network flow endpoints
  rcl_network_flow_endpoint_array_t network_flow_endpoint_array_2 =
    rcl_get_zero_initialized_network_flow_endpoint_array();
  if (rcl_publisher_is_valid(&this->publisher_2)) {
    rcl_network_flow_endpoint_array_t network_flow_endpoint_array_2 =
      rcl_get_zero_initialized_network_flow_endpoint_array();
    ret_2 = rcl_publisher_get_network_flow_endpoints(
      &this->publisher_2, &allocator, &network_flow_endpoint_array_2);
    EXPECT_TRUE(ret_2 == RCL_RET_OK || ret_2 == RCL_RET_UNSUPPORTED);
  } else {
    ret_2 = RCL_RET_ERROR;
  }

  if (ret_1 == RCL_RET_OK && ret_2 == RCL_RET_OK) {
    // Expect network flow endpoints not to be same
    for (size_t i = 0; i < network_flow_endpoint_array_1.size; i++) {
      for (size_t j = 0; j < network_flow_endpoint_array_2.size; j++) {
        bool strcmp_ret = false;
        if (strcmp(
            network_flow_endpoint_array_1.network_flow_endpoint[i].internet_address,
            network_flow_endpoint_array_2.network_flow_endpoint[j].internet_address) == 0)
        {
          strcmp_ret = true;
        }
        EXPECT_FALSE(
          network_flow_endpoint_array_1.network_flow_endpoint[i].transport_protocol ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].transport_protocol &&
          network_flow_endpoint_array_1.network_flow_endpoint[i].internet_protocol ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].internet_protocol &&
          network_flow_endpoint_array_1.network_flow_endpoint[i].transport_port ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].transport_port &&
          network_flow_endpoint_array_1.network_flow_endpoint[i].flow_label ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].flow_label &&
          network_flow_endpoint_array_1.network_flow_endpoint[i].dscp ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].dscp &&
          strcmp_ret);
      }
    }
  }

  // Release resources
  rcl_network_flow_endpoint_array_fini(&network_flow_endpoint_array_1);
  rcl_network_flow_endpoint_array_fini(&network_flow_endpoint_array_2);
}

TEST_F(
  CLASSNAME(
    TestSubscriptionNetworkFlowEndpoints,
    RMW_IMPLEMENTATION), test_subscription_get_network_flow_endpoints_errors) {
  rcl_ret_t ret;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_allocator_t failing_allocator = get_failing_allocator();
  rcl_network_flow_endpoint_array_t network_flow_endpoint_array =
    rcl_get_zero_initialized_network_flow_endpoint_array();

  // Invalid subscription
  ret = rcl_subscription_get_network_flow_endpoints(
    nullptr, &allocator, &network_flow_endpoint_array);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  // Invalid allocator
  ret = rcl_subscription_get_network_flow_endpoints(
    &this->subscription_1, nullptr, &network_flow_endpoint_array);
  EXPECT_TRUE(ret == RCL_RET_INVALID_ARGUMENT || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();

  // Invalid network_flow_endpoint_array
  ret = rcl_subscription_get_network_flow_endpoints(
    &this->subscription_1, &allocator, nullptr);
  EXPECT_TRUE(ret == RCL_RET_INVALID_ARGUMENT || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();

  // Failing allocator
  set_failing_allocator_is_failing(failing_allocator, true);
  ret = rcl_subscription_get_network_flow_endpoints(
    &this->subscription_1, &failing_allocator, &network_flow_endpoint_array);
  EXPECT_TRUE(ret == RCL_RET_BAD_ALLOC || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();

  // Non-zero network_flow_endpoint_array
  network_flow_endpoint_array.size = 1;
  ret = rcl_subscription_get_network_flow_endpoints(
    &this->subscription_1, &allocator, &network_flow_endpoint_array);
  EXPECT_TRUE(ret == RCL_RET_ERROR || ret == RCL_RET_UNSUPPORTED);
  rcl_reset_error();
}

TEST_F(
  CLASSNAME(
    TestSubscriptionNetworkFlowEndpoints,
    RMW_IMPLEMENTATION), test_subscription_get_network_flow_endpoints) {
  rcl_ret_t ret_1;
  rcl_ret_t ret_2;
  rcl_allocator_t allocator = rcl_get_default_allocator();

  // Get network flow endpoints of ordinary subscription
  rcl_network_flow_endpoint_array_t network_flow_endpoint_array_1 =
    rcl_get_zero_initialized_network_flow_endpoint_array();
  ret_1 = rcl_subscription_get_network_flow_endpoints(
    &this->subscription_1, &allocator, &network_flow_endpoint_array_1);
  EXPECT_TRUE(ret_1 == RCL_RET_OK || ret_1 == RCL_RET_UNSUPPORTED);

  // Get network flow endpoints of subscription with unique network flow endpoints
  rcl_network_flow_endpoint_array_t network_flow_endpoint_array_2 =
    rcl_get_zero_initialized_network_flow_endpoint_array();
  if (rcl_subscription_is_valid(&this->subscription_2)) {
    rcl_network_flow_endpoint_array_t network_flow_endpoint_array_2 =
      rcl_get_zero_initialized_network_flow_endpoint_array();
    ret_2 = rcl_subscription_get_network_flow_endpoints(
      &this->subscription_2, &allocator, &network_flow_endpoint_array_2);
    EXPECT_TRUE(ret_2 == RCL_RET_OK || ret_2 == RCL_RET_UNSUPPORTED);
  } else {
    ret_2 = RCL_RET_ERROR;
  }

  if (ret_1 == RCL_RET_OK && ret_2 == RCL_RET_OK) {
    // Expect network flow endpoints not to be same
    for (size_t i = 0; i < network_flow_endpoint_array_1.size; i++) {
      for (size_t j = 0; j < network_flow_endpoint_array_2.size; j++) {
        bool strcmp_ret = false;
        if (strcmp(
            network_flow_endpoint_array_1.network_flow_endpoint[i].internet_address,
            network_flow_endpoint_array_2.network_flow_endpoint[j].internet_address) == 0)
        {
          strcmp_ret = true;
        }
        EXPECT_FALSE(
          network_flow_endpoint_array_1.network_flow_endpoint[i].transport_protocol ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].transport_protocol &&
          network_flow_endpoint_array_1.network_flow_endpoint[i].internet_protocol ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].internet_protocol &&
          network_flow_endpoint_array_1.network_flow_endpoint[i].transport_port ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].transport_port &&
          network_flow_endpoint_array_1.network_flow_endpoint[i].flow_label ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].flow_label &&
          network_flow_endpoint_array_1.network_flow_endpoint[i].dscp ==
          network_flow_endpoint_array_2.network_flow_endpoint[j].dscp &&
          strcmp_ret);
      }
    }
  }

  // Release resources
  rcl_network_flow_endpoint_array_fini(&network_flow_endpoint_array_1);
  rcl_network_flow_endpoint_array_fini(&network_flow_endpoint_array_2);
}
