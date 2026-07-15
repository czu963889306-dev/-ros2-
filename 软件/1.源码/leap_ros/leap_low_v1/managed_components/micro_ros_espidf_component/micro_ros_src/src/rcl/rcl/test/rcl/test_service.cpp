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

#include "rcl/service.h"
#include "rcl/rcl.h"

#include "test_msgs/srv/basic_types.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rmw/validate_namespace.h"

#include "wait_for_entity_helpers.hpp"
#include "./allocator_testing_utils.h"
#include "../mocking_utils/patch.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestServiceFixture, RMW_IMPLEMENTATION) : public ::testing::Test
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
    const char * name = "test_service_node";
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

/* Basic nominal test of a service.
 */
TEST_F(CLASSNAME(TestServiceFixture, RMW_IMPLEMENTATION), test_service_nominal) {
  rcl_ret_t ret;
  const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    test_msgs, srv, BasicTypes);
  const char * topic = "primitives";
  const char * expected_topic = "/primitives";

  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_service_options_t service_options = rcl_service_get_default_options();
  ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
  EXPECT_EQ(RCL_RET_ALREADY_INIT, ret) << rcl_get_error_string().str;

  const rmw_qos_profile_t * request_subscription_qos =
    rcl_service_request_subscription_get_actual_qos(&service);
  EXPECT_EQ(rmw_qos_profile_services_default.reliability, request_subscription_qos->reliability);
  EXPECT_EQ(rmw_qos_profile_services_default.history, request_subscription_qos->history);
  EXPECT_EQ(rmw_qos_profile_services_default.depth, request_subscription_qos->depth);
  EXPECT_EQ(rmw_qos_profile_services_default.durability, request_subscription_qos->durability);

  const rmw_qos_profile_t * response_publisher_qos =
    rcl_service_response_publisher_get_actual_qos(&service);
  EXPECT_EQ(rmw_qos_profile_services_default.reliability, response_publisher_qos->reliability);
  EXPECT_EQ(rmw_qos_profile_services_default.history, response_publisher_qos->history);
  EXPECT_EQ(rmw_qos_profile_services_default.depth, response_publisher_qos->depth);
  EXPECT_EQ(rmw_qos_profile_services_default.durability, response_publisher_qos->durability);

  ret = rcl_service_fini(&service, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Check if null service is valid
  EXPECT_FALSE(rcl_service_is_valid(nullptr));
  rcl_reset_error();

  // Check if zero initialized client is valid
  service = rcl_get_zero_initialized_service();
  EXPECT_FALSE(rcl_service_is_valid(&service));
  rcl_reset_error();

  // Check that a valid service is valid
  service = rcl_get_zero_initialized_service();
  ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_service_is_valid(&service));
  rcl_reset_error();

  // Check that the service name matches what we assigned.
  EXPECT_EQ(strcmp(rcl_service_get_service_name(&service), expected_topic), 0);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_service_fini(&service, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  rcl_client_t client = rcl_get_zero_initialized_client();
  rcl_client_options_t client_options = rcl_client_get_default_options();
  ret = rcl_client_init(&client, this->node_ptr, ts, topic, &client_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_client_fini(&client, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  ASSERT_TRUE(wait_for_server_to_be_available(this->node_ptr, &client, 10, 1000));

  // Initialize a request.
  test_msgs__srv__BasicTypes_Request client_request;
  test_msgs__srv__BasicTypes_Request__init(&client_request);
  // TODO(clalancette): the C __init methods do not initialize all structure
  // members, so the numbers in the fields not explicitly set is arbitrary.
  // The CDR deserialization in Fast-CDR requires a 0 or 1 for bool fields,
  // so we explicitly initialize that even though we don't use it.  This can be
  // removed once the C __init methods initialize all members by default.
  client_request.bool_value = false;
  client_request.uint8_value = 1;
  client_request.uint32_value = 2;
  int64_t sequence_number;
  rcutils_time_point_value_t start_timestamp;
  // take timestamp before sending request
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_system_time_now(&start_timestamp));
  ret = rcl_send_request(&client, &client_request, &sequence_number);
  EXPECT_EQ(sequence_number, 1);
  test_msgs__srv__BasicTypes_Request__fini(&client_request);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ASSERT_TRUE(wait_for_service_to_be_ready(&service, context_ptr, 10, 100));

  // This scope simulates the service responding in a different context so that we can
  // test take_request/send_response in a single-threaded, deterministic execution.
  {
    // Initialize a response.
    test_msgs__srv__BasicTypes_Response service_response;
    test_msgs__srv__BasicTypes_Response__init(&service_response);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__srv__BasicTypes_Response__fini(&service_response);
    });

    // Initialize a separate instance of the request and take the pending request.
    test_msgs__srv__BasicTypes_Request service_request;
    test_msgs__srv__BasicTypes_Request__init(&service_request);
    rmw_service_info_t header;
    ret = rcl_take_request_with_info(&service, &header, &service_request);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    EXPECT_EQ(1, service_request.uint8_value);
    EXPECT_EQ(2UL, service_request.uint32_value);
#ifdef RMW_TIMESTAMPS_SUPPORTED
    EXPECT_GE(header.source_timestamp, start_timestamp);
#ifdef RMW_RECEIVED_TIMESTAMP_SUPPORTED
    EXPECT_GE(header.received_timestamp, start_timestamp);
    EXPECT_GE(header.received_timestamp, header.source_timestamp);
#else
    EXPECT_EQ(0u, header.received_timestamp);
#endif
#else
    EXPECT_EQ(0u, header.source_timestamp);
    EXPECT_EQ(0u, header.received_timestamp);
#endif
    // Simulate a response callback by summing the request and send the response..
    service_response.uint64_value = service_request.uint8_value + service_request.uint32_value;
    // take new timestamp before sending response
    EXPECT_EQ(RCUTILS_RET_OK, rcutils_system_time_now(&start_timestamp));
    ret = rcl_send_response(&service, &header.request_id, &service_response);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    test_msgs__srv__BasicTypes_Request__fini(&service_request);
  }
  ASSERT_FALSE(wait_for_service_to_be_ready(&service, context_ptr, 10, 100));

  // Initialize the response owned by the client and take the response.
  test_msgs__srv__BasicTypes_Response client_response;
  test_msgs__srv__BasicTypes_Response__init(&client_response);

  rmw_service_info_t header;
  ret = rcl_take_response_with_info(&client, &header, &client_response);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(client_response.uint64_value, 3ULL);
  EXPECT_EQ(header.request_id.sequence_number, 1);
