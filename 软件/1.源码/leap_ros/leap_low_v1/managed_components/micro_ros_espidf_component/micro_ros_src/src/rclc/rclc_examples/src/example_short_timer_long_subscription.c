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
// limitations under the License.

#include <stdio.h>
#include <std_msgs/msg/int32.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>

// these data structures for the publisher and subscriber are global, so that
// they can be configured in main() and can be used in the corresponding callback.
rcl_publisher_t my_pub;
std_msgs__msg__Int32 pub_msg;
std_msgs__msg__Int32 sub_msg;
unsigned int short_timer_counter = 0;

/***************************** CALLBACKS ***********************************/

void my_subscriber_callback(const void * msgin)
{
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  if (msg == NULL) {
    printf("Callback: msg NULL\n");
  } else {
    printf("Callback: I heard: %d\n", msg->data);
  }
  if (msg->data % 2)
  {
    rclc_sleep_ms(900);
  }
}

void my_timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  rcl_ret_t rc;
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    //printf("Timer: time since last call %d\n", (int) last_call_time);
    rc = rcl_publish(&my_pub, &pub_msg, NULL);

    if (rc == RCL_RET_OK) {
      // printf("Published message %d\n", pub_msg.data);
    } else {
      printf("timer_callback: Error publishing message %d\n", pub_msg.data);
    }
    pub_msg.data++;
  } else {
    printf("timer_callback Error: timer parameter is NULL\n");
  }
}

void short_timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(timer);
  RCLC_UNUSED(last_call_time);
  printf("shorttimer %d\n",short_timer_counter++);
}

/******************** MAIN PROGRAM ****************************************/
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
  rcl_node_t my_node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&my_node, "node_0", "", &support);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_node_init_default\n");
    return -1;
  }

  // create a publisher to publish topic 'topic_0' with type std_msg::msg::String
  // my_pub is global, so that the timer callback can access this publisher.
  const char * topic_name = "topic_0";
  const rosidl_message_type_support_t * my_type_support =
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32);

  rc = rclc_publisher_init_default(
    &my_pub,
    &my_node,
    my_type_support,
    topic_name);
  if (RCL_RET_OK != rc) {
    printf("Error in rclc_publisher_init_default %s.\n", topic_name);
    return -1;
  }

  // create a timer, which will call the publisher with period=`timer_timeout` ms in the 'my_timer_callback'
  rcl_timer_t my_timer = rcl_get_zero_initialized_timer();
  const unsigned int timer_timeout = 1000;
  rc = rclc_timer_init_default(
    &my_timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    my_timer_callback);
  if (rc != RCL_RET_OK) {
    printf("Error in rcl_timer_init_default.\n");
    return -1;
  } else {
    printf("Created timer with timeout %d ms.\n", timer_timeout);
  }

  rcl_timer_t short_timer = rcl_get_zero_initialized_timer();
  const unsigned int short_timer_timeout = 100;
  rc = rclc_timer_init_default(
    &short_timer,
    &support,
    RCL_MS_TO_NS(short_timer_timeout),
    short_timer_callback);
  if (rc != RCL_RET_OK) {
    printf("Error in rcl_timer_init_default.\n");
    return -1;
  } else {
    printf("Created timer with timeout %d ms.\n", short_timer_timeout);
  }

  // assign message to publisher
  pub_msg.data = 1;

  // create subscription
  rcl_subscription_t my_sub = rcl_get_zero_initialized_subscription();
  rc = rclc_subscription_init_default(
    &my_sub,
    &my_node,
    my_type_support,
    topic_name);
  if (rc != RCL_RET_OK) {
    printf("Failed to create subscriber %s.\n", topic_name);
    return -1;
  } else {
    printf("Created subscriber %s:\n", topic_name);
  }

  // initialize subscription message
  std_msgs__msg__Int32__init(&sub_msg);

  ////////////////////////////////////////////////////////////////////////////
  // Configuration of RCL Executor
  ////////////////////////////////////////////////////////////////////////////
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  // total number of handles = #subscriptions + #timers
  // check also xrce-dds configuration for maximum number of publisher, 
  // subscribers, timers etc. 
  // Note:
  // If you need more than the default number of publisher/subscribers, etc., you
  // need to configure the micro-ROS middleware also!
  // See documentation in the executor.h at the function rclc_executor_init()
  // for more details.
  unsigned int num_handles = 1 + 2;
  printf("Debug: number of DDS handles: %u\n", num_handles);
  rclc_executor_init(&executor, &support.context, num_handles, &allocator);

  // add subscription to executor
  rc = rclc_executor_add_subscription(
    &executor, &my_sub, &sub_msg, &my_subscriber_callback,
    ON_NEW_DATA);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_subscription. \n");
  }

  rclc_executor_add_timer(&executor, &my_timer);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_timer.\n");
  }

  rclc_executor_add_timer(&executor, &short_timer);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_timer.\n");
  }
  // Start Executor
  rclc_executor_spin(&executor);

  // clean up
  rc = rclc_executor_fini(&executor);
  rc += rcl_publisher_fini(&my_pub, &my_node);
  rc += rcl_timer_fini(&my_timer);
  rc += rcl_subscription_fini(&my_sub, &my_node);
  rc += rcl_node_fini(&my_node);
  rc += rclc_support_fini(&support);

  std_msgs__msg__Int32__fini(&pub_msg);
  std_msgs__msg__Int32__fini(&sub_msg);

  if (rc != RCL_RET_OK) {
    printf("Error while cleaning up!\n");
    return -1;
  }
  return 0;
}
