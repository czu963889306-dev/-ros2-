// Copyright (c) 2021 - for information on the respective copyright owner
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
// limitations under the License.

#include <stdio.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <rclc_parameter/rclc_parameter.h>

rclc_parameter_server_t param_server;

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  (void) timer;
  (void) last_call_time;

  int64_t value;
  rclc_parameter_get_int(&param_server, "param2", &value);
  value++;
  rclc_parameter_set_int(&param_server, "param2", value);
}

bool on_parameter_changed(const Parameter * old_param, const Parameter * new_param, void * context)
{
  (void) context;

  if (old_param == NULL && new_param == NULL) {
    printf("Callback error, both parameters are NULL\n");
    return false;
  }

  if (old_param == NULL) {
    printf("Creating new parameter %s\n", new_param->name.data);
  } else if (new_param == NULL) {
    printf("Deleting parameter %s\n", old_param->name.data);
  } else {
    printf("Parameter %s modified.", old_param->name.data);
    switch (old_param->value.type) {
      case RCLC_PARAMETER_BOOL:
        printf(
          " Old value: %d, New value: %d (bool)", old_param->value.bool_value,
          new_param->value.bool_value);
        break;
      case RCLC_PARAMETER_INT:
        printf(
          " Old value: %ld, New value: %ld (int)", old_param->value.integer_value,
          new_param->value.integer_value);
        break;
      case RCLC_PARAMETER_DOUBLE:
        printf(
          " Old value: %f, New value: %f (double)", old_param->value.double_value,
          new_param->value.double_value);
        break;
      default:
        break;
    }
    printf("\n");
  }

  return true;
}

int main()
{
  rcl_ret_t rc;
  rcl_allocator_t allocator = rcl_get_default_allocator();

  // Create init_options
  rclc_support_t support;
  rclc_support_init(&support, 0, NULL, &allocator);

  // Create node
  rcl_node_t node;
  rclc_node_init_default(&node, "demo_param_node", "", &support);

  // Create parameter service
  rclc_parameter_server_init_default(&param_server, &node);

  // create timer,
  rcl_timer_t timer;
  rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(1000),
    timer_callback);

  // Create executor
  // Note:
  // If you need more than the default number of publisher/subscribers, etc., you
  // need to configure the micro-ROS middleware also!
  // See documentation in the executor.h at the function rclc_executor_init()
  // for more details.
  rclc_executor_t executor;
  rclc_executor_init(
    &executor, &support.context, RCLC_EXECUTOR_PARAMETER_SERVER_HANDLES + 1,
    &allocator);
  rclc_executor_add_parameter_server(&executor, &param_server, on_parameter_changed);
  rclc_executor_add_timer(&executor, &timer);

  // Add parameters
  rclc_add_parameter(&param_server, "param1", RCLC_PARAMETER_BOOL);
  rclc_add_parameter(&param_server, "param2", RCLC_PARAMETER_INT);
  rclc_add_parameter(&param_server, "param3", RCLC_PARAMETER_DOUBLE);

  rclc_parameter_set_bool(&param_server, "param1", false);
  rclc_parameter_set_int(&param_server, "param2", 10);
  rclc_parameter_set_double(&param_server, "param3", 0.01);

  // Add parameters constraints
  rclc_add_parameter_description(&param_server, "param2", "Second parameter", "Only even numbers");
  rclc_add_parameter_constraint_integer(&param_server, "param2", -10, 120, 2);

  rclc_add_parameter_description(&param_server, "param3", "Third parameter", "");
  rclc_set_parameter_read_only(&param_server, "param3", true);

  bool param1;
  int64_t param2;
  double param3;

  rclc_parameter_get_bool(&param_server, "param1", &param1);
  rclc_parameter_get_int(&param_server, "param2", &param2);
  rclc_parameter_get_double(&param_server, "param3", &param3);

  // Start Executor
  rclc_executor_spin(&executor);

  // clean up
  rc = rclc_executor_fini(&executor);
  rc += rclc_parameter_server_fini(&param_server, &node);
  rc += rcl_node_fini(&node);

  if (rc != RCL_RET_OK) {
    printf("Error while cleaning up!\n");
    return -1;
  }
  return 0;
}
