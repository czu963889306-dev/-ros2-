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

#include <std_msgs/msg/string.h>
#include <std_msgs/msg/int32.h>

#include <rclc/executor.h>
#include <rclc/rclc.h>

// these data structures for the publisher and subscriber are global, so that
// they can be configured in main() and can be used in the corresponding callback.
rcl_publisher_t my_string_pub;
rcl_publisher_t my_int_pub;
std_msgs__msg__Int32 int_pub_msg;
int int_pub_value;
int string_pub_value;

std_msgs__msg__String string_sub_msg;
std_msgs__msg__Int32 int_sub_msg;


/***************************** CALLBACKS ***********************************/

typedef struct
{
  rcl_timer_t * timer1;
  rcl_timer_t * timer2;
} pub_trigger_object_t;

typedef struct
{
  rcl_subscription_t * sub1;
  rcl_subscription_t * sub2;
} sub_trigger_object_t;

bool pub_trigger(rclc_executor_handle_t * handles, unsigned int size, void * obj)
{
  if (handles == NULL) {
    printf("Error in pub_trigger: 'handles' is a NULL pointer\n");
    return false;
  }
  if (obj == NULL) {
    printf("Error in pub_trigger: 'obj' is a NULL pointer\n");
    return false;
  }
  pub_trigger_object_t * comm_obj = (pub_trigger_object_t *) obj;
  bool timer1 = false;
  bool timer2 = false;
  //printf("pub_trigger ready set: ");
  for (unsigned int i = 0; i < size; i++) {
    if (handles[i].data_available) {
      void * handle_ptr = rclc_executor_handle_get_ptr(&handles[i]);
      if (handle_ptr == comm_obj->timer1) {
        timer1 = true;
        //printf("timer1 ");
      }
      if (handle_ptr == comm_obj->timer2) {
        timer2 = true;
        //printf("timer2 ");
      }

    }
  }
  //printf("\n");
  return timer1 || timer2;
}


bool sub_trigger(rclc_executor_handle_t * handles, unsigned int size, void * obj)
{
  if (handles == NULL) {
    printf("Error in sub_trigger: 'handles' is a NULL pointer\n");
    return false;
  }
  if (obj == NULL) {
    printf("Error in sub_trigger: 'obj' is a NULL pointer\n");
    return false;
  }
  sub_trigger_object_t * comm_obj = (sub_trigger_object_t *) obj;
  bool sub1 = false;
  bool sub2 = false;
  //printf("sub_trigger ready set: ");
  for (unsigned int i = 0; i < size; i++) {
    if (handles[i].data_available) {
      void * handle_ptr = rclc_executor_handle_get_ptr(&handles[i]);

      if (handle_ptr == comm_obj->sub1) {
        sub1 = true;
        //printf("sub1 ");
      }

      if (handle_ptr == comm_obj->sub2) {
        sub2 = true;
        //printf("sub2 ");
      }
    }
  }
  //printf("\n");
  return sub1 && sub2;

}

void my_string_subscriber_callback(const void * msgin)
{
  const std_msgs__msg__String * msg = (const std_msgs__msg__String *)msgin;
  if (msg == NULL) {
    printf("my_string_subscriber_callback: msgin is NULL\n");
  } else {
    printf("Callback 1: %s\n", msg->data.data);
  }
}


void my_int_subscriber_callback(const void * msgin)
{
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  if (msg == NULL) {
    printf("my_int_subscriber_callback: msgin is NULL\n");
  } else {
    printf("Callback 2: %d\n", msg->data);
  }
}

#define RCLC_UNUSED(x) (void)x

void my_timer_string_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  rcl_ret_t rc;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    //printf("Timer: time since last call %d\n", (int) last_call_time);

    std_msgs__msg__String pub_msg;
    std_msgs__msg__String__init(&pub_msg);
    const unsigned int PUB_MSG_CAPACITY = 20;
    pub_msg.data.data = allocator.reallocate(pub_msg.data.data, PUB_MSG_CAPACITY, allocator.state);
    pub_msg.data.capacity = PUB_MSG_CAPACITY;
    snprintf(pub_msg.data.data, pub_msg.data.capacity, "Hello World!%d", string_pub_value++);
    pub_msg.data.size = strlen(pub_msg.data.data);

    rc = rcl_publish(&my_string_pub, &pub_msg, NULL);
    if (rc == RCL_RET_OK) {
      printf("Published: %s\n", pub_msg.data.data);
    } else {
      printf("Error in my_timer_string_callback: publishing message %s\n", pub_msg.data.data);
    }
    std_msgs__msg__String__fini(&pub_msg);
  } else {
    printf("Error in my_timer_string_callback: timer parameter is NULL\n");
  }
}

