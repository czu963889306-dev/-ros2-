// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcutils/allocator.h"
#include "rcutils/strdup.h"

#include "rmw/rmw.h"
#include "rmw/error_handling.h"

#include "test_msgs/srv/basic_types.h"

#include "./config.hpp"
#include "./testing_macros.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestService, RMW_IMPLEMENTATION) : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rmw_init_options_t init_options = rmw_get_zero_initialized_init_options();
    rmw_ret_t ret = rmw_init_options_init(&init_options, rcutils_get_default_allocator());
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      rmw_ret_t ret = rmw_init_options_fini(&init_options);
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    });
    init_options.enclave = rcutils_strdup("/", rcutils_get_default_allocator());
    ASSERT_STREQ("/", init_options.enclave);
    ret = rmw_init(&init_options, &context);
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    constexpr char node_name[] = "my_test_node";
    constexpr char node_namespace[] = "/my_test_ns";
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
  }

  rmw_context_t context{rmw_get_zero_initialized_context()};
  rmw_node_t * node{nullptr};
};

TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), create_and_destroy) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  rmw_service_t * srv =
    rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
  ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_service(node, srv);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), create_and_destroy_native) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  rmw_qos_profile_t native_qos_profile = rmw_qos_profile_default;
  native_qos_profile.avoid_ros_namespace_conventions = true;
  rmw_service_t * srv =
    rmw_create_service(node, ts, service_name, &native_qos_profile);
  ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_service(node, srv);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), create_with_bad_arguments) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);

  rmw_service_t * srv =
    rmw_create_service(nullptr, ts, service_name, &rmw_qos_profile_default);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  srv = rmw_create_service(node, nullptr, service_name, &rmw_qos_profile_default);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  srv = rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
  node->implementation_identifier = implementation_identifier;
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  srv = rmw_create_service(node, ts, nullptr, &rmw_qos_profile_default);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  srv = rmw_create_service(node, ts, "", &rmw_qos_profile_default);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  constexpr char service_name_with_spaces[] = "/foo bar";
  srv = rmw_create_service(node, ts, service_name_with_spaces, &rmw_qos_profile_default);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  constexpr char relative_service_name[] = "foo";
  srv = rmw_create_service(node, ts, relative_service_name, &rmw_qos_profile_default);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  srv = rmw_create_service(node, ts, service_name, nullptr);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  srv = rmw_create_service(node, ts, service_name, &rmw_qos_profile_unknown);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();

  rosidl_service_type_support_t * non_const_ts =
    const_cast<rosidl_service_type_support_t *>(ts);
  const char * typesupport_identifier = non_const_ts->typesupport_identifier;
  non_const_ts->typesupport_identifier = "not-a-typesupport-identifier";
  srv = rmw_create_service(node, non_const_ts, service_name, &rmw_qos_profile_default);
  EXPECT_EQ(nullptr, srv);
  rmw_reset_error();
  non_const_ts->typesupport_identifier = typesupport_identifier;

  // Creating and destroying a service still succeeds.
  srv = rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
  ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;
  rmw_ret_t ret = rmw_destroy_service(node, srv);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}


TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), create_with_internal_errors) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rmw_service_t * srv = rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
    if (srv) {
      RCUTILS_NO_FAULT_INJECTION(
      {
        rmw_ret_t ret = rmw_destroy_service(node, srv);
        EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
      });
    } else {
      rmw_reset_error();
    }
  });
}

TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), destroy_with_internal_errors) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rmw_service_t * srv = nullptr;
    RCUTILS_NO_FAULT_INJECTION(
    {
      srv = rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
      ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;
    });
    if (RMW_RET_OK != rmw_destroy_service(node, srv)) {
      rmw_reset_error();
    }
  });
}

class CLASSNAME (TestServiceUse, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestService, RMW_IMPLEMENTATION)
{
protected:
  using Base = CLASSNAME(TestService, RMW_IMPLEMENTATION);

  void SetUp() override
  {
    Base::SetUp();
    constexpr char service_name[] = "/test";
    const rosidl_service_type_support_t * ts =
      ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
    srv = rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
    ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;
  }

