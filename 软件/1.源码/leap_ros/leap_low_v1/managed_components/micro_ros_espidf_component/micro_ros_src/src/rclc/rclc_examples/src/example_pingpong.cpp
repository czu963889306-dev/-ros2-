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
#include<iostream>
#include<functional>
#include <stdio.h>
#include <std_msgs/msg/string.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
// #include "example_pingpong_helper.h"


// these data structures for the publisher and subscriber are global, so that
// they can be configured in main() and can be used in the corresponding callback.
rcl_publisher_t ping_publisher;

rcl_publisher_t pong_publisher;

// ping node
std_msgs__msg__String pingNode_ping_msg;
std_msgs__msg__String pingNode_pong_msg;

// pong node
std_msgs__msg__String pongNode_ping_msg;
std_msgs__msg__String pongNode_pong_msg;


class MySubscription {
public: 
  MySubscription(){};
  static void on_update(const void * msgin) {
    const std_msgs__msg__String * msg = (const std_msgs__msg__String *)msgin;
    if (msg == NULL) {
      printf("Callback: msg NULL\n");
    } else {
      printf("CLASS Callback: I heard: %s\n", msg->data.data);
    }
    // number++; not possible to use member variables
  }
  private:
    int number;

};

/***************************** PING NODE CALLBACKS ***********************************/

void ping_timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  rcl_ret_t rc;
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    //printf("Timer: time since last call %d\n", (int) last_call_time);
    rc = rcl_publish(&ping_publisher, &pingNode_ping_msg, NULL);
    if (rc == RCL_RET_OK) {
      printf("Published message %s\n", pingNode_ping_msg.data.data);
    } else {
      printf("timer_callback: Error publishing message %s\n", pingNode_ping_msg.data.data);
    }
  } else {
    printf("timer_callback Error: timer parameter is NULL\n");
  }
}

void pong_subscription_callback(const void * msgin)
{
  // pong_subscription_callback_on_update(msgin);
  
  const std_msgs__msg__String * msg = (const std_msgs__msg__String *)msgin;
  if (msg == NULL) {
    printf("Callback: msg NULL\n");
  } else {
    printf("Callback: I heard: %s\n", msg->data.data);
  }
  
}



/***************************** PONG NODE CALLBACKS ***********************************/

void ping_subscription_callback(const void * msgin)
{
  const std_msgs__msg__String * msg = (const std_msgs__msg__String *)msgin;
  if (msg == NULL) {
    printf("Callback: msg NULL\n");
  } else {
    printf("Callback: I heard: %s\n", msg->data.data);
  }
}

void pong_timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  rcl_ret_t rc;
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    //printf("Timer: time since last call %d\n", (int) last_call_time);
    rc = rcl_publish(&pong_publisher, &pongNode_pong_msg, NULL);
    if (rc == RCL_RET_OK) {
      printf("Published message %s\n", pongNode_pong_msg.data.data);
    } else {
      printf("timer_callback: Error publishing message %s\n", pongNode_pong_msg.data.data);
    }
  } else {
    printf("timer_callback Error: timer parameter is NULL\n");
  }
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

//*******************************************************//
  // create rcl_node ping
  rcl_node_t ping_node ;
  rc = rclc_node_init_default(&ping_node, "ping", "", &support);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_node_init_default\n");
    return -1;
  }

  // create a publisher to publish topic 'topic_0' with type std_msg::msg::String
  // my_pub is global, so that the timer callback can access this publisher.
  const char * ping_topic_name = "ping";
  const rosidl_message_type_support_t * ping_type_support =
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);


  const char * pong_topic_name = "pong";
  const rosidl_message_type_support_t * pong_type_support =
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);


  rc = rclc_publisher_init_default(
    &ping_publisher,
    &ping_node,
    ping_type_support,
    ping_topic_name);
  if (RCL_RET_OK != rc) {
    printf("Error in rclc_publisher_init_default %s.\n", ping_topic_name);
    return -1;
  }

  // create a timer, which will call the publisher with period=`timer_timeout` ms in the 'my_timer_callback'
  rcl_timer_t ping_timer ;
  const unsigned int timer_timeout = 50; //50 milliseconds userdefined value
  rc = rclc_timer_init_default(
    &ping_timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    ping_timer_callback);
  if (rc != RCL_RET_OK) {
    printf("Error in rcl_timer_init_default.\n");
    return -1;
  } else {
    printf("Created timer with timeout %d ms.\n", timer_timeout);
  }

  // assign message to publisher
  std_msgs__msg__String__init(&pingNode_ping_msg);
  const unsigned int PUB_MSG_CAPACITY = 20;
  pingNode_ping_msg.data.data = (char *) allocator.reallocate(pingNode_ping_msg.data.data, PUB_MSG_CAPACITY, allocator.state);
  pingNode_ping_msg.data.capacity = PUB_MSG_CAPACITY;
  snprintf(pingNode_ping_msg.data.data, pingNode_ping_msg.data.capacity, "AAAAAAAAAAAAAAAAAAA");
  pingNode_ping_msg.data.size = strlen(pingNode_ping_msg.data.data);

  // ************ create subscription
  rcl_subscription_t pong_subscription;
  rc = rclc_subscription_init_default(
    &pong_subscription,
    &ping_node,
    pong_type_support,
    pong_topic_name);
  if (rc != RCL_RET_OK) {
    printf("Failed to create subscriber %s.\n", pong_topic_name);
    return -1;
  } else {
    printf("Created subscriber %s:\n", pong_topic_name);
  }

  // one string message for subscriber
  std_msgs__msg__String__init(&pingNode_pong_msg);