void my_timer_int_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  rcl_ret_t rc;
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    //printf("Timer: time since last call %d\n", (int) last_call_time);
    int_pub_msg.data = int_pub_value++;
    rc = rcl_publish(&my_int_pub, &int_pub_msg, NULL);
    if (rc == RCL_RET_OK) {
      printf("Published: %d\n", int_pub_msg.data);
    } else {
      printf("Error in my_timer_int_callback: publishing message %d\n", int_pub_msg.data);
    }
  } else {
    printf("Error in my_timer_int_callback: timer parameter is NULL\n");
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

  // create rcl_node
  rcl_node_t my_node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&my_node, "node_0", "executor_examples", &support);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_node_init_default\n");
    return -1;
  }

  // create a publisher 1
  // - topic name: 'topic_0'
  // - message type: std_msg::msg::String
  const char * topic_name = "topic_0";
  const rosidl_message_type_support_t * my_type_support =
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);

  rc = rclc_publisher_init_default(
    &my_string_pub,
    &my_node,
    my_type_support,
    topic_name);
  if (RCL_RET_OK != rc) {
    printf("Error in rclc_publisher_init_default %s.\n", topic_name);
    return -1;
  }

  // create timer 1
  // - publishes 'my_string_pub' every 'timer_timeout' ms
  rcl_timer_t my_string_timer = rcl_get_zero_initialized_timer();
  const unsigned int timer_timeout = 100;
  rc = rclc_timer_init_default(
    &my_string_timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    my_timer_string_callback);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_timer_init_default.\n");
    return -1;
  } else {
    printf("Created timer 'my_string_timer' with timeout %d ms.\n", timer_timeout);
  }

  // create publisher 2
  // - topic name: 'topic_1'
  // - message type: std_msg::msg::Int
  const char * topic_name_1 = "topic_1";
  const rosidl_message_type_support_t * my_int_type_support =
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32);
  rc = rclc_publisher_init_default(
    &my_int_pub,
    &my_node,
    my_int_type_support,
    topic_name_1);
  if (RCL_RET_OK != rc) {
    printf("Error in rclc_publisher_init_default %s.\n", topic_name_1);
    return -1;
  }

  // create timer 2
  // - publishes 'my_int_pub' every 'timer_int_timeout' ms
  rcl_timer_t my_int_timer = rcl_get_zero_initialized_timer();
  const unsigned int timer_int_timeout = 10 * timer_timeout;
  rc = rclc_timer_init_default(
    &my_int_timer,
    &support,
    RCL_MS_TO_NS(timer_int_timeout),
    my_timer_int_callback);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_timer_init_default.\n");
    return -1;
  } else {
    printf("Created timer with timeout %d ms.\n", timer_int_timeout);
  }

  // initialized messages and counter variables
  // the string publisher message 'pub_msg' is assigned in the callback
  std_msgs__msg__Int32__init(&int_pub_msg);
  int_pub_value = 0;
  string_pub_value = 0;

  // create subscription 1
  rcl_subscription_t my_string_sub = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t my_subscription_options = rcl_subscription_get_default_options();
  my_subscription_options.qos.depth = 0; // qos: last is best = register semantics
  rc = rcl_subscription_init(
    &my_string_sub,
    &my_node,
    my_type_support,
    topic_name,
    &my_subscription_options);

  if (rc != RCL_RET_OK) {
    printf("Failed to create subscriber %s.\n", topic_name);
    return -1;
  } else {
    printf("Created subscriber %s:\n", topic_name);
  }
  // initialize subscription message
  std_msgs__msg__String__init(&string_sub_msg);


  // create subscription 2
  rcl_subscription_t my_int_sub = rcl_get_zero_initialized_subscription();
  rc = rclc_subscription_init_default(
    &my_int_sub,
    &my_node,
    my_int_type_support,
    topic_name_1);
  if (rc != RCL_RET_OK) {
    printf("Failed to create subscriber %s.\n", topic_name_1);
    return -1;
  } else {
    printf("Created subscriber %s:\n", topic_name_1);
  }
  // initialize subscription message
  std_msgs__msg__Int32__init(&int_sub_msg);

  ////////////////////////////////////////////////////////////////////////////
  // Configuration of RCL Executor
  ////////////////////////////////////////////////////////////////////////////
  rclc_executor_t executor_pub;
  rclc_executor_t executor_sub;

  // Executor
  // Note:
  // If you need more than the default number of publisher/subscribers, etc., you
  // need to configure the micro-ROS middleware also!
  // See documentation in the executor.h at the function rclc_executor_init()
  // for more details.
  unsigned int num_handles_pub = 2;
  printf("Executor_pub: number of DDS handles: %u\n", num_handles_pub);
  rclc_executor_init(&executor_pub, &support.context, num_handles_pub, &allocator);

  rc = rclc_executor_add_timer(&executor_pub, &my_string_timer);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_timer 'my_string_timer'.\n");
  }

  rc = rclc_executor_add_timer(&executor_pub, &my_int_timer);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_timer 'my_int_timer'.\n");
  }

  // Executor for subscribing messages
  unsigned int num_handles_sub = 2;
  printf("Executor_sub: number of DDS handles: %u\n", num_handles_sub);
  rclc_executor_init(&executor_sub, &support.context, num_handles_sub, &allocator);

  // add subscription to executor
  rc = rclc_executor_add_subscription(
    &executor_sub, &my_string_sub, &string_sub_msg,
    &my_string_subscriber_callback,
    ON_NEW_DATA);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_subscription 'my_string_sub'. \n");
  }

  // add int subscription to executor
  rc = rclc_executor_add_subscription(
    &executor_sub, &my_int_sub, &int_sub_msg,
    &my_int_subscriber_callback,
    ON_NEW_DATA);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_subscription 'my_int_sub'. \n");
  }

  // pub_trigger_object_t comm_obj_pub;
  // comm_obj_pub.timer1 = &my_string_timer;
  // comm_obj_pub.timer2 = &my_int_timer;

  // sub_trigger_object_t comm_obj_sub;
  // comm_obj_sub.sub1 = &my_string_sub;
  // comm_obj_sub.sub2 = &my_int_sub;
  // rc = rclc_executor_set_trigger(&executor_pub, pub_trigger, &comm_obj_pub);
  // rc = rclc_executor_set_trigger(&executor_sub, sub_trigger, &comm_obj_sub);

  rc = rclc_executor_set_trigger(&executor_pub, rclc_executor_trigger_any, NULL);
  rc = rclc_executor_set_trigger(&executor_sub, rclc_executor_trigger_all, NULL);

  for (unsigned int i = 0; i < 100; i++) {
    // timeout specified in ns                 (here: 1s)
    rclc_executor_spin_some(&executor_pub, 1000 * (1000 * 1000));
    rclc_sleep_ms(1); // 1ms
    rclc_executor_spin_some(&executor_sub, 1000 * (1000 * 1000));
  }

  // clean up
  rc = rclc_executor_fini(&executor_pub);
  rc += rclc_executor_fini(&executor_sub);
  rc += rcl_publisher_fini(&my_string_pub, &my_node);
  rc += rcl_publisher_fini(&my_int_pub, &my_node);
  rc += rcl_timer_fini(&my_string_timer);
  rc += rcl_timer_fini(&my_int_timer);
  rc += rcl_subscription_fini(&my_string_sub, &my_node);
  rc += rcl_subscription_fini(&my_int_sub, &my_node);
  rc += rcl_node_fini(&my_node);
  rc += rclc_support_fini(&support);

  std_msgs__msg__Int32__fini(&int_pub_msg);
  std_msgs__msg__String__fini(&string_sub_msg);
  std_msgs__msg__Int32__fini(&int_sub_msg);

  if (rc != RCL_RET_OK) {
    printf("Error while cleaning up!\n");
    return -1;
  }
  return 0;
}
