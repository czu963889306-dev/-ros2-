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

#include <rmw/rmw.h>
#include <rmw/validate_namespace.h>
#include <rmw/validate_node_name.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros_internal/rmw_microxrcedds_topic.h>

#include <vector>
#include <memory>
#include <string>

#include "./rmw_base_test.hpp"
#include "./test_utils.hpp"

class TestTopic : public RMWBaseTest
{
protected:
  void SetUp() override
  {
    RMWBaseTest::SetUp();

    node = rmw_create_node(&test_context, "my_node", "/ns");
    ASSERT_NE((void *)node, (void *)NULL);
  }

  void TearDown() override
  {
    ASSERT_EQ(rmw_destroy_node(node), RMW_RET_OK);
    RMWBaseTest::TearDown();
  }

  rmw_node_t * node;
  const size_t attempts = RMW_UXRCE_MAX_TOPICS_INTERNAL;
  size_t id_gen = 0;

  const char * topic_type = "topic_type";
  const char * topic_name = "topic_name";
  const char * package_name = "package_name";
};


/*
 * Testing topic construction and destruction.
 */
TEST_F(TestTopic, construction_and_destruction)
{
  dummy_type_support_t dummy_type_support;

  ConfigureDummyTypeSupport(
    topic_type,
    topic_type,
    package_name,
    id_gen++,
    &dummy_type_support);

  rmw_qos_profile_t dummy_qos_policies;
  ConfigureDefaultQOSPolices(&dummy_qos_policies);

  rmw_uxrce_topic_t * topic = create_topic(
    reinterpret_cast<struct rmw_uxrce_node_t *>(node->data),
    package_name,
    &dummy_type_support.callbacks,
    &dummy_qos_policies);
  ASSERT_NE((void *)topic, (void *)NULL);

  // TODO(pablogs9): Topic must be related to publisher in order to be counted
  // ASSERT_EQ(topic_count(reinterpret_cast<struct rmw_uxrce_node_t *>(node->data)), 1);

  rmw_ret_t ret = destroy_topic(topic);
  // TODO(pablogs9): Topic must be related to publisher in order to be counted
  // ASSERT_EQ(topic_count(reinterpret_cast<struct rmw_uxrce_node_t *>(node->data)), 0);
  ASSERT_EQ(ret, RMW_RET_OK);
}

/*
 * Testing creation multiple topics
 */
TEST_F(TestTopic, multiple_topic_creation)
{
  rmw_qos_profile_t dummy_qos_policies;

  ConfigureDefaultQOSPolices(&dummy_qos_policies);

  std::vector<rmw_uxrce_topic_t *> created_topics;
  std::vector<dummy_type_support_t> dummy_type_supports;
  for (size_t i = 0; i < attempts; i++) {
    dummy_type_supports.push_back(dummy_type_support_t());
    ConfigureDummyTypeSupport(
      topic_type,
      topic_type,
      package_name,
      id_gen++,
      &dummy_type_supports.back());

    rmw_uxrce_topic_t * created_topic = create_topic(
      reinterpret_cast<struct rmw_uxrce_node_t *>(node->data),
      dummy_type_supports.back().topic_name.data(),
      &dummy_type_supports.back().callbacks,
      &dummy_qos_policies);

    ASSERT_NE((void *)created_topic, (void *)NULL);
    // TODO(pablogs9): Topic must be related to publisher in order to be counted
    // ASSERT_EQ(topic_count(reinterpret_cast<struct rmw_uxrce_node_t *>(node->data)), i + 1);

    created_topics.push_back(created_topic);
  }

  for (size_t i = 0; i < created_topics.size(); i++) {
    // TODO(pablogs9): Topic must be related to publisher in order to be counted
    // ASSERT_EQ(topic_count(reinterpret_cast<struct rmw_uxrce_node_t *>(node->data)), attempts - i); NOLINT
    rmw_ret_t ret = destroy_topic(created_topics.at(i));
    ASSERT_EQ(ret, RMW_RET_OK);
  }

  // Destroy an already destroyed topic
  rmw_ret_t ret = destroy_topic(created_topics.at(0));
  ASSERT_EQ(ret, RMW_RET_ERROR);

  // TODO(pablogs9): Topic must be related to publisher in order to be counted
  // ASSERT_EQ(topic_count(reinterpret_cast<struct rmw_uxrce_node_t *>(node->data)), 0);
}