  void TearDown() override
  {
    rmw_ret_t ret = rmw_destroy_service(node, srv);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    Base::TearDown();
  }

  rmw_service_t * srv{nullptr};
};

TEST_F(CLASSNAME(TestServiceUse, RMW_IMPLEMENTATION), destroy_with_null_node) {
  rmw_ret_t ret = rmw_destroy_service(nullptr, srv);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestServiceUse, RMW_IMPLEMENTATION), destroy_null_service) {
  rmw_ret_t ret = rmw_destroy_service(node, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestServiceUse, RMW_IMPLEMENTATION), destroy_with_node_of_another_impl) {
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  rmw_ret_t ret = rmw_destroy_service(node, srv);
  node->implementation_identifier = implementation_identifier;
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestServiceUse, RMW_IMPLEMENTATION), destroy_service_of_another_impl) {
  const char * implementation_identifier = srv->implementation_identifier;
  srv->implementation_identifier = "not-an-rmw-implementation-identifier";
  rmw_ret_t ret = rmw_destroy_service(node, srv);
  srv->implementation_identifier = implementation_identifier;
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), take_request_with_bad_arguments) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  test_msgs__srv__BasicTypes_Request service_request;
  ASSERT_TRUE(test_msgs__srv__BasicTypes_Request__init(&service_request));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__srv__BasicTypes_Request__fini(&service_request);
  });
  service_request.bool_value = false;
  service_request.uint8_value = 1;
  service_request.uint32_value = 2;
  bool taken = false;
  rmw_service_info_t header;
  rmw_service_t * srv =
    rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
  ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;

  rmw_ret_t ret = rmw_take_request(nullptr, &header, &service_request, &taken);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(false, service_request.bool_value);  // Verify post conditions
  EXPECT_EQ(1u, service_request.uint8_value);
  EXPECT_EQ(2u, service_request.uint32_value);
  EXPECT_EQ(false, taken);
  rmw_reset_error();

  ret = rmw_take_request(srv, nullptr, &service_request, &taken);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(false, service_request.bool_value);  // Verify post conditions
  EXPECT_EQ(1u, service_request.uint8_value);
  EXPECT_EQ(2u, service_request.uint32_value);
  EXPECT_EQ(false, taken);
  rmw_reset_error();

  ret = rmw_take_request(srv, &header, nullptr, &taken);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(false, taken);
  rmw_reset_error();

  ret = rmw_take_request(srv, &header, &service_request, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(false, service_request.bool_value);  // Verify post conditions
  EXPECT_EQ(1u, service_request.uint8_value);
  EXPECT_EQ(2u, service_request.uint32_value);
  rmw_reset_error();

  const char * implementation_identifier = srv->implementation_identifier;
  srv->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_take_request(srv, &header, &service_request, &taken);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  EXPECT_EQ(false, service_request.bool_value);  // Verify post conditions
  EXPECT_EQ(1u, service_request.uint8_value);
  EXPECT_EQ(2u, service_request.uint32_value);
  EXPECT_EQ(false, taken);
  rmw_reset_error();
  srv->implementation_identifier = implementation_identifier;

  ret = rmw_destroy_service(node, srv);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), send_reponse_with_bad_arguments) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  test_msgs__srv__BasicTypes_Response service_response;
  ASSERT_TRUE(test_msgs__srv__BasicTypes_Response__init(&service_response));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__srv__BasicTypes_Response__fini(&service_response);
  });
  service_response.bool_value = false;
  service_response.uint8_value = 1;
  service_response.uint32_value = 2;
  test_msgs__srv__BasicTypes_Request request;
  ASSERT_TRUE(test_msgs__srv__BasicTypes_Request__init(&request));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__srv__BasicTypes_Request__fini(&request);
  });
  request.bool_value = false;
  request.uint8_value = 1;
  request.uint32_value = 2;
  int64_t sequence_number;
  rmw_service_info_t header;
  rmw_service_t * srv =
    rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
  ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rmw_ret_t ret = rmw_destroy_service(node, srv);
    EXPECT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
  });
  rmw_client_t * client =
    rmw_create_client(node, ts, service_name, &rmw_qos_profile_default);
  ASSERT_NE(nullptr, client) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rmw_ret_t ret = rmw_destroy_client(node, client);
    EXPECT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
  });

  bool is_available = false;
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    rmw_ret_t ret = rmw_service_server_is_available(node, client, &is_available);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    rmw_reset_error();
    if (is_available) {
      break;
    }
  }
  ASSERT_TRUE(is_available);

  rmw_ret_t ret = rmw_send_request(client, &request, &sequence_number);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  size_t number_of_services = 1u;
  rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, number_of_services);
  ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rmw_ret_t ret = rmw_destroy_wait_set(wait_set);
    EXPECT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
  });
  void * array[1];
  array[0] = srv->data;
  rmw_services_t srv_array;
  srv_array.service_count = 1u;
  srv_array.services = array;
  rmw_time_t timeout;
  auto rmw_intraprocess_discovery_delay_in_nanoseconds =
    std::chrono::duration_cast<std::chrono::nanoseconds>(
    rmw_intraprocess_discovery_delay * 10).count();
  timeout.sec = rmw_intraprocess_discovery_delay_in_nanoseconds / 1000000000;
  timeout.nsec = rmw_intraprocess_discovery_delay_in_nanoseconds % 1000000000;
  ret = rmw_wait(nullptr, nullptr, &srv_array, nullptr, nullptr, wait_set, &timeout);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ASSERT_NE(nullptr, srv_array.services[0]);

  bool taken = false;
  ret = rmw_take_request(srv, &header, &request, &taken);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ASSERT_EQ(true, taken);

  ret = rmw_send_response(nullptr, &header.request_id, &service_response);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  ret = rmw_send_response(srv, nullptr, &service_response);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  ret = rmw_send_response(srv, &header.request_id, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  const char * implementation_identifier = srv->implementation_identifier;
  srv->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_send_response(srv, &header.request_id, &service_response);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret) << rmw_get_error_string().str;
  rmw_reset_error();
  srv->implementation_identifier = implementation_identifier;
}

TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), send_reponse_with_client_gone) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
  test_msgs__srv__BasicTypes_Response service_response;
  ASSERT_TRUE(test_msgs__srv__BasicTypes_Response__init(&service_response));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__srv__BasicTypes_Response__fini(&service_response);
  });
  service_response.bool_value = false;
  service_response.uint8_value = 1;
  service_response.uint32_value = 2;
  test_msgs__srv__BasicTypes_Request request;
  ASSERT_TRUE(test_msgs__srv__BasicTypes_Request__init(&request));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__srv__BasicTypes_Request__fini(&request);
  });
  request.bool_value = false;
  request.uint8_value = 1;
  request.uint32_value = 2;
  int64_t sequence_number;
  rmw_service_info_t header;
  rmw_service_t * srv =
    rmw_create_service(node, ts, service_name, &rmw_qos_profile_default);
  ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rmw_ret_t ret = rmw_destroy_service(node, srv);
    EXPECT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
  });
  bool destroy_client = true;
  rmw_client_t * client =
    rmw_create_client(node, ts, service_name, &rmw_qos_profile_default);
  ASSERT_NE(nullptr, client) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    if (destroy_client) {
      rmw_ret_t ret = rmw_destroy_client(node, client);
      EXPECT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    }
  });

  bool is_available = false;
  SLEEP_AND_RETRY_UNTIL(rmw_intraprocess_discovery_delay, rmw_intraprocess_discovery_delay * 10) {
    rmw_ret_t ret = rmw_service_server_is_available(node, client, &is_available);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    rmw_reset_error();
    if (is_available) {
      break;
    }
  }
  ASSERT_TRUE(is_available);

  rmw_ret_t ret = rmw_send_request(client, &request, &sequence_number);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  size_t number_of_services = 1u;
  rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, number_of_services);
  ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rmw_ret_t ret = rmw_destroy_wait_set(wait_set);
    EXPECT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
  });
  void * array[1];
  array[0] = srv->data;
  rmw_services_t srv_array;
  srv_array.service_count = 1u;
  srv_array.services = array;
  rmw_time_t timeout;
  auto rmw_intraprocess_discovery_delay_in_nanoseconds =
    std::chrono::duration_cast<std::chrono::nanoseconds>(
    rmw_intraprocess_discovery_delay * 10).count();
  timeout.sec = rmw_intraprocess_discovery_delay_in_nanoseconds / 1000000000;
  timeout.nsec = rmw_intraprocess_discovery_delay_in_nanoseconds % 1000000000;
  ret = rmw_wait(nullptr, nullptr, &srv_array, nullptr, nullptr, wait_set, &timeout);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ASSERT_NE(nullptr, srv_array.services[0]);

  bool taken = false;
  ret = rmw_take_request(srv, &header, &request, &taken);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ASSERT_EQ(true, taken);

  // Remove client
  ret = rmw_destroy_client(node, client);
  EXPECT_EQ(RMW_RET_OK, ret);
  destroy_client = false;

  // RMW_RET_OK is returned even if the client is gone
  ret = rmw_send_response(srv, &header.request_id, &service_response);
  EXPECT_EQ(RMW_RET_OK, ret);
}

