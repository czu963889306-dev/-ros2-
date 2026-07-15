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
#ifdef _WIN32
  #include <windows.h>
#else
  #include <pthread.h>
#endif

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <example_interfaces/action/fibonacci.h>

const char * goalResult[] =
{[GOAL_STATE_SUCCEEDED] = "succeeded", [GOAL_STATE_CANCELED] = "canceled",
  [GOAL_STATE_ABORTED] = "aborted"};

#ifdef _WIN32
DWORD WINAPI fibonacci_worker(LPVOID args)
#else
void * fibonacci_worker(void * args)
#endif
{
  (void) args;
  rclc_action_goal_handle_t * goal_handle = (rclc_action_goal_handle_t *) args;
  rcl_action_goal_state_t goal_state;

  example_interfaces__action__Fibonacci_SendGoal_Request * req =
    (example_interfaces__action__Fibonacci_SendGoal_Request *) goal_handle->ros_goal_request;

  example_interfaces__action__Fibonacci_GetResult_Response response = {0};
  example_interfaces__action__Fibonacci_FeedbackMessage feedback;

  if (req->goal.order < 2) {
    goal_state = GOAL_STATE_ABORTED;
  } else {
    feedback.feedback.sequence.capacity = req->goal.order;
    feedback.feedback.sequence.size = 0;
    feedback.feedback.sequence.data =
      (int32_t *) malloc(feedback.feedback.sequence.capacity * sizeof(int32_t));

    feedback.feedback.sequence.data[0] = 0;
    feedback.feedback.sequence.data[1] = 1;
    feedback.feedback.sequence.size = 2;

    for (size_t i = 2; i < (size_t) req->goal.order && !goal_handle->goal_cancelled; i++) {
      feedback.feedback.sequence.data[i] = feedback.feedback.sequence.data[i - 1] +
        feedback.feedback.sequence.data[i - 2];
      feedback.feedback.sequence.size++;

      if (i > 30) {
        goal_state = GOAL_STATE_ABORTED;
        break;
      }

      rclc_action_publish_feedback(goal_handle, &feedback);
      rclc_sleep_ms(100);
    }

    if (!goal_handle->goal_cancelled) {
      response.result.sequence.capacity = feedback.feedback.sequence.capacity;
      response.result.sequence.size = feedback.feedback.sequence.size;
      response.result.sequence.data = feedback.feedback.sequence.data;
      goal_state = GOAL_STATE_SUCCEEDED;
    } else {
      goal_state = GOAL_STATE_CANCELED;
    }
  }

  printf("Goal %d %s\n", req->goal.order, goalResult[goal_state]);

  rcl_ret_t rc;
  do {
    rc = rclc_action_send_result(goal_handle, goal_state, &response);
    rclc_sleep_ms(1e3);
  } while (rc != RCL_RET_OK);

  free(feedback.feedback.sequence.data);
#ifdef _WIN32
  return 0;
#else
  pthread_exit(NULL);
#endif
}

rcl_ret_t handle_goal(rclc_action_goal_handle_t * goal_handle, void * context)
{
  (void) context;

  example_interfaces__action__Fibonacci_SendGoal_Request * req =
    (example_interfaces__action__Fibonacci_SendGoal_Request *) goal_handle->ros_goal_request;

  // Too big, rejecting
  if (req->goal.order > 200) {
    printf("Goal %d rejected\n", req->goal.order);
    return RCL_RET_ACTION_GOAL_REJECTED;
  }

  printf("Goal %d accepted\n", req->goal.order);

#ifdef _WIN32
  HANDLE thread_id = CreateThread(
    NULL, 0, (LPTHREAD_START_ROUTINE) fibonacci_worker, goal_handle, 0, NULL);
  CloseHandle(thread_id);  // libère immédiatement le handle, le thread continue
#else
  pthread_t * thread_id = malloc(sizeof(pthread_t));
  pthread_create(thread_id, NULL, fibonacci_worker, goal_handle);
#endif
  return RCL_RET_ACTION_GOAL_ACCEPTED;
}

bool handle_cancel(rclc_action_goal_handle_t * goal_handle, void * context)
{
  (void) context;
  (void) goal_handle;

  return true;
}

int main()
{
  rcl_allocator_t allocator = rcl_get_default_allocator();

  // Create init_options
  rclc_support_t support;
  rclc_support_init(&support, 0, NULL, &allocator);

  // Create node
  rcl_node_t node;
  rclc_node_init_default(&node, "demo_action_server_node", "", &support);

  // Create action service
  rclc_action_server_t action_server;
  rclc_action_server_init_default(
    &action_server,
    &node,
    &support,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );

  // Create executor
  // Note:
  // If you need more than the default number of publisher/subscribers, etc., you
  // need to configure the micro-ROS middleware also!
  // See documentation in the executor.h at the function rclc_executor_init()
  // for more details.
  rclc_executor_t executor;
  rclc_executor_init(&executor, &support.context, 1, &allocator);

  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request[10];

  rclc_executor_add_action_server(
    &executor,
    &action_server,
    10,
    ros_goal_request,
    sizeof(example_interfaces__action__Fibonacci_SendGoal_Request),
    handle_goal,
    handle_cancel,
    (void *) &action_server);

  while (1) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
    rclc_sleep_ms(100);
  }

  // clean up
  rclc_executor_fini(&executor);
  (void)!rcl_node_fini(&node);

  return 0;
}