#ifdef RMW_TIMESTAMPS_SUPPORTED
  EXPECT_GE(header.source_timestamp, start_timestamp);
#ifdef RMW_RECEIVED_TIMESTAMP_SUPPORTED
  EXPECT_GE(header.received_timestamp, start_timestamp);
  EXPECT_GE(header.received_timestamp, header.source_timestamp);
#else
  EXPECT_EQ(0u, header.received_timestamp);
#endif
#else
  EXPECT_EQ(0u, header.source_timestamp);
  EXPECT_EQ(0u, header.received_timestamp);
#endif

  ret = rcl_take_response_with_info(&client, &header, &client_response);
  EXPECT_EQ(RCL_RET_CLIENT_TAKE_FAILED, ret) << rcl_get_error_string().str;

  test_msgs__srv__BasicTypes_Response__fini(&client_response);
}

/* Basic nominal test of a service with rcl_take_response
 */
TEST_F(CLASSNAME(TestServiceFixture, RMW_IMPLEMENTATION), test_service_without_info) {
  rcl_ret_t ret;
  const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    test_msgs, srv, BasicTypes);
  const char * topic = "primitives";
  const char * expected_topic = "/primitives";

  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_service_options_t service_options = rcl_service_get_default_options();
  ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_service_is_valid(&service));
  rcl_reset_error();

  // Check that the service name matches what we assigned.
  EXPECT_STREQ(rcl_service_get_service_name(&service), expected_topic);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_service_fini(&service, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  rcl_client_t client = rcl_get_zero_initialized_client();
  rcl_client_options_t client_options = rcl_client_get_default_options();
  ret = rcl_client_init(&client, this->node_ptr, ts, topic, &client_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_client_fini(&client, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  ASSERT_TRUE(wait_for_server_to_be_available(this->node_ptr, &client, 10, 1000));

  // Initialize a request.
  test_msgs__srv__BasicTypes_Request client_request;
  test_msgs__srv__BasicTypes_Request__init(&client_request);
  // TODO(clalancette): the C __init methods do not initialize all structure
  // members, so the numbers in the fields not explicitly set is arbitrary.
  // The CDR deserialization in Fast-CDR requires a 0 or 1 for bool fields,
  // so we explicitly initialize that even though we don't use it.  This can be
  // removed once the C __init methods initialize all members by default.
  client_request.bool_value = false;
  client_request.uint8_value = 1;
  client_request.uint32_value = 2;
  int64_t sequence_number = 0;
  ret = rcl_send_request(&client, &client_request, &sequence_number);
  EXPECT_NE(sequence_number, 0);
  test_msgs__srv__BasicTypes_Request__fini(&client_request);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ASSERT_TRUE(wait_for_service_to_be_ready(&service, context_ptr, 10, 100));

  // This scope simulates the service responding in a different context so that we can
  // test take_request/send_response in a single-threaded, deterministic execution.
  {
    // Initialize a response.
    test_msgs__srv__BasicTypes_Response service_response;
    test_msgs__srv__BasicTypes_Response__init(&service_response);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__srv__BasicTypes_Response__fini(&service_response);
    });

    // Initialize a separate instance of the request and take the pending request.
    test_msgs__srv__BasicTypes_Request service_request;
    test_msgs__srv__BasicTypes_Request__init(&service_request);
    rmw_service_info_t header;
    ret = rcl_take_request(&service, &(header.request_id), &service_request);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    EXPECT_EQ(1, service_request.uint8_value);
    EXPECT_EQ(2UL, service_request.uint32_value);
    // Simulate a response callback by summing the request and send the response..
    service_response.uint64_value = service_request.uint8_value + service_request.uint32_value;
    ret = rcl_send_response(&service, &header.request_id, &service_response);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    test_msgs__srv__BasicTypes_Request__fini(&service_request);
  }
  ASSERT_FALSE(wait_for_service_to_be_ready(&service, context_ptr, 10, 100));

  // Initialize the response owned by the client and take the response.
  test_msgs__srv__BasicTypes_Response client_response;
  test_msgs__srv__BasicTypes_Response__init(&client_response);

  rmw_service_info_t header;
  ret = rcl_take_response(&client, &(header.request_id), &client_response);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(client_response.uint64_value, 3ULL);
  EXPECT_NE(header.request_id.sequence_number, 0);

  ret = rcl_take_response(&client, &(header.request_id), &client_response);
  EXPECT_EQ(RCL_RET_CLIENT_TAKE_FAILED, ret) << rcl_get_error_string().str;

  test_msgs__srv__BasicTypes_Response__fini(&client_response);
}