//*******************************************************//



//*******************************************************//
  // create rcl_node pong
  rcl_node_t pong_node ;
  rc = rclc_node_init_default(&pong_node, "pong", "", &support);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_node_init_default\n");
    return -1;
  }

  // create a publisher to publish topic 'topic_0' with type std_msg::msg::String
  // my_pub is global, so that the timer callback can access this publisher.
  
  /* <jst3si>  duplicated declaration
  const char * ping_topic_name = "ping";
  const rosidl_message_type_support_t * ping_type_support =
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);


  const char * pong_topic_name = "pong";
  const rosidl_message_type_support_t * pong_type_support =
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);
 */

  rc = rclc_publisher_init_default(
    &pong_publisher,
    &pong_node,
    pong_type_support,
    pong_topic_name);
  if (RCL_RET_OK != rc) {
    printf("Error in rclc_publisher_init_default %s.\n", pong_topic_name);
    return -1;
  }

  // create a timer, which will call the publisher with period=`timer_timeout` ms in the 'my_timer_callback'
  rcl_timer_t pong_timer ;
  const unsigned int pong_timer_timeout = 50; //50 milliseconds userdefined value
  rc = rclc_timer_init_default(
    &pong_timer,
    &support,
    RCL_MS_TO_NS(pong_timer_timeout),
    pong_timer_callback);
  if (rc != RCL_RET_OK) {
    printf("Error in rcl_timer_init_default.\n");
    return -1;
  } else {
    printf("Created timer with timeout %d ms.\n", timer_timeout);
  }

  // assign message to publisher
  std_msgs__msg__String__init(&pongNode_pong_msg);
  //const unsigned int PUB_MSG_CAPACITY = 20;
  pongNode_pong_msg.data.data = (char *) allocator.reallocate(pongNode_pong_msg.data.data, PUB_MSG_CAPACITY, allocator.state);
  pongNode_pong_msg.data.capacity = PUB_MSG_CAPACITY;
  snprintf(pongNode_pong_msg.data.data, pongNode_pong_msg.data.capacity, "BAAAAAAAAAAAAAAAAAAA");
  pongNode_pong_msg.data.size = strlen(pongNode_pong_msg.data.data);

  // ************ create subscription
  rcl_subscription_t ping_subscription;
  rc = rclc_subscription_init_default(
    &ping_subscription,
    &pong_node,
    ping_type_support,
    ping_topic_name);
  if (rc != RCL_RET_OK) {
    printf("Failed to create subscriber %s.\n", ping_topic_name);
    return -1;
  } else {
    printf("Created subscriber %s:\n", ping_topic_name);
  }

  // one string message for subscriber
  std_msgs__msg__String__init(&pongNode_ping_msg);

