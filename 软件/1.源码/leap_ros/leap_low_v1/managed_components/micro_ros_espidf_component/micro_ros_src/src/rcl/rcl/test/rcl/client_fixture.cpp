// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#include "rcutils/logging_macros.h"

#include "rcl/client.h"
#include "rcl/rcl.h"

#include "test_msgs/srv/basic_types.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "wait_for_entity_helpers.hpp"

int main(int argc, char ** argv)
{
  int main_ret = 0;
  {
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
    if (RCL_RET_OK != ret) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in rcl init options init: %s", rcl_get_error_string().str);
      return -1;
    }
    rcl_context_t context = rcl_get_zero_initialized_context();
    if (rcl_init(argc, argv, &init_options, &context) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in rcl init: %s", rcl_get_error_string().str);
      return -1;
    }
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      if (rcl_shutdown(&context) != RCL_RET_OK) {
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME, "Error shutting down rcl: %s", rcl_get_error_string().str);
        main_ret = -1;
      }
      if (rcl_context_fini(&context) != RCL_RET_OK) {
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME, "Error finalizing rcl context: %s", rcl_get_error_string().str);
        main_ret = -1;
      }
    });
    ret = rcl_init_options_fini(&init_options);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in options fini: %s", rcl_get_error_string().str);
      return -1;
    }
    rcl_node_t node = rcl_get_zero_initialized_node();
    const char * name = "client_fixture_node";
    rcl_node_options_t node_options = rcl_node_get_default_options();
    if (rcl_node_init(&node, name, "", &context, &node_options) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in node init: %s", rcl_get_error_string().str);
      return -1;
    }
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      if (rcl_node_fini(&node) != RCL_RET_OK) {
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME, "Error in node fini: %s", rcl_get_error_string().str);
        main_ret = -1;
      }
    });

    const rosidl_service_type_support_t * ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
      test_msgs, srv, BasicTypes);
    const char * service_name = "basic_types";

    rcl_client_t client = rcl_get_zero_initialized_client();
    rcl_client_options_t client_options = rcl_client_get_default_options();
    ret = rcl_client_init(&client, &node, ts, service_name, &client_options);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in client init: %s", rcl_get_error_string().str);
      return -1;
    }

    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      if (rcl_client_fini(&client, &node)) {
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME, "Error in client fini: %s", rcl_get_error_string().str);
        main_ret = -1;
      }
    });

    // Wait until server is available
    if (!wait_for_server_to_be_available(&node, &client, 30, 100)) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Server never became available");
      return -1;
    }

    // Initialize a request.
    test_msgs__srv__BasicTypes_Request client_request;
    // TODO(dirk-thomas) zero initialization necessary until
    // https://github.com/ros2/ros2/issues/397 is implemented
    memset(&client_request, 0, sizeof(test_msgs__srv__BasicTypes_Request));
    test_msgs__srv__BasicTypes_Request__init(&client_request);
    client_request.uint8_value = 1;
    client_request.uint32_value = 2;
    int64_t sequence_number;

    if (rcl_send_request(&client, &client_request, &sequence_number)) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in send request: %s", rcl_get_error_string().str);
      return -1;
    }

    if (sequence_number != 1) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Got invalid sequence number");
      return -1;
    }

    test_msgs__srv__BasicTypes_Request__fini(&client_request);

    // Initialize the response owned by the client and take the response.
    test_msgs__srv__BasicTypes_Response client_response;
    // TODO(dirk-thomas) zero initialization necessary until
    // https://github.com/ros2/ros2/issues/397 is implemented
    memset(&client_response, 0, sizeof(test_msgs__srv__BasicTypes_Response));
    test_msgs__srv__BasicTypes_Response__init(&client_response);

    if (!wait_for_client_to_be_ready(&client, &context, 30, 100)) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Client never became ready");
      return -1;
    }
    rmw_service_info_t header;
    if (rcl_take_response_with_info(&client, &header, &client_response) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in send response: %s", rcl_get_error_string().str);
      return -1;
    }

    test_msgs__srv__BasicTypes_Response__fini(&client_response);
  }

  return main_ret;
}