/* Passing bad/invalid arguments to service functions
 */
TEST_F(CLASSNAME(TestServiceFixture, RMW_IMPLEMENTATION), test_bad_arguments) {
  const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    test_msgs, srv, BasicTypes);
  const char * topic = "primitives";

  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_service_options_t service_options = rcl_service_get_default_options();

  rcl_service_options_t service_options_bad_alloc = rcl_service_get_default_options();
  service_options_bad_alloc.allocator.allocate = nullptr;
  rcl_node_t invalid_node = rcl_get_zero_initialized_node();

  EXPECT_EQ(
    RCL_RET_NODE_INVALID, rcl_service_init(
      &service, nullptr, ts, topic, &service_options)) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_NODE_INVALID, rcl_service_init(
      &service, &invalid_node, ts, topic, &service_options)) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_service_init(
      nullptr, this->node_ptr, ts, topic, &service_options)) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_service_init(
      &service, this->node_ptr, nullptr, topic, &service_options)) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_service_init(
      &service, this->node_ptr, ts, nullptr, &service_options)) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_service_init(
      &service, this->node_ptr, ts, topic, nullptr)) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_service_init(
      &service, this->node_ptr, ts, topic,
      &service_options_bad_alloc)) << rcl_get_error_string().str;

  EXPECT_EQ(
    RCL_RET_NODE_INVALID, rcl_service_fini(&service, nullptr)) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_NODE_INVALID, rcl_service_fini(&service, &invalid_node)) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_SERVICE_INVALID, rcl_service_fini(
      nullptr, this->node_ptr)) << rcl_get_error_string().str;

  test_msgs__srv__BasicTypes_Request service_request;
  test_msgs__srv__BasicTypes_Response service_response;
  test_msgs__srv__BasicTypes_Request__init(&service_request);
  test_msgs__srv__BasicTypes_Response__init(&service_response);
  rmw_service_info_t header;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__srv__BasicTypes_Request__fini(&service_request);
    test_msgs__srv__BasicTypes_Response__fini(&service_response);
  });

  EXPECT_EQ(nullptr, rcl_service_get_service_name(nullptr));
  EXPECT_EQ(nullptr, rcl_service_get_options(nullptr));
  EXPECT_EQ(nullptr, rcl_service_get_rmw_handle(nullptr));
  EXPECT_EQ(
    RCL_RET_SERVICE_INVALID, rcl_take_request_with_info(nullptr, &header, &service_request));
  EXPECT_EQ(
    RCL_RET_SERVICE_INVALID, rcl_send_response(nullptr, &header.request_id, &service_response));
  EXPECT_EQ(
    RCL_RET_SERVICE_INVALID, rcl_take_request(nullptr, &(header.request_id), &service_request));
  EXPECT_EQ(nullptr, rcl_service_request_subscription_get_actual_qos(nullptr));
  EXPECT_EQ(nullptr, rcl_service_response_publisher_get_actual_qos(nullptr));

  EXPECT_EQ(nullptr, rcl_service_get_service_name(&service));
  EXPECT_EQ(nullptr, rcl_service_get_options(&service));
  EXPECT_EQ(nullptr, rcl_service_get_rmw_handle(&service));
  EXPECT_EQ(
    RCL_RET_SERVICE_INVALID, rcl_take_request_with_info(&service, &header, &service_request));
  EXPECT_EQ(
    RCL_RET_SERVICE_INVALID, rcl_send_response(&service, &(header.request_id), &service_response));
  EXPECT_EQ(
    RCL_RET_SERVICE_INVALID, rcl_take_request(&service, &(header.request_id), &service_request));

  service_options_bad_alloc.allocator = get_failing_allocator();
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC, rcl_service_init(
      &service, this->node_ptr, ts,
      topic, &service_options_bad_alloc)) << rcl_get_error_string().str;

  EXPECT_EQ(nullptr, rcl_service_request_subscription_get_actual_qos(&service));
  EXPECT_EQ(nullptr, rcl_service_response_publisher_get_actual_qos(&service));
}