//*******************************************************//




  ////////////////////////////////////////////////////////////////////////////
  // Configuration of RCL Executor
  ////////////////////////////////////////////////////////////////////////////
  bool oneExecutor = false; // false => using two exectors
  if (oneExecutor)
  {
    rclc_executor_t executor;
    executor = rclc_executor_get_zero_initialized_executor();
    // total number of handles = #subscriptions + #timers + #Services (in below case services are 0)
    unsigned int num_handles = 2 + 2;
    printf("Debug: number of DDS handles: %u\n", num_handles);
    rclc_executor_init(&executor, &support.context, num_handles, &allocator);

  //add publisher (timer)
  rc= rclc_executor_add_timer(&executor, &ping_timer);

    if (rc != RCL_RET_OK) {
      printf("Error in rclc_executor_add_timer.\n");
    }
  rc= rclc_executor_add_timer(&executor, &pong_timer);

    if (rc != RCL_RET_OK) {
      printf("Error in rclc_executor_add_timer.\n");
    }

    // add subscription to executor
    rc = rclc_executor_add_subscription(
      &executor, &pong_subscription, &pingNode_pong_msg, &pong_subscription_callback,
      ON_NEW_DATA);

    if (rc != RCL_RET_OK) {
      printf("Error in rclc_executor_add_subscription. \n");
    }
    
      rc = rclc_executor_add_subscription(
      &executor, &ping_subscription, &pongNode_ping_msg, &ping_subscription_callback,
      ON_NEW_DATA);

    if (rc != RCL_RET_OK) {
      printf("Error in rclc_executor_add_subscription. \n");
    }

  
    // Optional prepare for avoiding allocations during spin
    rclc_executor_prepare(&executor);

    // rclc_executor_spin(&executor ); end less loop

    for (unsigned int i = 0; i < 10; i++) {
        // timeout specified in nanoseconds (here 1s)
      rclc_executor_spin_some(&executor, 1000 * (1000 * 1000));
    }

    // clean up
    rc = rclc_executor_fini(&executor);
    rc += rcl_publisher_fini(&ping_publisher, &ping_node);
    rc += rcl_publisher_fini(&pong_publisher, &pong_node);
    rc += rcl_timer_fini(&ping_timer);
    rc += rcl_timer_fini(&pong_timer);
    rc += rcl_subscription_fini(&pong_subscription, &ping_node);
    rc += rcl_subscription_fini(&ping_subscription, &pong_node);
    rc += rcl_node_fini(&ping_node);
    rc += rcl_node_fini(&pong_node);
    rc += rclc_support_fini(&support);

    std_msgs__msg__String__fini(&pingNode_ping_msg);
    std_msgs__msg__String__fini(&pingNode_pong_msg);
    std_msgs__msg__String__fini(&pongNode_ping_msg);
    std_msgs__msg__String__fini(&pongNode_pong_msg);

    if (rc != RCL_RET_OK) {
      printf("Error while cleaning up!\n");
      return -1;
    }
  } else {
    // use two executors

    // executor for ping node 
    rclc_executor_t ping_executor;
    ping_executor = rclc_executor_get_zero_initialized_executor();
    // total number of handles = #subscriptions + #timers + #Services (in below case services are 0)
    // Note:
    // If you need more than the default number of publisher/subscribers, etc., you
    // need to configure the micro-ROS middleware also!
    // See documentation in the executor.h at the function rclc_executor_init()
    // for more details.
    unsigned int pingNode_num_handles = 1 + 1;
    printf("Debug: number of DDS handles: %u\n", pingNode_num_handles);
    rclc_executor_init(&ping_executor, &support.context, pingNode_num_handles, &allocator);

    //add publisher (timer) for ping_msg
    rc= rclc_executor_add_timer(&ping_executor, &ping_timer);
    if (rc != RCL_RET_OK) {
      printf("Error in rclc_executor_add_timer.\n");
    }
 
    // add subscription for pong_msg
    // rc = rclc_executor_add_subscription(
    //   &ping_executor, &pong_subscription, &pingNode_pong_msg, &pong_subscription_callback,
    //  ON_NEW_DATA);

    rc = rclc_executor_add_subscription(
      &ping_executor, &pong_subscription, &pingNode_pong_msg, &MySubscription::on_update,
      ON_NEW_DATA);

    if (rc != RCL_RET_OK) {
      printf("Error in rclc_executor_add_subscription. \n");
    }
    


    // pong node
    rclc_executor_t pong_executor;
    pong_executor = rclc_executor_get_zero_initialized_executor();
    // total number of handles = #subscriptions + #timers + #Services (in below case services are 0)
    unsigned int pongNode_num_handles = 1 + 1;
    printf("Debug: number of DDS handles: %u\n", pongNode_num_handles);
    rclc_executor_init(&pong_executor, &support.context, pongNode_num_handles, &allocator);
    
    // add subscription of ping_msg
    rc = rclc_executor_add_subscription(
    &pong_executor, &ping_subscription, &pongNode_ping_msg, &ping_subscription_callback,
    ON_NEW_DATA);
    if (rc != RCL_RET_OK) {
      printf("Error in rclc_executor_add_subscription. \n");
    }
    // add publisher (timer) of pong_msg
    rc= rclc_executor_add_timer(&pong_executor, &pong_timer);
    if (rc != RCL_RET_OK) {
      printf("Error in rclc_executor_add_timer.\n");
    }

    // Optional: prepare for avoiding allocations during spin
    rclc_executor_prepare(&ping_executor);
    rclc_executor_prepare(&pong_executor);

    for (unsigned int i = 0; i < 10; i++) {
        // timeout specified in nanoseconds (here 1s)
      rclc_executor_spin_some(&ping_executor, RCL_MS_TO_NS( 1000 ));
      rclc_executor_spin_some(&pong_executor, RCL_MS_TO_NS( 1000 ));
    }

    // clean up
    rc = rclc_executor_fini(&ping_executor);
    rc = rclc_executor_fini(&pong_executor);

    rc += rcl_publisher_fini(&ping_publisher, &ping_node);
    rc += rcl_publisher_fini(&pong_publisher, &pong_node);
    rc += rcl_timer_fini(&ping_timer);
    rc += rcl_timer_fini(&pong_timer);
    rc += rcl_subscription_fini(&pong_subscription, &ping_node);
    rc += rcl_subscription_fini(&ping_subscription, &pong_node);
    rc += rcl_node_fini(&ping_node);
    rc += rcl_node_fini(&pong_node);
    rc += rclc_support_fini(&support);

    std_msgs__msg__String__fini(&pingNode_ping_msg);
    std_msgs__msg__String__fini(&pingNode_pong_msg);
    std_msgs__msg__String__fini(&pongNode_ping_msg);
    std_msgs__msg__String__fini(&pongNode_pong_msg);

    if (rc != RCL_RET_OK) {
      printf("Error while cleaning up!\n");
      return -1;
    }

  }

  return 0;
}