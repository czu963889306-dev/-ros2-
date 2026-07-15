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

#include <vector>
#include <memory>

#include "./rmw_base_test.hpp"
#include "./test_utils.hpp"

class TestNode : public RMWBaseTest
{
};

/*
 * Testing node construction and destruction.
 */
TEST_F(TestNode, construction_and_destruction)
{
  // Success creation
  rmw_node_t * node = rmw_create_node(&test_context, "my_node", "/ns");

  ASSERT_NE(node, nullptr);
  rmw_ret_t ret = rmw_destroy_node(node);
  ASSERT_EQ(ret, RMW_RET_OK);
  ASSERT_EQ(CheckErrorState(), false);

  // Unsuccess creation
  node = rmw_create_node(&test_context, "", "/ns");
  ASSERT_EQ(node, nullptr);
  ASSERT_EQ(CheckErrorState(), true);
  rcutils_reset_error();

  // Unsuccess creation
  node = rmw_create_node(&test_context, "my_node", "");
  ASSERT_EQ(node, nullptr);
  rcutils_reset_error();
}

/*
 * Testing node memory poll
 */
TEST_F(TestNode, memory_poll)
{
  std::vector<rmw_node_t *> nodes;
  rmw_ret_t ret;
  rmw_node_t * node;

  // Get all available nodes
  for (size_t i = 0; i < RMW_UXRCE_MAX_NODES; i++) {
    node = rmw_create_node(&test_context, "my_node", "/ns");
    ASSERT_NE(node, nullptr);
    nodes.push_back(node);
  }

  // Try to get one
  node = rmw_create_node(&test_context, "my_node", "/ns");
  ASSERT_EQ(node, nullptr);
  ASSERT_EQ(CheckErrorState(), true);
  rcutils_reset_error();

  // Relese one
  ret = rmw_destroy_node(nodes.back());
  ASSERT_EQ(ret, RMW_RET_OK);
  nodes.pop_back();

  // Get one
  node = rmw_create_node(&test_context, "my_node", "/ns");
  ASSERT_NE(node, nullptr);
  nodes.push_back(node);

  // Release all
  for (size_t i = 0; i < nodes.size(); i++) {
    ret = rmw_destroy_node(nodes.at(i));
    ASSERT_EQ(ret, RMW_RET_OK);
  }
  nodes.clear();

  // Get all available nodes
  for (size_t i = 0; i < RMW_UXRCE_MAX_NODES; i++) {
    node = rmw_create_node(&test_context, "my_node", "/ns");
    ASSERT_NE(node, nullptr);
    nodes.push_back(node);
  }

  // Release all
  for (size_t i = 0; i < nodes.size(); i++) {
    ret = rmw_destroy_node(nodes.at(i));
    ASSERT_EQ(ret, RMW_RET_OK);
  }

  // Destroy an already detroyed node
  ret = rmw_destroy_node(nodes.at(0));
  ASSERT_EQ(ret, RMW_RET_ERROR);

  nodes.clear();
}