/* Name failed tests
 */
TEST_F(CLASSNAME(TestServiceFixture, RMW_IMPLEMENTATION), test_service_fail_name) {
  const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    test_msgs, srv, BasicTypes);
  const char * topic = "white space";
  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_ret_t ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
  EXPECT_EQ(RCL_RET_SERVICE_NAME_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  const char * topic2 = "{invalidbecausecurlybraces}";
  ret = rcl_service_init(&service, this->node_ptr, ts, topic2, &service_options);
  EXPECT_EQ(RCL_RET_SERVICE_NAME_INVALID, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

// Define dummy comparison operators for rcutils_allocator_t type for use with the Mimick Library
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, ==)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, <)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, >)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, !=)

/* Test failed service initialization using mocks
 */
TEST_F(CLASSNAME(TestServiceFixture, RMW_IMPLEMENTATION), test_fail_ini_mocked) {
  const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    test_msgs, srv, BasicTypes);
  constexpr char topic[] = "topic";
  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_service_options_t service_options = rcl_service_get_default_options();
  service_options.qos.durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
  rcl_ret_t ret = RCL_RET_OK;

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rcutils_string_map_init, RCUTILS_RET_ERROR);
    ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    // Mocking this function causes rcl_expand_topic_name to return RCL_RET_ERROR
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_validate_namespace, RMW_RET_ERROR);
    ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    auto mock = mocking_utils::inject_on_return(
      "lib:rcl", rcutils_string_map_fini, RCUTILS_RET_ERROR);
    ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
  }
  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_validate_full_topic_name, RMW_RET_ERROR);
    ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    auto mock = mocking_utils::patch(
      "lib:rcl", rmw_validate_full_topic_name,
      [](auto, int * result, auto) {
        *result = RMW_TOPIC_INVALID_IS_EMPTY_STRING;
        return RMW_RET_OK;
      });
    ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
    EXPECT_EQ(RCL_RET_SERVICE_NAME_INVALID, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_create_service, nullptr);
    ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
}

