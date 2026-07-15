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

#include <rosidl_runtime_c/string.h>
#include <uxr/client/core/session/session_info.h>

#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <map>

#include "./rmw_base_test.hpp"
#include "./test_utils.hpp"

#define MICROXRCEDDS_PADDING    sizeof(uint32_t)


static dummy_service_type_support_t dummy_type_support;

class TestReqRes : public ::testing::Test
{
public:
  void SetUp() override
  {
    ASSERT_EQ(rmw_init_options_init(&options_srv, rcutils_get_default_allocator()), RMW_RET_OK);
    ASSERT_EQ(rmw_init(&options_srv, &context_srv), RMW_RET_OK);

    ASSERT_EQ(rmw_init_options_init(&options_cli, rcutils_get_default_allocator()), RMW_RET_OK);
    ASSERT_EQ(rmw_init(&options_cli, &context_cli), RMW_RET_OK);

    configure_typesupport();

    node_srv = rmw_create_node(&context_srv, "node_srv", "/ns");
    node_cli = rmw_create_node(&context_cli, "node_cli", "/ns");


    EXPECT_NE(node_srv, nullptr);
    EXPECT_NE(node_cli, nullptr);
  }

  void TearDown() override
  {
    for (auto srv : services) {
      EXPECT_EQ(rmw_destroy_service(node_srv, srv), RMW_RET_OK);
    }

    for (auto cli : clients) {
      EXPECT_EQ(rmw_destroy_client(node_cli, cli), RMW_RET_OK);
    }

    EXPECT_EQ(rmw_destroy_node(node_srv), RMW_RET_OK);
    EXPECT_EQ(rmw_destroy_node(node_cli), RMW_RET_OK);

    ASSERT_EQ(rmw_init_options_fini(&options_srv), RMW_RET_OK);
    ASSERT_EQ(rmw_init_options_fini(&options_cli), RMW_RET_OK);

    ASSERT_EQ(rmw_shutdown(&context_srv), RMW_RET_OK);
    ASSERT_EQ(rmw_shutdown(&context_cli), RMW_RET_OK);
  }

  void configure_typesupport()
  {
    std::string service_type_test = std::string(service_type) +
      ::testing::UnitTest::GetInstance()->current_test_info()->name();
    std::string service_name_test = std::string(service_name) +
      ::testing::UnitTest::GetInstance()->current_test_info()->name();

    ConfigureDummyServiceTypeSupport(
      service_type_test.c_str(),
      service_name_test.c_str(),
      "",
      id_gen++,
      &dummy_type_support);

    dummy_type_support.callbacks.response_members_ = []() -> const rosidl_message_type_support_t * {
        return &dummy_type_support.response_members.type_support;
      };
    dummy_type_support.callbacks.request_members_ = []() -> const rosidl_message_type_support_t * {
        return &dummy_type_support.request_members.type_support;
      };

    dummy_type_support.request_members.callbacks.cdr_serialize =
      [](const void * untyped_service_req_message, ucdrBuffer * cdr) -> bool
      {
        bool ret;
        const rosidl_runtime_c__String * service_req_message =
          reinterpret_cast<const rosidl_runtime_c__String *>(untyped_service_req_message);

        ret = ucdr_serialize_string(cdr, service_req_message->data);
        return ret;
      };

    dummy_type_support.request_members.callbacks.cdr_deserialize =
      [](ucdrBuffer * cdr, void * untyped_service_req_message) -> bool
      {
        bool ret;
        rosidl_runtime_c__String * service_req_message =
          reinterpret_cast<rosidl_runtime_c__String *>(untyped_service_req_message);

        ret =
          ucdr_deserialize_string(cdr, service_req_message->data, service_req_message->capacity);
        if (ret) {
          service_req_message->size = strlen(service_req_message->data);
        }
        return ret;
      };

    dummy_type_support.request_members.callbacks.get_serialized_size =
      [](const void * untyped_service_req_message) -> uint32_t
      {
        const rosidl_runtime_c__String * service_req_message =
          reinterpret_cast<const rosidl_runtime_c__String *>(untyped_service_req_message);

        return MICROXRCEDDS_PADDING +
               ucdr_alignment(0, MICROXRCEDDS_PADDING) + service_req_message->size + 8;
      };
    dummy_type_support.request_members.callbacks.max_serialized_size = []() -> size_t
      {
        return static_cast<size_t>(MICROXRCEDDS_PADDING +
               ucdr_alignment(0, MICROXRCEDDS_PADDING) + 1);
      };

    dummy_type_support.response_members.callbacks.cdr_serialize =
      dummy_type_support.request_members.callbacks.cdr_serialize;
    dummy_type_support.response_members.callbacks.cdr_deserialize =
      dummy_type_support.request_members.callbacks.cdr_deserialize;
    dummy_type_support.response_members.callbacks.get_serialized_size =
      dummy_type_support.request_members.callbacks.get_serialized_size;
    dummy_type_support.response_members.callbacks.max_serialized_size =
      dummy_type_support.request_members.callbacks.max_serialized_size;
  }