TEST_F(CLASSNAME(TestService, RMW_IMPLEMENTATION), check_qos) {
  constexpr char service_name[] = "/test";
  const rosidl_service_type_support_t * ts =
    ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);

  rmw_qos_profile_t qos_profile = rmw_qos_profile_services_default;
  qos_profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  uint64_t duration = 1;
  qos_profile.deadline = {duration, duration};
  qos_profile.lifespan = {duration, duration};
  qos_profile.liveliness_lease_duration = {duration, duration};

  rmw_service_t * srv =
    rmw_create_service(node, ts, service_name, &qos_profile);

  rmw_qos_profile_t actual_rp_qos;
  rmw_ret_t ret = rmw_service_response_publisher_get_actual_qos(
    srv,
    &actual_rp_qos);

  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(actual_rp_qos.reliability, qos_profile.reliability);
  EXPECT_EQ(actual_rp_qos.durability, qos_profile.durability);
  EXPECT_EQ(actual_rp_qos.liveliness, qos_profile.liveliness);
  EXPECT_EQ(actual_rp_qos.history, qos_profile.history);
  EXPECT_EQ(actual_rp_qos.depth, qos_profile.depth);
  EXPECT_EQ(actual_rp_qos.deadline.sec, qos_profile.deadline.sec);
  EXPECT_EQ(actual_rp_qos.lifespan.sec, qos_profile.lifespan.sec);
  EXPECT_EQ(actual_rp_qos.liveliness_lease_duration.sec, qos_profile.liveliness_lease_duration.sec);
  EXPECT_EQ(actual_rp_qos.deadline.nsec, qos_profile.deadline.nsec);
  EXPECT_EQ(actual_rp_qos.lifespan.nsec, qos_profile.lifespan.nsec);
  EXPECT_EQ(
    actual_rp_qos.liveliness_lease_duration.nsec, qos_profile.liveliness_lease_duration.nsec);

  rmw_qos_profile_t actual_rs_qos;
  ret = rmw_service_request_subscription_get_actual_qos(
    srv,
    &actual_rs_qos);

  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(actual_rs_qos.reliability, qos_profile.reliability);
  EXPECT_EQ(actual_rs_qos.durability, qos_profile.durability);
  EXPECT_EQ(actual_rs_qos.liveliness, qos_profile.liveliness);
  EXPECT_EQ(actual_rs_qos.history, qos_profile.history);
  EXPECT_EQ(actual_rs_qos.depth, qos_profile.depth);
  EXPECT_EQ(actual_rs_qos.deadline.sec, qos_profile.deadline.sec);
  EXPECT_EQ(actual_rs_qos.deadline.nsec, qos_profile.deadline.nsec);
  EXPECT_EQ(actual_rs_qos.liveliness_lease_duration.sec, qos_profile.liveliness_lease_duration.sec);
  EXPECT_EQ(
    actual_rs_qos.liveliness_lease_duration.nsec, qos_profile.liveliness_lease_duration.nsec);
}