/* Test failed service finalization using mocks
 */
TEST_F(CLASSNAME(TestServiceFixture, RMW_IMPLEMENTATION), test_fail_fini_mocked) {
  const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    test_msgs, srv, BasicTypes);
  constexpr char topic[] = "primitives";

  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_ret_t ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_service_t empty_service = rcl_get_zero_initialized_service();
  ret = rcl_service_fini(&empty_service, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  auto mock = mocking_utils::inject_on_return(
    "lib:rcl", rmw_destroy_service, RMW_RET_ERROR);
  ret = rcl_service_fini(&service, this->node_ptr);
  EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}

/* Test failed service take_request_with_info using mocks and nullptrs
 */
TEST_F(CLASSNAME(TestServiceFixture, RMW_IMPLEMENTATION), test_fail_take_request_with_info) {
  const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    test_msgs, srv, BasicTypes);
  constexpr char topic[] = "primitives";

  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_ret_t ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_service_fini(&service, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  test_msgs__srv__BasicTypes_Request service_request;
  test_msgs__srv__BasicTypes_Request__init(&service_request);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__srv__BasicTypes_Request__fini(&service_request);
  });
  rmw_service_info_t header;

  ret = rcl_take_request_with_info(nullptr, &header, &service_request);
  EXPECT_EQ(RCL_RET_SERVICE_INVALID, ret);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  ret = rcl_take_request_with_info(&service, nullptr, &service_request);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  ret = rcl_take_request_with_info(&service, &header, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_take_request, RMW_RET_ERROR);
    ret = rcl_take_request_with_info(&service, &header, &service_request);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_take_request, RMW_RET_BAD_ALLOC);
    ret = rcl_take_request_with_info(&service, &header, &service_request);
    EXPECT_EQ(RCL_RET_BAD_ALLOC, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    auto mock = mocking_utils::patch(
      "lib:rcl", rmw_take_request,
      [](auto, auto, auto, bool * taken) {
        *taken = false;
        return RMW_RET_OK;
      });
    ret = rcl_take_request_with_info(&service, &header, &service_request);
    EXPECT_EQ(RCL_RET_SERVICE_TAKE_FAILED, ret);
  }
}

/* Test failed service send_response using mocks and nullptrs
 */
TEST_F(CLASSNAME(TestServiceFixture, RMW_IMPLEMENTATION), test_fail_send_response) {
  const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    test_msgs, srv, BasicTypes);
  constexpr char topic[] = "primitives";

  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_ret_t ret = rcl_service_init(&service, this->node_ptr, ts, topic, &service_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_service_fini(&service, this->node_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  // Init dummy response.
  test_msgs__srv__BasicTypes_Response service_response;
  test_msgs__srv__BasicTypes_Response__init(&service_response);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__srv__BasicTypes_Response__fini(&service_response);
  });
  rmw_service_info_t header;

  ret = rcl_send_response(nullptr, &header.request_id, &service_response);
  EXPECT_EQ(RCL_RET_SERVICE_INVALID, ret);

  ret = rcl_send_response(&service, nullptr, &service_response);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);

  ret = rcl_send_response(&service, &header.request_id, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_send_response, RMW_RET_ERROR);
    ret = rcl_send_response(&service, &header.request_id, &service_response);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
}
