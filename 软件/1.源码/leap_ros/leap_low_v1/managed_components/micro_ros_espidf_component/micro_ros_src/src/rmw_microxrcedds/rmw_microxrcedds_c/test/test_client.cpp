// Copyright 2018-2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <rosidl_typesupport_microxrcedds_c/message_type_support.h>
#include <rmw/rmw.h>
#include <rmw/validate_namespace.h>
#include <rmw/validate_node_name.h>
#include <rmw_microxrcedds_c/config.h>

#include <vector>
#include <memory>
#include <string>

#include "./rmw_base_test.hpp"
#include "./test_utils.hpp"

class TestClient : public RMWBaseTest
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

  const char * service_type = "service_type";
  const char * service_name = "service_name";

  size_t id_gen = 0;
};

/*
 * Testing client construction and destruction.
 */
TEST_F(TestClient, construction_and_destruction)
{
  dummy_service_type_support_t dummy_type_support;

  ConfigureDummyServiceTypeSupport(
    service_type,
    service_name,
    "",
    id_gen++,
    &dummy_type_support);

  rmw_qos_profile_t dummy_qos_policies;
  ConfigureDefaultQOSPolices(&dummy_qos_policies);

  rmw_client_t * client = rmw_create_client(
    this->node,
    &dummy_type_support.type_support,
    service_name,
    &dummy_qos_policies);

  ASSERT_NE((void *)client, (void *)NULL);

  rmw_ret_t ret = rmw_destroy_client(node, client);
  ASSERT_EQ(ret, RMW_RET_OK);
}


/*
 * Testing node memory poll for clients
 */
TEST_F(TestClient, memory_poll_multiple_clients)
{
  rmw_qos_profile_t dummy_qos_policies;

  ConfigureDefaultQOSPolices(&dummy_qos_policies);

  std::vector<dummy_service_type_support_t> dummy_type_supports;
  std::vector<rmw_client_t *> clients;
  rmw_ret_t ret;
  rmw_client_t * client;

  // Get all available nodes
  {
    for (size_t i = 0; i < RMW_UXRCE_MAX_CLIENTS; i++) {
      dummy_type_supports.push_back(dummy_service_type_support_t());
      ConfigureDummyServiceTypeSupport(
        service_type,
        service_name,
        "",
        id_gen++,
        &dummy_type_supports.back());

      client = rmw_create_client(
        this->node,
        &dummy_type_supports.back().type_support,
        service_name,
        &dummy_qos_policies);

      ASSERT_NE((void *)client, (void *)NULL);
      clients.push_back(client);
    }
  }


  // Try to get one
  {
    dummy_type_supports.push_back(dummy_service_type_support_t());
    ConfigureDummyServiceTypeSupport(
      service_type,
      service_name,
      "",
      id_gen++,
      &dummy_type_supports.back());

    client = rmw_create_client(
      this->node,
      &dummy_type_supports.back().type_support,
      service_name,
      &dummy_qos_policies);

    ASSERT_EQ((void *)client, (void *)NULL);
    ASSERT_EQ(CheckErrorState(), true);

    // Release one
    client = clients.back();
    clients.pop_back();
    ret = rmw_destroy_client(this->node, client);
    ASSERT_EQ(ret, RMW_RET_OK);
  }

  // Get one
  {
    dummy_type_supports.push_back(dummy_service_type_support_t());
    ConfigureDummyServiceTypeSupport(
      service_type,
      service_name,
      "",
      id_gen++,
      &dummy_type_supports.back());

    client = rmw_create_client(
      this->node,
      &dummy_type_supports.back().type_support,
      service_name,
      &dummy_qos_policies);
    ASSERT_NE((void *)client, (void *)NULL);
    clients.push_back(client);
  }


  // Release all
  {
    for (size_t i = 0; i < clients.size(); i++) {
      client = clients.at(i);
      ret = rmw_destroy_client(this->node, client);
      ASSERT_EQ(ret, RMW_RET_OK);
    }
    clients.clear();
  }
}