  rmw_service_t * create_service(rmw_qos_profile_t qos)
  {
    rmw_service_t * serv = rmw_create_service(
      node_srv, &dummy_type_support.type_support,
      "test_service", &qos);
    EXPECT_NE(serv, nullptr);
    services.push_back(serv);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return serv;
  }

  rmw_client_t * create_client(rmw_qos_profile_t qos)
  {
    rmw_client_t * client = rmw_create_client(
      node_cli, &dummy_type_support.type_support,
      "test_service", &qos);
    EXPECT_NE(client, nullptr);
    clients.push_back(client);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return client;
  }

  void send_request(const char * data, rmw_client_t * client, int64_t & sequence_id)
  {
    rosidl_runtime_c__String service_req_message;
    service_req_message.data = const_cast<char *>(data);
    service_req_message.capacity = strlen(data);
    service_req_message.size = service_req_message.capacity;

    ASSERT_EQ(rmw_send_request(client, &service_req_message, &sequence_id), RMW_RET_OK);
    ASSERT_NE(sequence_id, UXR_INVALID_REQUEST_ID);
  }

  void send_response(const char * data, rmw_service_t * serv, rmw_service_info_t & request_header)
  {
    rosidl_runtime_c__String service_req_message;
    service_req_message.data = const_cast<char *>(data);
    service_req_message.capacity = strlen(data);
    service_req_message.size = service_req_message.capacity;

    ASSERT_EQ(
      rmw_send_response(serv, &request_header.request_id, &service_req_message),
      RMW_RET_OK);
  }

  rmw_ret_t wait_for_request(rmw_service_t * serv)
  {
    rmw_services_t services = {};
    void * servs[1] = {serv->data};
    services.services = servs;
    services.service_count = 1;

    rmw_time_t wait_timeout = (rmw_time_t) {4LL, 0LL};

    return rmw_wait(NULL, NULL, &services, NULL, NULL, NULL, &wait_timeout);
  }

  rmw_ret_t wait_for_response(rmw_client_t * client)
  {
    rmw_clients_t clients = {};
    void * clis[1] = {client->data};
    clients.clients = clis;
    clients.client_count = 1;

    rmw_time_t wait_timeout = (rmw_time_t) {4LL, 0LL};

    return rmw_wait(NULL, NULL, NULL, &clients, NULL, NULL, &wait_timeout);
  }

  rmw_ret_t take_request(
    rmw_service_t * serv, char * buff, size_t buff_size,
    bool & taken, rmw_service_info_t & request_header)
  {
    rosidl_runtime_c__String read_ros_message;
    read_ros_message.data = buff;
    read_ros_message.capacity = buff_size;
    read_ros_message.size = 0;

    return rmw_take_request(serv, &request_header, &read_ros_message, &taken);
  }

  rmw_ret_t take_response(
    rmw_client_t * client, char * buff, size_t buff_size,
    bool & taken, rmw_service_info_t & request_header)
  {
    rosidl_runtime_c__String read_ros_message;
    read_ros_message.data = buff;
    read_ros_message.capacity = buff_size;
    read_ros_message.size = 0;

    return rmw_take_response(client, &request_header, &read_ros_message, &taken);
  }

protected:
  size_t id_gen = 0;

  const char * service_type = "topic_type";
  const char * service_name = "topic_name";
  const char * message_namespace = "package_name";

  rmw_context_t context_srv = rmw_get_zero_initialized_context();
  rmw_init_options_t options_srv = rmw_get_zero_initialized_init_options();
  rmw_node_t * node_srv;

  rmw_context_t context_cli = rmw_get_zero_initialized_context();
  rmw_init_options_t options_cli = rmw_get_zero_initialized_init_options();
  rmw_node_t * node_cli;

  std::vector<rmw_client_t *> clients;
  std::vector<rmw_service_t *> services;
};

