// Copyright (c) 2020 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
// Copyright 2014 Open Source Robotics Foundation, Inc.
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

#include <stdio.h>

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>

#include <lifecycle_msgs/msg/transition_description.h>
#include <lifecycle_msgs/msg/transition_event.h>
#include <lifecycle_msgs/srv/change_state.h>
#include <lifecycle_msgs/srv/get_state.h>
#include <lifecycle_msgs/srv/get_available_states.h>
#include <lifecycle_msgs/srv/get_available_transitions.h>

#include <rclc/executor.h>

#include "rclc_lifecycle/rclc_lifecycle.h"

#define RCCHECK(fn) {rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)) {printf( \
        "Failed status on line %d: %d. Aborting.\n", __LINE__, (int)temp_rc); return 1;}}
#define RCSOFTCHECK(fn) {rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)) {printf( \
        "Failed status on line %d: %d. Continuing.\n", __LINE__, (int)temp_rc);}}

rcl_ret_t my_on_configure()
{
  printf("  >>> lifecycle_node: on_configure() callback called.\n");
  return RCL_RET_OK;
}

rcl_ret_t my_on_activate()
{
  printf("  >>> lifecycle_node: on_activate() callback called.\n");
  return RCL_RET_OK;
}

rcl_ret_t my_on_deactivate()
{
  printf("  >>> lifecycle_node: on_deactivate() callback called.\n");
  return RCL_RET_OK;
}

rcl_ret_t my_on_cleanup()
{
  printf("  >>> lifecycle_node: on_cleanup() callback called.\n");
  return RCL_RET_OK;
}

int main(int argc, const char * argv[])
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;
  rcl_ret_t rc;

  // create init_options
  rc = rclc_support_init(&support, argc, argv, &allocator);
  if (rc != RCL_RET_OK) {
    printf("Error rclc_support_init.\n");
    return -1;
  }

  // create rcl_node
  rcl_node_t my_node;
  rc = rclc_node_init_default(&my_node, "lifecycle_node", "rclc", &support);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_node_init_default\n");
    return -1;
  }

  // make it a lifecycle node
  printf("creating lifecycle node...\n");
  rcl_lifecycle_state_machine_t state_machine_ = rcl_lifecycle_get_zero_initialized_state_machine();
  rclc_lifecycle_node_t lifecycle_node;
  rc = rclc_make_node_a_lifecycle_node(
    &lifecycle_node,
    &my_node,
    &state_machine_,
    &allocator,
    true);
  if (rc != RCL_RET_OK) {
    printf("Error in creating lifecycle node.\n");
    return -1;
  }

  // Executor
  // Note:
  // If you need more than the default number of publisher/subscribers, etc., you
  // need to configure the micro-ROS middleware also!
  // See documentation in the executor.h at the function rclc_executor_init()
  // for more details.
  rclc_executor_t executor;
  RCCHECK(rclc_executor_init(
    &executor,
    &support.context,
    4,  // 1 for the node + 1 for each lifecycle service
    &allocator));

  unsigned int rcl_wait_timeout = 1000;  // in ms
  RCCHECK(rclc_executor_set_timeout(&executor, RCL_MS_TO_NS(rcl_wait_timeout)));

  // Register lifecycle services
  printf("registering lifecycle services...\n");
  rclc_lifecycle_service_context_t context;
  context.lifecycle_node = &lifecycle_node;
  RCCHECK(rclc_lifecycle_init_get_state_server(&context, &executor));
  RCCHECK(rclc_lifecycle_init_get_available_states_server(&context, &executor));
  RCCHECK(rclc_lifecycle_init_change_state_server(&context, &executor));

  // Register lifecycle service callbacks
  printf("registering callbacks...\n");
  rclc_lifecycle_register_on_configure(&lifecycle_node, &my_on_configure);
  rclc_lifecycle_register_on_activate(&lifecycle_node, &my_on_activate);
  rclc_lifecycle_register_on_deactivate(&lifecycle_node, &my_on_deactivate);
  rclc_lifecycle_register_on_cleanup(&lifecycle_node, &my_on_cleanup);

  // Run
  RCSOFTCHECK(rclc_executor_spin(&executor));

  // Cleanup
  printf("cleaning up...\n");
  rc = rclc_lifecycle_node_fini(&lifecycle_node, &allocator);
  rc += rcl_node_fini(&my_node);
  rc += rclc_executor_fini(&executor);
  rc += rclc_support_fini(&support);

  if (rc != RCL_RET_OK) {
    printf("Error while cleaning up!\n");
    return -1;
  }
  return 0;
}
