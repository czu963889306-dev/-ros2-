// Copyright (c) 2020 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
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
// limitations under the License.#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include "example_interfaces/srv/add_two_ints.h"

#include <stdio.h>

#define RCCHECK(fn) {rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)) {printf( \
        "Failed status on line %d: %d. Aborting.\n", __LINE__, (int)temp_rc); return 1;}}
#define RCSOFTCHECK(fn) {rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)) {printf( \
        "Failed status on line %d: %d. Continuing.\n", __LINE__, (int)temp_rc);}}

example_interfaces__srv__AddTwoInts_Request req;
example_interfaces__srv__AddTwoInts_Response res;

void client_callback(const void * msg)
{
  example_interfaces__srv__AddTwoInts_Response * msgin =
    (example_interfaces__srv__AddTwoInts_Response *) msg;
  printf(
    "Received service response %ld + %ld = %ld.\n", req.a, req.b, msgin->sum);
}

int main(int argc, const char * const * argv)
{
  RCLC_UNUSED(argc);
  RCLC_UNUSED(argv);
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  rcl_node_t node;
  RCCHECK(rclc_node_init_default(&node, "add_twoints_client_rclc", "", &support));

  // create client
  rcl_client_t client = rcl_get_zero_initialized_client();
  RCCHECK(
    rclc_client_init_default(
      &client, &node,
      ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts), "/addtwoints"));

  // create executor
  // Note:
  // If you need more than the default number of publisher/subscribers, etc., you
  // need to configure the micro-ROS middleware also!
  // See documentation in the executor.h at the function rclc_executor_init()
  // for more details.
  rclc_executor_t executor = rclc_executor_get_zero_initialized_executor();
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

  unsigned int rcl_wait_timeout = 10;         // in ms
  RCCHECK(rclc_executor_set_timeout(&executor, RCL_MS_TO_NS(rcl_wait_timeout)));
  RCCHECK(rclc_executor_add_client(&executor, &client, &res, client_callback));

  int64_t seq;
  example_interfaces__srv__AddTwoInts_Request__init(&req);
  req.a = 24;
  req.b = 42;

  rclc_sleep_ms(2000);   // Sleep a while to ensure DDS matching before sending request

  RCCHECK(rcl_send_request(&client, &req, &seq))
  printf("Send service request %ld + %ld.\n", req.a, req.b);

  // Start Executor
  rclc_executor_spin(&executor);

  RCCHECK(rcl_client_fini(&client, &node));
  RCCHECK(rcl_node_fini(&node));
}