TEST_F(TestReqRes, request_and_reply)
{
  rmw_service_t * service = create_service(rmw_qos_profile_services_default);
  rmw_client_t * client = create_client(rmw_qos_profile_services_default);

  // Request
  std::string req_data = "test_request";
  int64_t sequence_id = -1;
  send_request(req_data.c_str(), client, sequence_id);

  ASSERT_EQ(wait_for_request(service), RMW_RET_OK);

  bool taken = false;
  rmw_service_info_t request_header;
  char req_recv_data[100] = {0};
  ASSERT_EQ(
    take_request(
      service, req_recv_data, sizeof(req_recv_data), taken,
      request_header), RMW_RET_OK);

  ASSERT_TRUE(taken);
  ASSERT_EQ(strcmp(req_data.c_str(), req_recv_data), 0);

  // Response
  std::string res_data = "test_response";
  send_response(res_data.c_str(), service, request_header);

  ASSERT_EQ(wait_for_response(client), RMW_RET_OK);

  taken = false;
  rmw_service_info_t response_header;
  char res_recv_data[100] = {0};
  ASSERT_EQ(
    take_response(
      client, res_recv_data, sizeof(res_recv_data), taken,
      response_header), RMW_RET_OK);

  ASSERT_TRUE(taken);
  ASSERT_EQ(sequence_id, response_header.request_id.sequence_number);
  ASSERT_EQ(strcmp(res_data.c_str(), res_recv_data), 0);
}

TEST_F(TestReqRes, request_expired)
{
  rmw_qos_profile_t qos = rmw_qos_profile_services_default;
  qos.lifespan = (rmw_time_t) {1LL, 0LL};

  rmw_service_t * service = create_service(qos);
  rmw_client_t * client = create_client(qos);

  // Request
  std::string req_data = "test_request";
  int64_t sequence_id = -1;
  send_request(req_data.c_str(), client, sequence_id);

  ASSERT_EQ(wait_for_request(service), RMW_RET_OK);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  bool taken = false;
  rmw_service_info_t request_header;
  char req_recv_data[100] = {0};
  ASSERT_EQ(
    take_request(
      service, req_recv_data, sizeof(req_recv_data), taken,
      request_header), RMW_RET_ERROR);

  ASSERT_FALSE(taken);
  ASSERT_NE(strcmp(req_data.c_str(), req_recv_data), 0);
}

TEST_F(TestReqRes, reply_expired)
{
  rmw_qos_profile_t qos = rmw_qos_profile_services_default;
  qos.lifespan = (rmw_time_t) {1LL, 0LL};

  rmw_service_t * service = create_service(qos);
  rmw_client_t * client = create_client(qos);

  // Request
  std::string req_data = "test_request";
  int64_t sequence_id = -1;
  send_request(req_data.c_str(), client, sequence_id);

  ASSERT_EQ(wait_for_request(service), RMW_RET_OK);

  bool taken = false;
  rmw_service_info_t request_header;
  char req_recv_data[100] = {0};
  ASSERT_EQ(
    take_request(
      service, req_recv_data, sizeof(req_recv_data), taken,
      request_header), RMW_RET_OK);

  ASSERT_TRUE(taken);
  ASSERT_EQ(strcmp(req_data.c_str(), req_recv_data), 0);

  // Response
  std::string res_data = "test_response";
  send_response(res_data.c_str(), service, request_header);

  ASSERT_EQ(wait_for_response(client), RMW_RET_OK);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  taken = false;
  rmw_service_info_t response_header = {};
  char res_recv_data[100] = {0};
  ASSERT_EQ(
    take_response(
      client, res_recv_data, sizeof(res_recv_data), taken,
      response_header), RMW_RET_ERROR);

  ASSERT_FALSE(taken);
  ASSERT_NE(sequence_id, response_header.request_id.sequence_number);
  ASSERT_NE(strcmp(res_data.c_str(), res_recv_data), 0);
}

// NOTE: Services are keep_last history 1 by default in the Agent
// Those tests will require Client API modifications

// TEST_F(TestReqRes, request_order_keep_all)
// {
//   rmw_qos_profile_t qos_service = rmw_qos_profile_services_default;
//   qos_service.history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
//   qos_service.depth = 6;

//   rmw_qos_profile_t qos_client = rmw_qos_profile_services_default;
//   qos_client.history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
//   qos_client.depth = 4;

//   rmw_service_t * service = create_service(qos_service);
//   rmw_client_t * client = create_client(qos_client);

//   // Requests
//   std::map<std::string, int64_t> sequence_map;
//   for (size_t i = 0; i < 10; i++) {
//     std::string req_data = "test_request_" + std::to_string(i);
//     int64_t sequence_id = -1;
//     send_request(req_data.c_str(), client, sequence_id);
//     sequence_map[req_data] = sequence_id;
//   }

//   for (size_t i = 0; i < 10; i++) {
//     wait_for_request(service);
//     bool taken = false;
//     rmw_service_info_t request_header;
//     char req_recv_data[100] = {0};
//     ASSERT_EQ(
//       take_request(service, req_recv_data, sizeof(req_recv_data), taken, request_header),
//       (i < qos_service.depth) ? RMW_RET_OK : RMW_RET_ERROR);

