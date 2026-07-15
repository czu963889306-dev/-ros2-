// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <rosidl_runtime_c/message_type_support_struct.h>
#include <rmw/rmw.h>
#include <rmw/validate_namespace.h>
#include <rmw/validate_node_name.h>
#include <rmw_microxrcedds_c/config.h>

#include <vector>
#include <memory>
#include <string>

#include "./rmw_base_test.hpp"
#include "./test_utils.hpp"

class TestSubscription : public RMWBaseTest
{
protected:
  void SetUp() override
  {
    RMWBaseTest::SetUp();

    node = rmw_create_node(&test_context, "my_node", "/ns");
    ASSERT_NE(node, nullptr);
  }

  void TearDown() override
  {
    ASSERT_EQ(rmw_destroy_node(node), RMW_RET_OK);
    RMWBaseTest::TearDown();
  }

  rmw_node_t * node;

  const char * topic_type = "topic_type";
  const char * topic_name = "topic_name";
  const char * message_namespace = "package_name";

  size_t id_gen = 0;
};

/*
 * Testing subscription construction and destruction.
 */
TEST_F(TestSubscription, construction_and_destruction)
{
  dummy_type_support_t dummy_type_support;

  ConfigureDummyTypeSupport(
    topic_type,
    topic_type,
    message_namespace,
    id_gen++,
    &dummy_type_support);

  rmw_qos_profile_t dummy_qos_policies;
  ConfigureDefaultQOSPolices(&dummy_qos_policies);

  rmw_subscription_options_t default_subscription_options = rmw_get_default_subscription_options();

  rmw_subscription_t * sub = rmw_create_subscription(
    this->node,
    &dummy_type_support.type_support,
    topic_name,
    &dummy_qos_policies,
    &default_subscription_options);
  ASSERT_NE(sub, nullptr);

  rmw_ret_t ret = rmw_destroy_subscription(this->node, sub);
  ASSERT_EQ(ret, RMW_RET_OK);
}


/*
 * Testing node memory poll
 */
TEST_F(TestSubscription, memory_poll_multiple_topic)
{
  rmw_qos_profile_t dummy_qos_policies;

  ConfigureDefaultQOSPolices(&dummy_qos_policies);

  std::vector<dummy_type_support_t> dummy_type_supports;
  std::vector<rmw_subscription_t *> subscriptions;
  rmw_ret_t ret;
  rmw_subscription_t * subscription;

  rmw_subscription_options_t default_subscription_options = rmw_get_default_subscription_options();

  // Get all available nodes
  {
    for (size_t i = 0; i < RMW_UXRCE_MAX_SUBSCRIPTIONS; i++) {
      dummy_type_supports.push_back(dummy_type_support_t());
      ConfigureDummyTypeSupport(
        topic_type,
        topic_type,
        message_namespace,
        id_gen++,
        &dummy_type_supports.back());
      subscription = rmw_create_subscription(
        this->node,
        &dummy_type_supports.back().type_support,
        dummy_type_supports.back().topic_name.data(),
        &dummy_qos_policies,
        &default_subscription_options);
      ASSERT_NE(subscription, nullptr);
      subscriptions.push_back(subscription);
    }
  }


  // Try to get one
  {
    dummy_type_supports.push_back(dummy_type_support_t());
    ConfigureDummyTypeSupport(
      topic_type,
      topic_type,
      message_namespace,
      id_gen++,
      &dummy_type_supports.back());
    subscription = rmw_create_subscription(
      this->node,
      &dummy_type_supports.back().type_support,
      dummy_type_supports.back().topic_name.data(),
      &dummy_qos_policies,
      &default_subscription_options);
    ASSERT_EQ(subscription, nullptr);
    ASSERT_EQ(CheckErrorState(), true);
  }


  // Relese one
  {
    subscription = subscriptions.back();
    subscriptions.pop_back();
    ret = rmw_destroy_subscription(this->node, subscription);
    ASSERT_EQ(ret, RMW_RET_OK);
  }


  // Get one
  {
    dummy_type_supports.push_back(dummy_type_support_t());
    ConfigureDummyTypeSupport(
      topic_type,
      topic_type,
      message_namespace,
      id_gen++,
      &dummy_type_supports.back());
    subscription = rmw_create_subscription(
      this->node,
      &dummy_type_supports.back().type_support,
      dummy_type_supports.back().topic_name.data(),
      &dummy_qos_policies,
      &default_subscription_options);
    ASSERT_NE(subscription, nullptr);
    subscriptions.push_back(subscription);
  }


  // Release all
  {
    for (size_t i = 0; i < subscriptions.size(); i++) {
      subscription = subscriptions.at(i);
      ret = rmw_destroy_subscription(this->node, subscription);
      ASSERT_EQ(ret, RMW_RET_OK);
    }
    subscriptions.clear();
  }
}


/*
 * Testing node memory poll for same topic
 */
TEST_F(TestSubscription, memory_poll_shared_topic)
{
  dummy_type_support_t dummy_type_support;

  ConfigureDummyTypeSupport(
    topic_type,
    topic_type,
    message_namespace,
    id_gen++,
    &dummy_type_support);

  rmw_qos_profile_t dummy_qos_policies;
  ConfigureDefaultQOSPolices(&dummy_qos_policies);

  rmw_subscription_options_t default_subscription_options = rmw_get_default_subscription_options();

  std::vector<rmw_subscription_t *> subscriptions;
  rmw_ret_t ret;
  rmw_subscription_t * subscription;


  // Get all available nodes
  {
    for (size_t i = 0; i < RMW_UXRCE_MAX_SUBSCRIPTIONS; i++) {
      subscription = rmw_create_subscription(
        this->node,
        &dummy_type_support.type_support,
        dummy_type_support.topic_name.data(),
        &dummy_qos_policies,
        &default_subscription_options);
      ASSERT_NE(subscription, nullptr);
      subscriptions.push_back(subscription);
    }
  }


  // Try to get one
  {
    subscription = rmw_create_subscription(
      this->node,
      &dummy_type_support.type_support,
      dummy_type_support.topic_name.data(),
      &dummy_qos_policies,
      &default_subscription_options);
    ASSERT_EQ(subscription, nullptr);
    ASSERT_EQ(CheckErrorState(), true);
  }


  // Relese one
  {
    subscription = subscriptions.back();
    subscriptions.pop_back();
    ret = rmw_destroy_subscription(this->node, subscription);
    ASSERT_EQ(ret, RMW_RET_OK);
  }


  // Get one
  {
    subscription = rmw_create_subscription(
      this->node,
      &dummy_type_support.type_support,
      dummy_type_support.topic_name.data(),
      &dummy_qos_policies,
      &default_subscription_options);
    ASSERT_NE(subscription, nullptr);
    subscriptions.push_back(subscription);
  }


  // Release all
  {
    for (size_t i = 0; i < subscriptions.size(); i++) {
      subscription = subscriptions.at(i);
      ret = rmw_destroy_subscription(this->node, subscription);
      ASSERT_EQ(ret, RMW_RET_OK);
    }

    // Destroy an already detroyed subscriber
    ret = rmw_destroy_subscription(this->node, subscriptions.at(0));
    ASSERT_EQ(ret, RMW_RET_ERROR);

    subscriptions.clear();
  }
}