//     EXPECT_EQ(taken, (i < qos_service.depth) ? true : false);
//     std::string expected_data = "test_request_" + std::to_string(i);
//     EXPECT_EQ(
//       0 == strcmp(
//         expected_data.c_str(), req_recv_data), (i < qos_service.depth) ? true : false);

//     if (taken) {
//       std::string res_data = "test_response_" + std::to_string(i);
//       send_response(res_data.c_str(), service, request_header);
//     }
//   }

//   // Response
//   for (size_t i = 0; i < 10; i++) {
//     wait_for_response(client);
//     bool taken = false;
//     rmw_service_info_t response_header = {};
//     char res_recv_data[100] = {0};
//     ASSERT_EQ(
//       take_response(client, res_recv_data, sizeof(res_recv_data), taken, response_header),
//       (i < qos_client.depth) ? RMW_RET_OK : RMW_RET_ERROR);

//     EXPECT_EQ(taken, (i < qos_client.depth) ? true : false);

//     std::string expected_data = "test_response_" + std::to_string(i);
//     EXPECT_EQ(
//       0 == strcmp(
//         expected_data.c_str(), res_recv_data), (i < qos_client.depth) ? true : false);

//     if (taken) {
//       std::string request_data = "test_request_" + std::to_string(i);
//       ASSERT_EQ(sequence_map[request_data], response_header.request_id.sequence_number);
//     }
//   }
// }


// TEST_F(TestReqRes, request_order_keep_last)
// {
//   rmw_qos_profile_t qos_service = rmw_qos_profile_services_default;
//   qos_service.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
//   qos_service.depth = 6;

//   rmw_qos_profile_t qos_client = rmw_qos_profile_services_default;
//   qos_client.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
//   qos_client.depth = 4;

//   rmw_service_t * service = create_service(qos_service);
//   rmw_client_t * client = create_client(qos_client);

//   // Requests
//   std::map<std::string, int64_t> sequence_map;
//   size_t sent_requests = 10;
//   for (size_t i = 0; i < sent_requests; i++) {
//     std::string req_data = "test_request_" + std::to_string(i);
//     int64_t sequence_id = -1;
//     send_request(req_data.c_str(), client, sequence_id);
//     sequence_map[req_data] = sequence_id;
//   }

//   for (size_t i = 0; i < qos_service.depth; i++) {
//     ASSERT_EQ(wait_for_request(service), RMW_RET_OK);
//   }

//   for (size_t i = 0; i < sent_requests; i++) {
//     size_t expected_no = (sent_requests - qos_service.depth) + i;
//     bool taken = false;
//     rmw_service_info_t request_header;
//     char req_recv_data[100] = {0};
//     ASSERT_EQ(
//       take_request(service, req_recv_data, sizeof(req_recv_data), taken, request_header),
//       (i < qos_service.depth) ? RMW_RET_OK : RMW_RET_ERROR);

//     EXPECT_EQ(taken, (i < qos_service.depth) ? true : false);
//     std::string expected_data = "test_request_" + std::to_string(expected_no);
//     EXPECT_EQ(
//       0 == strcmp(
//         expected_data.c_str(), req_recv_data), (i < qos_service.depth) ? true : false);

//     if (taken) {
//       std::string res_data = "test_response_" + std::to_string(expected_no);
//       send_response(res_data.c_str(), service, request_header);
//     }
//   }

//   // Response
//   ASSERT_EQ(wait_for_response(client), RMW_RET_OK);

//   for (size_t i = 0; i < sent_requests; i++) {
//     size_t expected_no = (sent_requests - qos_client.depth) + i;
//     bool taken = false;
//     rmw_service_info_t response_header = {};
//     char res_recv_data[100] = {0};
//     ASSERT_EQ(
//       take_response(client, res_recv_data, sizeof(res_recv_data), taken, response_header),
//       (i < qos_client.depth) ? RMW_RET_OK : RMW_RET_ERROR);

//     EXPECT_EQ(taken, (i < qos_client.depth) ? true : false);

//     std::string expected_data = "test_response_" + std::to_string(expected_no);
//     EXPECT_EQ(
//       0 == strcmp(
//         expected_data.c_str(), res_recv_data), (i < qos_client.depth) ? true : false);

//     if (taken) {
//       std::string request_data = "test_request_" + std::to_string(expected_no);
//       ASSERT_EQ(sequence_map[request_data], response_header.request_id.sequence_number);
//     }
//   }
// }
