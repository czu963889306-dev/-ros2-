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
#include <std_msgs/msg/int32.h>
#include <example_interfaces/srv/add_two_ints.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <vector>

#include "rclc/executor.h"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcutils/time.h"

// Include backport of function 'rcl_wait_set_is_valid' introduced in Foxy
// in case of building for Dashing and Eloquent. This pre-processor macro
// is defined in CMakeLists.txt.
#if defined (USE_RCL_WAIT_SET_IS_VALID_BACKPORT)
#include "rclc/rcl_wait_set_is_valid_backport.h"
#endif


// 27.06.2019, unit test adapted from ros2/rcl/rcl_lifecycle/test/test_default_state_machine.cpp
// by Jan Staschulat, under Apache 2.0 License

/******************************* CALLBACKS for subscriptions ************************/

// some variables for testing the LET-semantics
// array holds the msg_id in the order as they are received
// the callback calls the function _results_add(msg_id)
// at the end the array contains the order of received events.
#define TC_SPIN_SOME_NUM_PUBLISHER 3
#define TC_SPIN_SOME_PUBLISHED_MSGS 3
#define TC_SPIN_SOME_MAX_MSGS \
  (TC_SPIN_SOME_NUM_PUBLISHER * TC_SPIN_SOME_PUBLISHED_MSGS)
static unsigned int _executor_results[TC_SPIN_SOME_MAX_MSGS];
static unsigned int _executor_results_i;

// callback for timer
static unsigned int _cbt_cnt = 0;
// callback for topic "chatter1"
static unsigned int _cb1_cnt = 0;
static unsigned int _cb1_int_value = 0;
// callback for topic "chatter2"
static unsigned int _cb2_cnt = 0;
static unsigned int _cb2_int_value = 0;
// callback for topic "chatter3"
static unsigned int _cb3_cnt = 0;
static unsigned int _cb3_int_value = 0;
// callback for testing data communication semantics
static unsigned int _cb5_cnt = 0;
static int _cb5_int_value = 0;
rcl_publisher_t * _pub_int_ptr;
std_msgs__msg__Int32 * _pub_int_msg_ptr;

// client/server test
static unsigned int srv1_cnt = 0;
static unsigned int srv1_value = 0;
static unsigned int srv1_id = 0;
static unsigned int srv1_ctxt = 42;
static unsigned int cli1_cnt = 0;
static unsigned int cli1_value = 0;
static unsigned int cli1_id = 0;

// guard condition test
static unsigned int gc1_cnt = 0;

// sleep time beween publish and receive in DDS middleware
// to allow enough time on CI jobs (in milliseconds)
#define RCLC_UNIT_TEST_SLEEP_TIME_MS 1000
const std::chrono::milliseconds rclc_test_sleep_time =
  std::chrono::milliseconds(RCLC_UNIT_TEST_SLEEP_TIME_MS);

// timeout for rcl_wait() when calling spin_some API of executor
const uint64_t rclc_test_timeout_ns = 1000000000;  // 1s

static
void
_results_callback_counters_init()
{
  _cb1_cnt = 0;
  _cb2_cnt = 0;
  _cb3_cnt = 0;
}

static
void
_results_callback_values_init()
{
  _cb1_int_value = 0;
  _cb2_int_value = 0;
  _cb3_int_value = 0;
}

static
void
_executor_results_init(void)
{
  for (unsigned int i = 0; i < TC_SPIN_SOME_MAX_MSGS; i++) {
    _executor_results[i] = 0;
  }
  _executor_results_i = 0;

  _results_callback_counters_init();
}

static
void
_results_callback_init()
{
  _results_callback_counters_init();
  _results_callback_values_init();
  _executor_results_init();
}

static
void
_results_initialize_service_client()
{
  srv1_cnt = 0;
  srv1_value = 0;
  srv1_id = 0;

  cli1_cnt = 0;
  cli1_value = 0;
  cli1_id = 0;
}

static
unsigned int
_results_callback_num_received()
{
  return _cb1_cnt + _cb2_cnt + _cb3_cnt;
}


/// preserves the order of received data
/// message values are stored in an array (left to right)
/// after the message value is stored, the array index (_executor_results_i)
/// is incremented.
/// assumption msg_id > 0
void
_executor_results_add(unsigned int msg_id)
{
  if (_executor_results_i < TC_SPIN_SOME_MAX_MSGS) {
    _executor_results[_executor_results_i] = msg_id;
    _executor_results_i++;
  } else {
    printf("_executor_results_add: buffer overflow!\n");
  }
}

bool
_executor_results_all_msg_received()
{
  // total number of expected messages is TC_SPIN_SOME_MAX_MSGS
  return _results_callback_num_received() == TC_SPIN_SOME_MAX_MSGS;
}
void
_executor_results_print(void)
{
  printf("Results: ");
  for (unsigned int i = 0; i < TC_SPIN_SOME_MAX_MSGS; i++) {
    // if the value is zero, then it was not used => finished
    // assume positive message_id's
    if (_executor_results[i] > 0) {
      printf("%d ", _executor_results[i]);
    } else {
      break;
    }
  }
  printf("\n");
}

void
_executor_array_print(unsigned int * array, unsigned int max)
{
  printf("Results: ");
  for (unsigned int i = 0; i < max; i++) {
    // if the value is zero, then it was not used => finished
    // assume positive message_id's
    if (array[i] > 0) {
      printf("%d ", array[i]);
    } else {
      break;
    }
  }
  printf("\n");
}

bool
_executor_results_compare(unsigned int * array)
{
  // assume that array has same size as TC_SPIN_SOME_MAX_MSGS
  for (unsigned int i = 0; i < TC_SPIN_SOME_MAX_MSGS; i++) {
    if (_executor_results[i] != array[i]) {
      return false;
    }
  }
  return true;
}

// definition of callbacks
#define INT_CALLBACK_DEFINITION(NUM) \
  void int32_callback_ ## NUM(const void * msgin) \
  { \
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin; \
    if (msg == NULL) { \
      printf("Test CB " #NUM ": msg is NULL\n"); \
    } else { \
      _cb ## NUM ## _int_value = msg->data; \
    } \
    _cb ## NUM ## _cnt++; \
    _executor_results_add(NUM); \
  }

// definition of function pointer
#define INT_CALLBACK(NUM) \
  int32_callback_ ## NUM


// macro-definition               definition of callback function
INT_CALLBACK_DEFINITION(1)  // => int32_callback_1
INT_CALLBACK_DEFINITION(2)  // => int32_callback_2
INT_CALLBACK_DEFINITION(3)  // => int32_callback_3

#define CALLBACK_1 INT_CALLBACK(1)
#define CALLBACK_2 INT_CALLBACK(2)
#define CALLBACK_3 INT_CALLBACK(3)

// for test case semantics
void int32_callback4(const void * msgin)
{
  rcl_ret_t rc;
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  if (msg == NULL) {
    printf("Test CB: msg NULL\n");
  } else {
    _pub_int_msg_ptr->data = 2;
    rc = rcl_publish(_pub_int_ptr, _pub_int_msg_ptr, NULL);
    if (rc != RCL_RET_OK) {
      printf("Error in int32_callback4: could not publish!\n");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));  // 2s
  }
}

// for test case semantics
void int32_callback5(const void * msgin)
{
  _cb5_cnt++;
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  if (msg == NULL) {
    printf("(int32_callback5): msg is NULL\n");
  } else {
    _cb5_int_value = msg->data;
  }
}

void int32_callback_with_context(const void * msgin, void * context)
{
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  if (msg == NULL) {
    printf("(int32_callback_with_context): msg is NULL\n");
  }
  if (context == NULL) {
    printf("(int32_callback_with_context): context is NULL\n");
  } else {
    // This side effect allows the test to check that the subscription received its context
    int32_t * sub_context_value = reinterpret_cast<int32_t *>( context );
    *sub_context_value = msg->data;
  }
}

void service_callback(const void * req_msg, void * resp_msg)
{
  srv1_cnt++;
  printf("received service request\n");
  const example_interfaces__srv__AddTwoInts_Request * req =
    (const example_interfaces__srv__AddTwoInts_Request *) req_msg;
  srv1_value = req->a;

  example_interfaces__srv__AddTwoInts_Response * resp =
    reinterpret_cast<example_interfaces__srv__AddTwoInts_Response *>(resp_msg);
  resp->sum = req->a + req->b;
}

void service_callback_with_reqid(const void * req_msg, rmw_request_id_t * id, void * resp_msg)
{
  srv1_cnt++;
  printf("received service request\n");
  const example_interfaces__srv__AddTwoInts_Request * req =
    (const example_interfaces__srv__AddTwoInts_Request *) req_msg;
  srv1_value = req->a;
  srv1_id = id->sequence_number;

  example_interfaces__srv__AddTwoInts_Response * resp =
    reinterpret_cast<example_interfaces__srv__AddTwoInts_Response *>(resp_msg);
  resp->sum = req->a + req->b;
}

void service_callback_with_context(
  const void * req_msg,
  void * resp_msg,
  void * context)
{
  srv1_cnt++;
  printf("received service request with additional service context\n");
  const example_interfaces__srv__AddTwoInts_Request * req =
    (const example_interfaces__srv__AddTwoInts_Request *) req_msg;
  srv1_value = req->a;
  srv1_id = *((unsigned int *) context);

  example_interfaces__srv__AddTwoInts_Response * resp =
    reinterpret_cast<example_interfaces__srv__AddTwoInts_Response *>(resp_msg);
  resp->sum = req->a + req->b;
}

void client_callback(const void * req_msg)
{
  cli1_cnt++;
  printf("client_callback: received response\n");
  const example_interfaces__srv__AddTwoInts_Response * resp =
    (const example_interfaces__srv__AddTwoInts_Response *) req_msg;
  cli1_value = resp->sum;
}

void client_callback_with_reqid(const void * req_msg, rmw_request_id_t * id)
{
  cli1_cnt++;
  printf("client_callback: received response\n");
  const example_interfaces__srv__AddTwoInts_Response * resp =
    (const example_interfaces__srv__AddTwoInts_Response *) req_msg;
  cli1_value = resp->sum;
  cli1_id = id->sequence_number;
}

void gc_callback()
{
  gc1_cnt++;
  printf("guard_condition signaled\n");
}

// callback for unit test 'spin_period'
static const unsigned int TC_SPIN_PERIOD_MAX_INVOCATIONS = 100;
static rcutils_duration_value_t _tc_spin_period_timepoints[TC_SPIN_PERIOD_MAX_INVOCATIONS];
static unsigned int _tc_spin_period_invocation_count = 0;  // array index

void spin_period_callback(const void * msgin)
{
  rcutils_time_point_value_t now;
  rcl_ret_t rc;
  RCLC_UNUSED(msgin);

  rc = rcutils_system_time_now(&now);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(spin_period_callback, rcutils_system_time_now);
  }
  if (_tc_spin_period_invocation_count < TC_SPIN_PERIOD_MAX_INVOCATIONS) {
    _tc_spin_period_timepoints[_tc_spin_period_invocation_count] = now;
  } else {
    printf("Error: spin_period_callback: Too many calls to the callback.\n");
  }
  _tc_spin_period_invocation_count++;
}
// returns average time in nanoseconds
uint64_t test_case_evaluate_spin_period()
{
  uint64_t sum;
  sum = 0;
  // i starts from 1 because two timepoints are necessary, e.g. tp[1]- tp[0]
  for (unsigned int i = 1; i < TC_SPIN_PERIOD_MAX_INVOCATIONS; i++) {
    sum += _tc_spin_period_timepoints[i] - _tc_spin_period_timepoints[i - 1];
    // overflow => use micro-seconds (divide by 1000)
  }
  return sum / (TC_SPIN_PERIOD_MAX_INVOCATIONS - 1);
}

// timer callback
void my_timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  // Do timer work...
  // Optionally reconfigure, cancel, or reset the timer...
  if (timer != NULL) {
    // printf("Timer: time since last call %d\n", static_cast<int>(last_call_time));
    _cbt_cnt++;
  }
}

#define CREATE_PUBLISHER(PUB, TOPIC_NAME) \
  this->PUB = rcl_get_zero_initialized_publisher(); \
  this->PUB ## _topic_name = #TOPIC_NAME; \
  this->PUB ## _type_support = \
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32); \
  this->PUB ## _options = rcl_publisher_get_default_options(); \
  ret = rcl_publisher_init( \
    &this->PUB, &this->node, this->PUB ## _type_support, \
    this->PUB ## _topic_name, &this->PUB ## _options); \
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
  rcl_reset_error();

#define CREATE_SUBSCRIPTION(SUB, TOPIC_NAME) \
  this->SUB = rcl_get_zero_initialized_subscription(); \
  this->SUB ## _topic_name = #TOPIC_NAME; \
  this->SUB ## _type_support = \
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32); \
  this->SUB ## _options = rcl_subscription_get_default_options(); \
  ret = rcl_subscription_init( \
    &this->SUB, &this->node, this->SUB ## _type_support, \
    this->SUB ## _topic_name, &this->SUB ## _options); \
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
  EXPECT_TRUE(rcl_subscription_is_valid(&this->SUB)); \
  rcl_reset_error();

void
_wait_for_msg(
  rcl_subscription_t * subscription,
  rcl_context_t * context_ptr,
  size_t max_tries,
  int64_t timeout_ms,
  unsigned int * tries,
  bool * success)
{
  (*tries) = 0;
  (*success) = false;
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret = rcl_wait_set_init(
    &wait_set, 1, 0, 0, 0, 0, 0, context_ptr,
    rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_wait_set_fini(&wait_set);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  do {
    (*tries)++;
    ret = rcl_wait_set_clear(&wait_set);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_wait_set_add_subscription(&wait_set, subscription, NULL);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_wait(&wait_set, RCL_MS_TO_NS(timeout_ms));
    if (ret == RCL_RET_TIMEOUT) {
      continue;
    }
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    for (size_t i = 0; i < wait_set.size_of_subscriptions; ++i) {
      if (wait_set.subscriptions[i] && wait_set.subscriptions[i] == subscription) {
        (*success) = true;
        return;
      }
    }
  } while ( (*tries) < max_tries);
}

class TestDefaultExecutor : public ::testing::Test
{
public:
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_node_t node = rcl_get_zero_initialized_node();

  // integer publisher 1
  rcl_publisher_t pub1 = rcl_get_zero_initialized_publisher();
  const char * pub1_topic_name;
  const rosidl_message_type_support_t * pub1_type_support;
  rcl_publisher_options_t pub1_options = rcl_publisher_get_default_options();
  std_msgs__msg__Int32 pub1_msg;

  // integer publisher 2
  rcl_publisher_t pub2 = rcl_get_zero_initialized_publisher();
  const char * pub2_topic_name;
  const rosidl_message_type_support_t * pub2_type_support;
  rcl_publisher_options_t pub2_options = rcl_publisher_get_default_options();
  std_msgs__msg__Int32 pub2_msg;

  // integer publisher 3
  rcl_publisher_t pub3 = rcl_get_zero_initialized_publisher();
  const char * pub3_topic_name;
  const rosidl_message_type_support_t * pub3_type_support;
  rcl_publisher_options_t pub3_options = rcl_publisher_get_default_options();
  std_msgs__msg__Int32 pub3_msg;

  // integer subscription 1
  rcl_subscription_t sub1 = rcl_get_zero_initialized_subscription();
  const char * sub1_topic_name;
  const rosidl_message_type_support_t * sub1_type_support;
  rcl_subscription_options_t sub1_options;
  std_msgs__msg__Int32 sub1_msg;

  // integer subscription 2
  rcl_subscription_t sub2 = rcl_get_zero_initialized_subscription();
  const char * sub2_topic_name;
  const rosidl_message_type_support_t * sub2_type_support;
  rcl_subscription_options_t sub2_options;
  std_msgs__msg__Int32 sub2_msg;

  // integer subscription 3
  rcl_subscription_t sub3 = rcl_get_zero_initialized_subscription();
  const char * sub3_topic_name;
  const rosidl_message_type_support_t * sub3_type_support;
  rcl_subscription_options_t sub3_options;
  std_msgs__msg__Int32 sub3_msg;

  // timer 1
  rcl_timer_t timer1 = rcl_get_zero_initialized_timer();
  const unsigned int timer1_timeout = 100;
  rcl_clock_t clock;
  rcl_allocator_t clock_allocator;
  const rcl_allocator_t * allocator_ptr;

  void SetUp()
  {
    rcl_ret_t ret;
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    // TODO(jan) hmm das kann weg, oder?
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
    });
    // sollte normales objekt sein
    this->context = rcl_get_zero_initialized_context();
    ret = rcl_init(0, nullptr, &init_options, &this->context);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    // create ROS node
    this->node = rcl_get_zero_initialized_node();
    const char * name = "executor_test";
    rcl_node_options_t node_options = rcl_node_get_default_options();
    ret = rcl_node_init(&this->node, name, "", &this->context, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    const rcl_node_options_t * node_ops = rcl_node_get_options(&this->node);
    this->allocator_ptr = &node_ops->allocator;

    // create publishers - correspond to member variables pub1, pub2, pub3
    // topic name can be configured (should only match with subscription configuration)
    CREATE_PUBLISHER(pub1, data1_int)
    CREATE_PUBLISHER(pub2, data2_int)
    CREATE_PUBLISHER(pub3, data3_int)
    std_msgs__msg__Int32__init(&this->pub1_msg);
    std_msgs__msg__Int32__init(&this->pub2_msg);
    std_msgs__msg__Int32__init(&this->pub3_msg);

    // create subscriptions - correspond to member variables sub1, sub2, sub3
    // topic name of subscriber i (subi) must be the same as publisher i (pubi)
    CREATE_SUBSCRIPTION(sub1, data1_int)
    CREATE_SUBSCRIPTION(sub2, data2_int)
    CREATE_SUBSCRIPTION(sub3, data3_int)
    std_msgs__msg__Int32__init(&this->sub1_msg);
    std_msgs__msg__Int32__init(&this->sub2_msg);
    std_msgs__msg__Int32__init(&this->sub3_msg);

    // create timer with rcl
    this->clock_allocator = rcl_get_default_allocator();
    ret = rcl_clock_init(RCL_STEADY_TIME, &this->clock, &this->clock_allocator);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    this->timer1 = rcl_get_zero_initialized_timer();
    ret =
      rcl_timer_init(
      &this->timer1, &this->clock, &this->context, RCL_MS_TO_NS(
        this->timer1_timeout), my_timer_callback, this->clock_allocator);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown()
  {
    rcl_ret_t ret;
    ret = rcl_subscription_fini(&this->sub1, &this->node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_subscription_fini(&this->sub2, &this->node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_subscription_fini(&this->sub3, &this->node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_timer_fini(&this->timer1);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_clock_fini(&this->clock);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_node_fini(&this->node);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_shutdown(&this->context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(&this->context);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    std_msgs__msg__Int32__fini(&this->pub1_msg);
    std_msgs__msg__Int32__fini(&this->pub2_msg);
    std_msgs__msg__Int32__fini(&pub3_msg);
    std_msgs__msg__Int32__fini(&this->pub1_msg);
    std_msgs__msg__Int32__fini(&this->pub2_msg);
    std_msgs__msg__Int32__fini(&pub3_msg);
  }
};

/*
 * Test suite
 */
TEST_F(TestDefaultExecutor, executor_init) {
  rcl_ret_t rc;

  rclc_executor_t executor;

  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // Executor: NULL executor
  rc = rclc_executor_init(NULL, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // Error case: zero handles
  rc = rclc_executor_init(&executor, &this->context, 0, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
}


TEST_F(TestDefaultExecutor, executor_fini) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // normal case
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // its okay to call rclc_executor_fini twice
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
}

TEST_F(TestDefaultExecutor, executor_add_subscription) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  // test with normal arguemnt and NULL pointers as arguments
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // normal case
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &INT_CALLBACK(1), ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  size_t num_subscriptions = 1;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test NULL pointer for executor
  rc = rclc_executor_add_subscription(
    NULL, &this->sub1, &this->sub1_msg, &CALLBACK_1,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test NULL pointer for subscription
  rc = rclc_executor_add_subscription(
    &executor, NULL, &this->sub1_msg, &CALLBACK_1,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test NULL pointer for message
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, NULL, &CALLBACK_1,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test NULL pointer for callback
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg, NULL,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_add_subscription_with_context) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  // test with normal arguemnt and NULL pointers as arguments
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  int32_t sub_context_value = 0;
  void * sub_context_ptr = reinterpret_cast<void *>( &sub_context_value );

  // normal case
  rc = rclc_executor_add_subscription_with_context(
    &executor, &this->sub1, &this->sub1_msg,
    &int32_callback_with_context, sub_context_ptr, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  size_t num_subscriptions = 1;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test NULL pointer for executor
  rc = rclc_executor_add_subscription_with_context(
    NULL, &this->sub1, &this->sub1_msg, &int32_callback_with_context,
    sub_context_ptr, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test NULL pointer for subscription
  rc = rclc_executor_add_subscription_with_context(
    &executor, NULL, &this->sub1_msg, &int32_callback_with_context,
    sub_context_ptr, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test NULL pointer for message
  rc = rclc_executor_add_subscription_with_context(
    &executor, &this->sub1, NULL, &int32_callback_with_context,
    sub_context_ptr, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test NULL pointer for callback
  rc = rclc_executor_add_subscription_with_context(
    &executor, &this->sub1, &this->sub1_msg, NULL,
    sub_context_ptr, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_add_subscription_too_many) {
  rcl_ret_t rc;
  rclc_executor_t executor;

  // insert one handle, add two subscriptions
  rc = rclc_executor_init(&executor, &this->context, 1, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // test 1: add subscription
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  size_t num_subscriptions = 1;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test 2: add another subscription : failure (array full)
  rc = rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg,
    &CALLBACK_2, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_ERROR, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_remove_subscription) {
  rcl_ret_t rc;
  rclc_executor_t executor;

  // insert one handle, add two subscriptions
  rc = rclc_executor_init(&executor, &this->context, 3, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // add subscription
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  size_t num_subscriptions = 1;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test: remove subscription
  rc = rclc_executor_remove_subscription(&executor, &this->sub1);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 0;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be zero";

  // test: remove non-existant should error
  rc = rclc_executor_remove_subscription(&executor, &this->sub1);
  EXPECT_EQ(RCL_RET_ERROR, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be zero";

  // test: remove from NULL executor should error
  rc = rclc_executor_remove_subscription(NULL, &this->sub1);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  // test: remove null subscription should error
  rc = rclc_executor_remove_subscription(&executor, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // test: add first subscription again
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 1;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be one";

  // test: add second subscription
  rc = rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 2;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be two";

  // test: add third subscription - execution order is 1,2,3
  rc = rclc_executor_add_subscription(
    &executor, &this->sub3, &this->sub3_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 3;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be three";

  // test: the handles are in the expected order
  EXPECT_EQ(executor.handles[0].subscription, &this->sub1) <<
    "expect to find sub1 in first handle";
  EXPECT_EQ(executor.handles[1].subscription, &this->sub2) <<
    "expect to find sub2 in second handle";
  EXPECT_EQ(executor.handles[2].subscription, &this->sub3) <<
    "expect to find sub3 in third handle";

  // test: remove last handle
  rc = rclc_executor_remove_subscription(&executor, &this->sub3);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 2;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be two";
  EXPECT_EQ(executor.handles[0].subscription, &this->sub1) <<
    "expect to find sub1 in first handle";
  EXPECT_EQ(executor.handles[1].subscription, &this->sub2) <<
    "expect to find sub2 in second handle";
  // test: restore last subscription - execution order is still 1,2,3
  rc = rclc_executor_add_subscription(
    &executor, &this->sub3, &this->sub3_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 3;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be three";

  // test: the handles are in the expected order
  EXPECT_EQ(executor.handles[0].subscription, &this->sub1) <<
    "expect to find sub1 in first handle";
  EXPECT_EQ(executor.handles[1].subscription, &this->sub2) <<
    "expect to find sub2 in second handle";
  EXPECT_EQ(executor.handles[2].subscription, &this->sub3) <<
    "expect to find sub3 in third handle";

  // test: remove middle handle
  rc = rclc_executor_remove_subscription(&executor, &this->sub2);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 2;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be two";
  EXPECT_EQ(executor.handles[0].subscription, &this->sub1) <<
    "expect to find sub1 in first handle";
  EXPECT_EQ(executor.handles[1].subscription, &this->sub3) <<
    "expect to find sub3 in second handle";
  // test: restore (push) sub2 subscription - this changes the order of excution to 1,3,2
  rc = rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 3;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be three";

  // test: the handles are in the expected order
  EXPECT_EQ(executor.handles[0].subscription, &this->sub1) <<
    "expect to find sub1 in first handle";
  EXPECT_EQ(executor.handles[1].subscription, &this->sub3) <<
    "expect to find sub3 in second handle";
  EXPECT_EQ(executor.handles[2].subscription, &this->sub2) <<
    "expect to find sub2 in third handle";

  // test: remove first handle
  rc = rclc_executor_remove_subscription(&executor, &this->sub1);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 2;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be two";
  EXPECT_EQ(executor.handles[0].subscription, &this->sub3) <<
    "expect to find sub3 in first handle";
  EXPECT_EQ(executor.handles[1].subscription, &this->sub2) <<
    "expect to find sub2 in second handle";
  // test: restore (push) sub1 subscription - this changes the order of excution to 3,2,1
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  num_subscriptions = 3;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be three";

  // test: the handles are in the expected order
  EXPECT_EQ(executor.handles[0].subscription, &this->sub3) <<
    "expect to find sub3 in first handle";
  EXPECT_EQ(executor.handles[1].subscription, &this->sub2) <<
    "expect to find sub2 in second handle";
  EXPECT_EQ(executor.handles[2].subscription, &this->sub1) <<
    "expect to find sub1 in third handle";


  // tear down
  // remove all subscriptions
  rc = rclc_executor_remove_subscription(&executor, &this->sub1);
  rc = rclc_executor_remove_subscription(&executor, &this->sub2);
  rc = rclc_executor_remove_subscription(&executor, &this->sub3);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  num_subscriptions = 0;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions is expected to be zero";

  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_add_timer) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  size_t exp_number_of_timers = 0;
  EXPECT_EQ(executor.info.number_of_timers, exp_number_of_timers) << "#times should be 0";
  rc = rclc_executor_add_timer(&executor, &this->timer1);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  exp_number_of_timers = 1;
  EXPECT_EQ(executor.info.number_of_timers, exp_number_of_timers) << "#timers should be 1";

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_spin_timer) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // spin_timeout must be < timer1_timeout
  const unsigned int spin_timeout = 50;
  const unsigned int spin_repeat = 10;
  const unsigned int expected_callbacks = (spin_timeout * spin_repeat) / timer1_timeout;
  _cbt_cnt = 0;

  rc = rclc_executor_add_timer(&executor, &this->timer1);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  for (size_t i = 0; i < spin_repeat; i++) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(spin_timeout));
  }

  EXPECT_EQ(_cbt_cnt, expected_callbacks);

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_spin_publisher_timer_cancelled) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  unsigned int expected_msg;

  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  _executor_results_init();

  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);

  rc = rclc_executor_add_timer(&executor, &this->timer1);

  for (unsigned int i = 0; i < TC_SPIN_SOME_PUBLISHED_MSGS; i++) {
    rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
    EXPECT_EQ(RCL_RET_OK, rc) << " pub1 not published";
  }

  // wait until messages are received
  bool success = false;
  unsigned int tries;
  unsigned int max_tries = 100;
  uint64_t timeout_ns = 100000000;  // 100ms

  rc = rcl_timer_cancel(&this->timer1);
  // process subscriptions. Assumption: messages for sub1 available
  for (unsigned int i = 0; i < 100; i++) {
    // Assumption: messages for all sub1, sub2 and sub3 are available
    _wait_for_msg(
      &this->sub1, &this->context, max_tries, timeout_ns, &tries,
      &success);
    ASSERT_TRUE(success);


    EXPECT_EQ(RCL_RET_OK, rc) << "failed to cancel timer";

    rc = rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
    if ((rc == RCL_RET_OK) || (rc == RCL_RET_TIMEOUT)) {
      // valid return values
    } else {
      // any other error
      EXPECT_EQ(RCL_RET_OK, rc) << "spin_some error";
    }
    if (_cb1_cnt == TC_SPIN_SOME_PUBLISHED_MSGS) {
      break;
    }
  }

  expected_msg = TC_SPIN_SOME_PUBLISHED_MSGS;
  EXPECT_EQ(_cb1_cnt, expected_msg) << "cb1 msg does not match";

  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
}

TEST_F(TestDefaultExecutor, executor_spin_timer_cancelled) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // spin_timeout must be < timer1_timeout
  const unsigned int spin_timeout = 50;
  const unsigned int spin_repeat = 10;
  const unsigned int expected_callbacks = (spin_timeout * spin_repeat) / timer1_timeout;
  _cbt_cnt = 0;

  rc = rclc_executor_add_timer(&executor, &this->timer1);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  for (size_t i = 0; i < spin_repeat; i++) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(spin_timeout));
    if (i > spin_repeat / 2) {
      rc = rcl_timer_cancel(&this->timer1);
      EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
    }
  }

  EXPECT_LT(_cbt_cnt, expected_callbacks);

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_remove_timer) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // setup with one timer
  size_t exp_number_of_timers = 0;
  EXPECT_EQ(executor.info.number_of_timers, exp_number_of_timers) << "#times should be 0";
  rc = rclc_executor_add_timer(&executor, &this->timer1);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  exp_number_of_timers = 1;
  EXPECT_EQ(executor.info.number_of_timers, exp_number_of_timers) << "#timers should be 1";

  // test removing from NULL executor
  rc = rclc_executor_remove_timer(NULL, &this->timer1);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  // test removing NULL timer
  rc = rclc_executor_remove_timer(&executor, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  exp_number_of_timers = 1;
  EXPECT_EQ(executor.info.number_of_timers, exp_number_of_timers) << "#timers should be 1";

  // test removing timer
  rc = rclc_executor_remove_timer(&executor, &this->timer1);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  exp_number_of_timers = 0;
  EXPECT_EQ(executor.info.number_of_timers, exp_number_of_timers) << "#timers should be 0";

  // test removing non-existent timer
  rc = rclc_executor_remove_timer(&executor, &this->timer1);
  EXPECT_EQ(RCL_RET_ERROR, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_add_client) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  const char * client_name = "/addtwoints";
  rcl_client_options_t client_options = rcl_client_get_default_options();
  rcl_client_t client = rcl_get_zero_initialized_client();
  const rosidl_service_type_support_t * client_type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
  rc = rcl_client_init(&client, &this->node, client_type_support, client_name, &client_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  example_interfaces__srv__AddTwoInts_Response res;
  example_interfaces__srv__AddTwoInts_Response__init(&res);

  size_t number_of_clients = 0;
  EXPECT_EQ(executor.info.number_of_clients, number_of_clients) << "should be 0";
  EXPECT_EQ(executor.info.number_of_services, (size_t) 0) << "should be 0 ";

  rc = rclc_executor_add_client(&executor, &client, &res, &client_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  number_of_clients = 1;
  EXPECT_EQ(executor.info.number_of_clients, number_of_clients) << " should be 1";
  EXPECT_EQ(executor.info.number_of_services, (size_t) 0) << "should be 0 ";

  // failure test cases
  rc = rclc_executor_add_client(NULL, &client, &res, &client_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_add_client(&executor, NULL, &res, &client_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_add_client(&executor, &client, NULL, &client_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_add_client(&executor, &client, &res, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // tear down
  rc = rcl_client_fini(&client, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_remove_client) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  const char * client_name = "/addtwoints";
  rcl_client_options_t client_options = rcl_client_get_default_options();
  rcl_client_t client = rcl_get_zero_initialized_client();
  const rosidl_service_type_support_t * client_type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
  rc = rcl_client_init(&client, &this->node, client_type_support, client_name, &client_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  example_interfaces__srv__AddTwoInts_Response res;
  example_interfaces__srv__AddTwoInts_Response__init(&res);

  size_t number_of_clients = 0;
  rc = rclc_executor_add_client(&executor, &client, &res, &client_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  number_of_clients = 1;
  EXPECT_EQ(executor.info.number_of_clients, number_of_clients) << " should be 1";
  EXPECT_EQ(executor.info.number_of_services, (size_t) 0) << "should be 0 ";

  // test removing from NULL executor
  rc = rclc_executor_remove_client(NULL, &client);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // test removing NULL client
  rc = rclc_executor_remove_client(&executor, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // test removing actual client
  rc = rclc_executor_remove_client(&executor, &client);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  number_of_clients = 0;
  EXPECT_EQ(executor.info.number_of_clients, number_of_clients) << " should be 0";
  EXPECT_EQ(executor.info.number_of_services, (size_t) 0) << "should be 0 ";

  // test removing non-existent client
  rc = rclc_executor_remove_client(&executor, &client);
  EXPECT_EQ(RCL_RET_ERROR, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // tear down
  rc = rcl_client_fini(&client, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_add_service) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;


  const char * service_name = "/addtwoints";
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_service_t service = rcl_get_zero_initialized_service();
  const rosidl_service_type_support_t * service_type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
  rc =
    rcl_service_init(&service, &this->node, service_type_support, service_name, &service_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  example_interfaces__srv__AddTwoInts_Request req;
  example_interfaces__srv__AddTwoInts_Request__init(&req);

  example_interfaces__srv__AddTwoInts_Response resp;
  example_interfaces__srv__AddTwoInts_Response__init(&resp);

  size_t number_of_services = 0;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, number_of_services) << "should be 0";

  rc = rclc_executor_add_service(&executor, &service, &req, &resp, &service_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 1);

  // failure test cases
  rc = rclc_executor_add_service(NULL, &service, &req, &resp, &service_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_add_service(&executor, NULL, &req, &resp, &service_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_add_service(&executor, &service, NULL, &resp, &service_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_add_service(&executor, &service, &req, NULL, &service_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_add_service(&executor, &service, &req, &resp, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // tear down
  rc = rcl_service_fini(&service, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_remove_service) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;


  const char * service_name = "/addtwoints";
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_service_t service = rcl_get_zero_initialized_service();
  const rosidl_service_type_support_t * service_type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
  rc =
    rcl_service_init(&service, &this->node, service_type_support, service_name, &service_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  example_interfaces__srv__AddTwoInts_Request req;
  example_interfaces__srv__AddTwoInts_Request__init(&req);

  example_interfaces__srv__AddTwoInts_Response resp;
  example_interfaces__srv__AddTwoInts_Response__init(&resp);

  size_t number_of_services = 0;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, number_of_services) << "should be 0";

  rc = rclc_executor_add_service(&executor, &service, &req, &resp, &service_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  number_of_services = 1;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, number_of_services) << "should be 1";

  // test removing from NULL executor
  rc = rclc_executor_remove_service(NULL, &service);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // test removing NULL service
  rc = rclc_executor_remove_service(&executor, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // test remove service
  rc = rclc_executor_remove_service(&executor, &service);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  number_of_services = 0;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, number_of_services) << "should be 0";

  // test remove non-existent service
  rc = rclc_executor_remove_service(&executor, &service);
  EXPECT_EQ(RCL_RET_ERROR, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // tear down
  rc = rcl_service_fini(&service, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_spin_some_API) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  rc = rclc_executor_add_timer(&executor, &this->timer1);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  rc = rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, pub_sub_example) {
  // bare metal example for publish and subscribe with rcl API.
  // this test case calls all rcl methods that are also called
  // by the Executor:
  // - rcl_wait_set_init initialize wait set
  // - rcl_wait to get notified about new message
  // - rcl_take to take it from DDS
  // - call the corresponding callback with the message
  rcl_ret_t ret;
  this->pub1_msg.data = 42;
  ret = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  bool success = false;
  unsigned int tries;
  unsigned int max_tries = 100;
  uint64_t timeout_ns = 100000000;  // 100ms
  _wait_for_msg(
    &this->sub1, &this->context, max_tries, timeout_ns, &tries,
    &success);
  // printf("Number of tries to access DDS-queue: %u\n", tries);
  ASSERT_TRUE(success);

  ret = rcl_take(&this->sub1, &this->sub1_msg, nullptr, nullptr);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_EQ(42, this->sub1_msg.data);

  // initialize logging data
  _results_callback_init();

  CALLBACK_1(&this->sub1_msg);
  ASSERT_EQ(_cb1_cnt, (unsigned int) 1) << "expect 1";
  ASSERT_EQ(_cb1_int_value, (unsigned int) 42) << "expect 42";
}

TEST_F(TestDefaultExecutor, spin_some_sequential_execution) {
  // 27.06.2019, adopted from ros2/rcl/rcl/test/rcl/test_subscriptions.cpp
  // by Jan Staschulat, under Apache 2.0 License
  rcl_ret_t ret;
  rclc_executor_t executor;
  // initialize executor for 3 subscriptions in the order sub1, sub2, sub3
  size_t num_subscriptions = 3;
  ret = rclc_executor_init(&executor, &this->context, num_subscriptions, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  unsigned int expected_msg;
  _executor_results_init();
  ret =
    rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg, &CALLBACK_1,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();
  ret =
    rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg, &CALLBACK_2,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();
  ret =
    rclc_executor_add_subscription(
    &executor, &this->sub3, &this->sub3_msg, &CALLBACK_3,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "expected number of subscriptions: 3";
  // these particular values are used to check the LET-semantics
  this->pub1_msg.data = 1;
  this->pub2_msg.data = 2;
  this->pub3_msg.data = 3;
  ///////////////////////////////////////////////////////////////////////////////////
  /////////// test case 1 : sent in increasing order (1 2 3)
  ///////////////////////////////////////////////////////////////////////////////////
  for (unsigned int i = 0; i < TC_SPIN_SOME_PUBLISHED_MSGS; i++) {
    ret = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
    EXPECT_EQ(RCL_RET_OK, ret) << " pub1 not published";

    ret = rcl_publish(&this->pub2, &this->pub2_msg, nullptr);
    EXPECT_EQ(RCL_RET_OK, ret) << " pub2 not published";

    ret = rcl_publish(&this->pub3, &this->pub3_msg, nullptr);
    EXPECT_EQ(RCL_RET_OK, ret) << " pub3 not published";
  }
  // wait until messages are received
  bool success = false;
  unsigned int tries;
  unsigned int max_tries = 100;
  uint64_t timeout_ns = 100000000;   // 100ms
  // process subscriptions
  for (unsigned int i = 0; i < 100; i++) {
    // Assumption: messages for all sub1, sub2 and sub3 are available
    _wait_for_msg(&this->sub1, &this->context, max_tries, timeout_ns, &tries, &success);
    ASSERT_TRUE(success);
    _wait_for_msg(&this->sub2, &this->context, max_tries, timeout_ns, &tries, &success);
    ASSERT_TRUE(success);
    _wait_for_msg(&this->sub3, &this->context, max_tries, timeout_ns, &tries, &success);
    ASSERT_TRUE(success);

    ret = rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
    if ((ret == RCL_RET_OK) || (ret == RCL_RET_TIMEOUT)) {
      // valid return values
    } else {
      // any other error
      EXPECT_EQ(RCL_RET_OK, ret) << "spin_some error";
    }
    if (_executor_results_all_msg_received()) {
      break;
    }
  }
  // check total number of received messages
  expected_msg = TC_SPIN_SOME_PUBLISHED_MSGS;
  EXPECT_EQ(_cb1_cnt, expected_msg) << "cb1 msg does not match";
  EXPECT_EQ(_cb2_cnt, expected_msg) << "cb2 msg does not match";
  EXPECT_EQ(_cb3_cnt, expected_msg) << "cb3 msg does not match";
  // test order of received handles
  // assumption: expecting TC_SPIN_SOME_PUBLISHED_MSGS==3 => 9 messages
  // _executor_results_print();
  unsigned int result_tc1[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};
  EXPECT_EQ(_executor_results_compare(result_tc1), true) << "expect [1,2,3, 1,2,3, 1,2,3]";
  ///////////////////////////////////////////////////////////////////////////////////
  /////////// test case 2 : publish in reverse order (3,2,1)
  ///////////////////////////////////////////////////////////////////////////////////
  _executor_results_init();
  // now sent in different order
  for (unsigned int i = 0; i < TC_SPIN_SOME_PUBLISHED_MSGS; i++) {
    ret = rcl_publish(&this->pub3, &this->pub3_msg, nullptr);
    EXPECT_EQ(RCL_RET_OK, ret) << " pub3 not published";

    ret = rcl_publish(&this->pub2, &this->pub2_msg, nullptr);
    EXPECT_EQ(RCL_RET_OK, ret) << " pub2 not published";

    ret = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
    EXPECT_EQ(RCL_RET_OK, ret) << " pub1 not published";
  }

  // process subscriptions. Assumption: messages for all sub1, sub2 and sub3 are available
  for (unsigned int i = 0; i < 100; i++) {
    // wait until messages are received
    _wait_for_msg(&this->sub1, &this->context, max_tries, timeout_ns, &tries, &success);
    ASSERT_TRUE(success);
    _wait_for_msg(&this->sub2, &this->context, max_tries, timeout_ns, &tries, &success);
    ASSERT_TRUE(success);
    _wait_for_msg(&this->sub3, &this->context, max_tries, timeout_ns, &tries, &success);
    ASSERT_TRUE(success);
    ret = rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
    if ((ret == RCL_RET_OK) || (ret == RCL_RET_TIMEOUT)) {
      // valid return values
    } else {
      // any other error
      EXPECT_EQ(RCL_RET_OK, ret) << "spin_some error";
    }
    if (_executor_results_all_msg_received()) {
      break;
    }
  }
  // check total number of received messages
  expected_msg = TC_SPIN_SOME_PUBLISHED_MSGS;
  EXPECT_EQ(_cb1_cnt, expected_msg) << "cb1 msg does not match";
  EXPECT_EQ(_cb2_cnt, expected_msg) << "cb2 msg does not match";
  EXPECT_EQ(_cb3_cnt, expected_msg) << "cb3 msg does not match";
  // test order of received handles
  // because processing order is 1,2,3 and even that the messages are published
  // in the order 3,2,1 we expect the order 1,2,3.
  unsigned int result_tc2[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};
  EXPECT_EQ(_executor_results_compare(result_tc2), true) << "expect: [1,2,3, 1,2,3, 1,2,3]";
  // _executor_results_print();
  ret = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();
}

TEST_F(TestDefaultExecutor, invocation_type) {
  // 27.06.2019, adopted from ros2/rcl/rcl/test/rcl/test_subscriptions.cpp
  // by Jan Staschulat, under Apache 2.0 License

  // test for invocation type ALWAYS ON_NEW_DATA
  //
  // publisher A
  // send 1 message
  // subscriber A' with invocation=ALWAYS

  // publisher B
  // send 1 message
  // subscriber B' with invocation=ON_NEW_DATA

  // executor setup()
  // executor_spin_some()
  // executor_spin_some()

  // expected result
  // number of invocations of callback A' = 2
  // number of invocations of callback B' = 1

// 27.06.2019, adopted from ros2/rcl/rcl/test/rcl/test_subscriptions.cpp
  // by Jan Staschulat, under Apache 2.0 License
  rcl_ret_t ret;
  rclc_executor_t executor;

  // initialize result variables
  _executor_results_init();
  // initialize executor with 2 handles
  ret = rclc_executor_init(&executor, &this->context, 2, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ret = rclc_executor_set_trigger(&executor, rclc_executor_trigger_always, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // add subscription to the executor
  ret =
    rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg, &CALLBACK_1,
    ALWAYS);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();

  ret =
    rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg, &CALLBACK_2,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();

  // check, if all subscriptions were added
  size_t num_subscriptions = 2;
  EXPECT_EQ(executor.info.number_of_subscriptions, num_subscriptions) <<
    "number of subscriptions should be = 2";

  ///////////////////////////////////////////////////////////////////////////////////
  /////////// test case 1 : publish one data for each publisher
  ///////////////////////////////////////////////////////////////////////////////////
  this->pub1_msg.data = 1;
  this->pub2_msg.data = 2;
  ret = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, ret) << " this->pub1 did not publish!";
  ret = rcl_publish(&this->pub2, &this->pub2_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, ret) << " this->pub2 did not publish!";


  // wait until messages are received
  bool success = false;
  unsigned int tries;
  unsigned int max_tries = 100;
  uint64_t timeout_ns = 100000000;  // 100ms
  _wait_for_msg(
    &this->sub1, &this->context, max_tries, timeout_ns, &tries,
    &success);
  ASSERT_TRUE(success);
  _wait_for_msg(
    &this->sub2, &this->context, max_tries, timeout_ns, &tries,
    &success);
  ASSERT_TRUE(success);

  // initialize result variables
  _cb1_cnt = 0;
  _cb2_cnt = 0;

  // running the executor
  // std::this_thread::sleep_for(rclc_test_sleep_time);

  ret = rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  if ((ret == RCL_RET_OK) || (ret == RCL_RET_TIMEOUT)) {
    // valid return values
  } else {
    // any other error
    EXPECT_EQ(RCL_RET_OK, ret) << "spin_some error";
  }
  // check total number of received messages
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1) << "cb1 msg does not match";
  EXPECT_EQ(_cb2_cnt, (unsigned int) 1) << "cb2 msg does not match";

  uint64_t reduced_timeout_ns = 1000000;  // 1ms
  ret = rclc_executor_spin_some(&executor, reduced_timeout_ns);
  if ((ret == RCL_RET_OK) || (ret == RCL_RET_TIMEOUT)) {
    // valid return values
  } else {
    // any other error
    EXPECT_EQ(RCL_RET_OK, ret) << "spin_some error";
  }
  // check total number of received messages
  EXPECT_EQ(_cb1_cnt, (unsigned int) 2) << "cb1 msg does not match";
  EXPECT_EQ(_cb2_cnt, (unsigned int) 1) << "cb2 msg does not match";

  // tear down
  ret = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, update_wait_set) {
  // test that subscription can be added after spin_some
  // add_subscription()
  // publish pub1, publish pub1, publish pub2
  // spin_some()        => sub1 called
  // add_subscription() => check that bookkeepin inside Executor is correct
  // spin_some()        => sub1 sub2 called
  rcl_ret_t ret;
  rclc_executor_t executor;
  // initialize executor with 2 handles
  ret = rclc_executor_init(&executor, &this->context, 2, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  _results_callback_init();

  // initially the wait_set is zero_initialized
  EXPECT_EQ(false, rcl_wait_set_is_valid(&executor.wait_set));
  // add subscription to the executor
  ret =
    rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg, &CALLBACK_1,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();

  // wait_set is not valid
  EXPECT_EQ(false, rcl_wait_set_is_valid(&executor.wait_set));

  // initialize pub messages
  this->pub1_msg.data = 1;
  this->pub2_msg.data = 2;
  ///////////////////////////////////////////////////////////////////////////////////
  /////////// test case 1 : spin_once (receive one message)
  ///////////////////////////////////////////////////////////////////////////////////
  EXPECT_EQ((unsigned int)0, _cb1_cnt);
  EXPECT_EQ((unsigned int)0, _cb2_cnt);

  ret = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, ret) << " publisher1 did not publish!";
  ret = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, ret) << " publisher1 did not publish!";
  ret = rcl_publish(&this->pub2, &this->pub2_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, ret) << " publisher1 did not publish!";

/*
  // wait until messages are received
  bool success = false;
  unsigned int tries;
  unsigned int max_tries = 100;
  uint64_t timeout_ns = 100000000;   // 100ms
  _wait_for_msg(
    &this->sub1, &this->context, max_tries, timeout_ns, &tries,
    &success);
  ASSERT_TRUE(success);
  _wait_for_msg(
    &this->sub2, &this->context, max_tries, timeout_ns, &tries,
    &success);
  ASSERT_TRUE(success);
*/
  std::this_thread::sleep_for(rclc_test_sleep_time);
  ret = rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  if ((ret == RCL_RET_OK) || (ret == RCL_RET_TIMEOUT)) {
    // valid return values
  } else {
    // any other error
    EXPECT_EQ(RCL_RET_OK, ret) << "spin_some error";
  }

  // wait_set is valid
  EXPECT_EQ(true, rcl_wait_set_is_valid(&executor.wait_set));

  // received one message
  EXPECT_EQ((unsigned int)1, _cb1_cnt);
  EXPECT_EQ((unsigned int)1, _cb1_int_value);
  EXPECT_EQ((unsigned int)0, _cb2_cnt);
  ///////////////////////////////////////////////////////////////////////////////////
  /////////// test case 2 : add another subscription
  ///////////////////////////////////////////////////////////////////////////////////
  // wait_set is valid
  EXPECT_EQ(true, rcl_wait_set_is_valid(&executor.wait_set));
  ret =
    rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg, &CALLBACK_2,
    ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcutils_reset_error();

  // wait_set is not valid
  EXPECT_EQ(false, rcl_wait_set_is_valid(&executor.wait_set));
  ///////////////////////////////////////////////////////////////////////////////////
  /////////// test case 3 : spin_some again
  ///////////////////////////////////////////////////////////////////////////////////
  _cb1_int_value = 0;
  _cb2_int_value = 0;
  ret = rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  if ((ret == RCL_RET_OK) || (ret == RCL_RET_TIMEOUT)) {
    // valid return values
  } else {
    // any other error
    EXPECT_EQ(RCL_RET_OK, ret) << "spin_some error";
  }

  // wait_set is valid
  EXPECT_EQ(true, rcl_wait_set_is_valid(&executor.wait_set));

  // callback 1 and callback 2 were executed
  // check call frequency and value of received message
  EXPECT_EQ((unsigned int)2, _cb1_cnt);
  EXPECT_EQ((unsigned int)1, _cb2_cnt);
  EXPECT_EQ((unsigned int)1, _cb1_int_value);
  EXPECT_EQ((unsigned int)2, _cb2_int_value);

  // tear down
  ret = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}


TEST_F(TestDefaultExecutor, spin_period) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  // initialize executor with 1 handle
  rc = rclc_executor_init(&executor, &this->context, 1, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // set timeout to zero - so that rcl_wait() comes back immediately
  rc = rclc_executor_set_timeout(&executor, 0);

  // add dummy subscription which is always executed
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &spin_period_callback, ALWAYS);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rclc_executor_set_trigger(&executor, rclc_executor_trigger_always, NULL);

  // measure the timepoint, when spin_period_callback() is called
  uint64_t spin_period = 20000000;  // 20 ms
  for (unsigned int i = 0; i < TC_SPIN_PERIOD_MAX_INVOCATIONS; i++) {
    rclc_executor_spin_one_period(&executor, spin_period);
  }
  // compute avarage time duration between calls to spin_period_callback
  uint64_t duration = test_case_evaluate_spin_period();
  printf("expected  'spin_period' : %lu ns\n", spin_period);
  printf("actual (%d iterations) : %lu ns\n", TC_SPIN_PERIOD_MAX_INVOCATIONS, duration);

  uint64_t delta = 5000000;  // 5 ms interval
  EXPECT_LE(duration, spin_period + delta);
  EXPECT_LE(spin_period - delta, duration);

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

/*
TEST_F(TestDefaultExecutor, semantics_RCLCPP) {
  rcl_ret_t rc;
  rclc_executor_t executor;

  // configuration
  // - one publisher
  //    - publishes integer topic X
  // - subscriber A
  //    - subscribes to X
  //    - publishes also on topic X
  // - subscriber B
  //     - subscribes to X with DDS quality of service option: last-is-best
  //     - evaluates data of topic X and copies it to global variable result_X
  // - evaluation in test case, what the value of result_X is.
  //
  // test setup
  //  - publisher: publishes X=1
  //  - sleep(100ms)
  //  - spin_some()
  //  - subscriber A : publishes X=2
  //  - sleep(100ms)
  //  - subscriber B :
  // expected test result for semantics RCLCPP:
  //   => subscriber B receives X=2 (because it takes most rececent data)
  // expected test result for semantics LET:
  //   => subscriber B receives X=1 (because data is taken from DDS at the start of spin_some())

  // implementation
  // use member variable this->pub1 as publisher
  // use member variable this->sub1 as subscription 1
  // create subscription 2 with last-is-best semantics
  rcl_subscription_t subscription2 = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t subscription_options2 = rcl_subscription_get_default_options();
  std_msgs__msg__Int32 subscription2_int_msg;
  subscription_options2.qos.depth = 0;  // qos: last is best
  rc = rcl_subscription_init(
    &subscription2, &this->node, this->pub1_type_support,
    this->pub1_topic_name, &subscription_options2);
  ASSERT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_subscription_is_valid(&subscription2));
  rcl_reset_error();

  // initialize executor with 2 handles
  rc = rclc_executor_init(&executor, &this->context, 2, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcl_reset_error();

  // add subscriptions to executor
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &int32_callback4, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  subscription2_int_msg.data = 77;

  rc = rclc_executor_add_subscription(
    &executor, &subscription2, &subscription2_int_msg,
    &int32_callback5, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // create global pointer to this publisher to access it from callback 'int32_callback4'
  _pub_int_ptr = &this->pub1;
  _pub_int_msg_ptr = &this->pub1_msg;
  // ------------------------- test case setup ------------------------
  rclc_executor_set_semantics(&executor, RCLCPP_EXECUTOR);
  this->pub1_msg.data = 1;
  _cb5_int_value = 0;  // received value in subscription2
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1(tc) did not publish!";

  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  // test result
  EXPECT_EQ(_cb5_int_value, 2) <<
    " expect value 2: Value from rcl_publish in int32_callback4 should have be received.";

  // clean-up
  rc = rcl_subscription_fini(&subscription2, &this->node);
  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}
*/
/*
TEST_F(TestDefaultExecutor, semantics_LET) {
  rcl_ret_t rc;
  rclc_executor_t executor;

  // copy-ans-paste  from semantics_RCLCPP except:
  // - semantics = LET
  // - expected result = 1 (original value of publisher)

  // configuration
  // - one publisher
  //    - publishes integer topic X
  // - subscriber A
  //    - subscribes to X
  //    - publishes also on topic X
  // - subscriber B
  //     - subscribes to X with DDS quality of service option: last-is-best
  //     - evaluates data of topic X and copies it to global variable result_X
  // - evaluation in test case, what the value of result_X is.
  //
  // test setup
  //  - publisher: publishes X=1
  //  - sleep(100ms)
  //  - spin_some()
  //  - subscriber A : publishes X=2
  //  - sleep(100ms)
  //  - subscriber B :
  // expected test result for semantics RCLCPP:
  //   => subscriber B receives X=2 (because it takes most rececent data)
  // expected test result for semantics LET:
  //   => subscriber B receives X=1 (because data is taken from DDS at the start of spin_some() )

  // implementation
  // use member variable this->pub1 as publisher
  // use member variable this->sub1 as subscription 1
  // create subscription 2 with last-is-best semantics
  rcl_subscription_t subscription2 = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t subscription_options2 = rcl_subscription_get_default_options();
  std_msgs__msg__Int32 subscription2_int_msg;
  subscription_options2.qos.depth = 0;  // qos: last is best
  rc = rcl_subscription_init(
    &subscription2, &this->node, this->pub1_type_support,
    this->pub1_topic_name, &subscription_options2);
  ASSERT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_subscription_is_valid(&subscription2));
  rcl_reset_error();

  // initialize executor with 2 handles
  rc = rclc_executor_init(&executor, &this->context, 2, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcl_reset_error();

  // add subscriptions to executor
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &int32_callback4, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  rc = rclc_executor_add_subscription(
    &executor, &subscription2, &subscription2_int_msg,
    &int32_callback5, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // create global pointer to this publisher to access it from callback 'int32_callback4'
  _pub_int_ptr = &this->pub1;
  _pub_int_msg_ptr = &this->pub1_msg;

  // ------------------------- test case setup ------------------------
  rclc_executor_set_semantics(&executor, LET);
  this->pub1_msg.data = 1;
  _cb5_int_value = 0;  // received value in subscription2
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1(tc) did not publish!";

  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  // test result
  EXPECT_EQ(
    _cb5_int_value,
    1) <<
    " expect value 1: first value of 'pub1' publisher should have been received.";

  // clean-up
  rc = rcl_subscription_fini(&subscription2, &this->node);
  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}
*/
/*
TEST_F(TestDefaultExecutor, trigger_one) {
  // test specification
  // multiple subscriptions
  //  - two publishers for topic a and topic b
  //  - two subscriptions A, B
  //     - subscriber A subscribes to topic a
  //     - subscriber B subscribes to topic b
  //     - subscriber B sets result variable.
  // set trigger: trigger_one(A) => execute when A receives new data
  // setup:
  // - publish topic A
  // - spin_some()
  // - expected result:
  //   - subscriber A called
  //   - subscriber B not called
  // - publish topic B
  // - spin_some()
  // - expected result:
  //   - subscriber A not called
  //   - subscriber B not called
  // - publish topic A
  // - spin_some()
  // - expected result:
  //   - subscriber A called
  //   - subscriber B called
  // -------------- rcl objects ---------------------------------------------------------

  rcl_ret_t rc;
  rclc_executor_t executor;
  // initialize executor with 2 handles
  rc = rclc_executor_init(&executor, &this->context, 2, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcl_reset_error();
  rc = rclc_executor_set_trigger(&executor, rclc_executor_trigger_one, &this->sub1);

  // add subscriptions to executor
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  rc = rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg,
    &CALLBACK_2, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  // ------------------------- test case setup ---------------------------------------------

  const std::chrono::milliseconds ci_job_time =
    std::chrono::milliseconds(1000);

  // first round
  _results_callback_init();
  this->pub1_msg.data = 3;
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1 did not publish!";
  std::this_thread::sleep_for(ci_job_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 3) << " expected: A called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 0) << " expected: B not called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 0);
  // second round
  this->pub2_msg.data = 787;
  std::this_thread::sleep_for(ci_job_time);
  rc = rcl_publish(&this->pub2, &this->pub2_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub2 did not publish!";
  std::this_thread::sleep_for(ci_job_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 3) << " expected: A not called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 0) << " expected: B not called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 0);

  // third round
  this->pub1_msg.data = 11;
  std::this_thread::sleep_for(ci_job_time);
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time + ci_job_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 11) << " expected: A called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 787) << " expected: B called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 2);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 1);

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, trigger_any) {
  // test specification
  // set trigger: trigger_any => execute when A or B received new data
  // setup:
  // - publish topic A
  // - spin_some()
  // - expected result:
  //   - subscriber A called
  //   - subscriber B not called
  // - publish topic B
  // - spin_some()
  // - expected result:
  //   - subscriber A not called
  //   - subscriber B called
  // - publish topic A
  // - spin_some()
  // - expected result:
  //   - subscriber A called
  //   - subscriber B not called

  // -------------- rcl objects ---------------------------------------------------------
  rcl_ret_t rc;
  rclc_executor_t executor;
  // initialize executor with 2 handles
  rc = rclc_executor_init(&executor, &this->context, 2, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcl_reset_error();
  rc = rclc_executor_set_trigger(&executor, rclc_executor_trigger_any, NULL);

  // add subscriptions to executor
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  rc = rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg,
    &CALLBACK_2, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  // ------------------------- test case setup --------------------------------------------
  // first round
  _results_callback_init();
  _executor_results_init();
  this->pub1_msg.data = 3;
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 3) << " expected: A called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 0) << " expected: B not called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 0);
  // second round
  this->pub2_msg.data = 7;
  rc = rcl_publish(&this->pub2, &this->pub2_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub2 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 3) << " expected: A not called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 7) << " expected: B called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 1);

  // third round
  this->pub1_msg.data = 11;
  _cb1_int_value = 0;
  _cb2_int_value = 0;
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 11) << " expected: A called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 0) << " expected: B not called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 2);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 1);

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, trigger_all) {
  // test specification
  // set trigger: trigger_all => execute when A and B received new data
  // setup:
  // - publish topic A
  // - spin_some()
  // - expected result:
  //   - subscriber A not called
  //   - subscriber B not called
  // - publish topic B
  // - spin_some()
  // - expected result:
  //   - subscriber A called
  //   - subscriber B called
  // - publish topic A
  // - spin_some()
  // - expected result:
  //   - subscriber A not called
  //   - subscriber B not called

  // -------------- rcl objects ---------------------------------------------------------
  rcl_ret_t rc;
  rclc_executor_t executor;
  // initialize executor with 2 handles
  rc = rclc_executor_init(&executor, &this->context, 2, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcl_reset_error();
  rc = rclc_executor_set_trigger(&executor, rclc_executor_trigger_all, NULL);

  // add subscriptions to executor
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  rc = rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg,
    &CALLBACK_2, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  // ------------------------- test case setup --------------------------------------------
  // first round
  _results_callback_init();
  _executor_results_init();
  this->pub1_msg.data = 3;
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 0) << " expected: A not called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 0) << " expected: B not called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 0);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 0);
  // second round
  this->pub2_msg.data = 76;
  rc = rcl_publish(&this->pub2, &this->pub2_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub2 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 3) << " expected: A called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 76) << " expected: B called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 1);

  // third round
  this->pub1_msg.data = 11;
  _cb1_int_value = 0;
  _cb2_int_value = 0;
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 0) << " expected: A not called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 0) << " expected: B not called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 1);

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, trigger_always) {
  // test specification
  // set trigger: trigger_always => execute always
  // additional subscriber with ALWAYS
  // test setup:
  // - subscriber A - ON_NEW_DATA
  // - subscriber B - ALWAYS

  // - spin_some()
  // - expected result:
  //   - subscriber A not called
  //   - subscriber B called
  // - publish topic A
  // - spin_some()
  //   - subscriber A called
  //   - subscriber B called

  // -------------- rcl objects ---------------------------------------------------------
  rcl_ret_t rc;
  rclc_executor_t executor;
  // initialize executor with 2 handles
  rc = rclc_executor_init(&executor, &this->context, 2, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcl_reset_error();
  rc = rclc_executor_set_trigger(&executor, rclc_executor_trigger_always, NULL);

  // add subscriptions to executor
  rc = rclc_executor_add_subscription(
    &executor, &this->sub1, &this->sub1_msg,
    &CALLBACK_1, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  rc = rclc_executor_add_subscription(
    &executor, &this->sub2, &this->sub2_msg,
    &CALLBACK_2, ALWAYS);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rcutils_reset_error();
  // ------------------------- test case setup --------------------------------------------
  // first round
  _results_callback_init();
  _executor_results_init();
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 0);
  EXPECT_EQ(_cb2_int_value, (unsigned int) 0);
  EXPECT_EQ(_cb1_cnt, (unsigned int) 0) << " expected: A not called";
  EXPECT_EQ(_cb2_cnt, (unsigned int) 1) << " expected: B called";

  // second round
  this->pub1_msg.data = 3;
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 3) << " expected: A called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 0) << " expected: B called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1) << " expected: A called";
  EXPECT_EQ(_cb2_cnt, (unsigned int) 2) << " expected: B called";
  // third round
  this->pub2_msg.data = 7;
  _cb1_int_value = 0;
  _cb2_int_value = 0;
  rc = rcl_publish(&this->pub2, &this->pub2_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub2 did not publish!";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(_cb1_int_value, (unsigned int) 0) << " expected: A not called";
  EXPECT_EQ(_cb2_int_value, (unsigned int) 7) << " expected: B called";
  EXPECT_EQ(_cb1_cnt, (unsigned int) 1);
  EXPECT_EQ(_cb2_cnt, (unsigned int) 3);

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}
*/
TEST_F(TestDefaultExecutor, executor_test_service) {
  // This unit test tests, if a request from a client is received by the executor
  // and the corresponding service callback is called
  // the value of the request message is checked.
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  const char * service_name = "addtwoints";
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_service_t service = rcl_get_zero_initialized_service();
  const rosidl_service_type_support_t * service_type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
  rc =
    rcl_service_init(&service, &this->node, service_type_support, service_name, &service_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  example_interfaces__srv__AddTwoInts_Request req;
  example_interfaces__srv__AddTwoInts_Request__init(&req);
  example_interfaces__srv__AddTwoInts_Response resp;
  example_interfaces__srv__AddTwoInts_Response__init(&resp);

  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 0);

  rc = rclc_executor_add_service(&executor, &service, &req, &resp, &service_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 1);

  // Creating client and options
  rcl_client_options_t client_options = rcl_client_get_default_options();
  rcl_client_t client = rcl_get_zero_initialized_client();
  rc = rcl_client_init(&client, &this->node, service_type_support, service_name, &client_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // client messages
  example_interfaces__srv__AddTwoInts_Request cli_req;
  example_interfaces__srv__AddTwoInts_Request__init(&cli_req);
  example_interfaces__srv__AddTwoInts_Response cli_resp;
  example_interfaces__srv__AddTwoInts_Response__init(&cli_resp);

  // add client to executor
  rc = rclc_executor_add_client(&executor, &client, &cli_resp, client_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 1);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 1);

  // send client request
  int64_t seq;
  cli_req.a = 1;
  cli_req.b = 2;
  rc = rcl_send_request(&client, &cli_req, &seq);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // initialize test results
  _results_initialize_service_client();
  EXPECT_EQ(srv1_cnt, (unsigned int) 0);
  EXPECT_EQ(srv1_value, (unsigned int) 0);

  // spin executor, which will
  // - receive request from client
  // - call service_callback function
  // - send response message to client
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);

  EXPECT_EQ(srv1_cnt, (unsigned int) 1);  // check that service callback was called
  EXPECT_EQ(srv1_value, (unsigned int) 1);  // check value of 'a' in request message

  // spin executor, which will
  // - receive response message from server
  // - call client_callback
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);

  EXPECT_EQ(cli1_cnt, (unsigned int) 1);  // check that client callback was called
  EXPECT_EQ(cli1_value, (unsigned int) 3);  // check value of 'sum' in response message

  // tear down
  rc = rcl_service_fini(&service, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rcl_client_fini(&client, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  example_interfaces__srv__AddTwoInts_Request__fini(&req);
  example_interfaces__srv__AddTwoInts_Response__fini(&resp);
  example_interfaces__srv__AddTwoInts_Request__fini(&cli_req);
  example_interfaces__srv__AddTwoInts_Response__fini(&cli_resp);
}

TEST_F(TestDefaultExecutor, executor_test_service_with_reqid) {
  // This unit test tests, if a request from a client is received by the executor
  // and the corresponding service callback is called
  // the value of the request message is checked.
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  const char * service_name = "addtwoints";
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_service_t service = rcl_get_zero_initialized_service();
  const rosidl_service_type_support_t * service_type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
  rc =
    rcl_service_init(&service, &this->node, service_type_support, service_name, &service_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  example_interfaces__srv__AddTwoInts_Request req;
  example_interfaces__srv__AddTwoInts_Request__init(&req);
  example_interfaces__srv__AddTwoInts_Response resp;
  example_interfaces__srv__AddTwoInts_Response__init(&resp);

  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 0);

  rc = rclc_executor_add_service_with_request_id(
    &executor, &service, &req, &resp,
    &service_callback_with_reqid);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 1);

  // Creating client and options
  rcl_client_options_t client_options = rcl_client_get_default_options();
  rcl_client_t client = rcl_get_zero_initialized_client();
  rc = rcl_client_init(&client, &this->node, service_type_support, service_name, &client_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // client messages
  example_interfaces__srv__AddTwoInts_Request cli_req;
  example_interfaces__srv__AddTwoInts_Request__init(&cli_req);
  example_interfaces__srv__AddTwoInts_Response cli_resp;
  example_interfaces__srv__AddTwoInts_Response__init(&cli_resp);

  // add client to executor
  rc = rclc_executor_add_client(
    &executor, &client, &cli_resp,
    client_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 1);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 1);

  // send client request
  int64_t seq;
  cli_req.a = 1;
  cli_req.b = 2;
  rc = rcl_send_request(&client, &cli_req, &seq);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // initialize test results
  _results_initialize_service_client();
  EXPECT_EQ(srv1_cnt, (unsigned int) 0);
  EXPECT_EQ(srv1_value, (unsigned int) 0);
  EXPECT_EQ(srv1_id, (unsigned int) 0);

  // spin executor, which will
  // - receive request from client
  // - call service_callback function
  // - send response message to client
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);

  EXPECT_EQ(srv1_cnt, (unsigned int) 1);  // check that service callback was called
  EXPECT_EQ(srv1_value, (unsigned int) 1);  // check value of 'a' in request message

  // spin executor, which will
  // - receive response message from server
  // - call client_callback
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);

  EXPECT_EQ(cli1_cnt, (unsigned int) 1);  // check that client callback was called
  EXPECT_EQ(cli1_value, (unsigned int) 3);  // check value of 'sum' in response message

  // tear down
  rc = rcl_service_fini(&service, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rcl_client_fini(&client, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  example_interfaces__srv__AddTwoInts_Request__fini(&req);
  example_interfaces__srv__AddTwoInts_Response__fini(&resp);
  example_interfaces__srv__AddTwoInts_Request__fini(&cli_req);
  example_interfaces__srv__AddTwoInts_Response__fini(&cli_resp);
}

TEST_F(TestDefaultExecutor, executor_test_service_with_context) {
  // This unit test tests, if a request from a client is received by the executor
  // and the corresponding service callback is called
  // the value of the request message is checked as well as the
  // additional service context
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  const char * service_name = "addtwoints";
  rcl_service_options_t service_options = rcl_service_get_default_options();
  rcl_service_t service = rcl_get_zero_initialized_service();
  const rosidl_service_type_support_t * service_type_support =
    ROSIDL_GET_SRV_TYPE_SUPPORT(example_interfaces, srv, AddTwoInts);
  rc =
    rcl_service_init(&service, &this->node, service_type_support, service_name, &service_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  example_interfaces__srv__AddTwoInts_Request req;
  example_interfaces__srv__AddTwoInts_Request__init(&req);
  example_interfaces__srv__AddTwoInts_Response resp;
  example_interfaces__srv__AddTwoInts_Response__init(&resp);

  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 0);

  rc = rclc_executor_add_service_with_context(
    &executor, &service,
    &req, &resp,
    &service_callback_with_context,
    &srv1_ctxt);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 0);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 1);

  // Creating client and options
  rcl_client_options_t client_options = rcl_client_get_default_options();
  rcl_client_t client = rcl_get_zero_initialized_client();
  rc = rcl_client_init(&client, &this->node, service_type_support, service_name, &client_options);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // client messages
  example_interfaces__srv__AddTwoInts_Request cli_req;
  example_interfaces__srv__AddTwoInts_Request__init(&cli_req);
  example_interfaces__srv__AddTwoInts_Response cli_resp;
  example_interfaces__srv__AddTwoInts_Response__init(&cli_resp);

  // add client to executor
  rc = rclc_executor_add_client(
    &executor, &client, &cli_resp,
    client_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_clients, (size_t) 1);
  EXPECT_EQ(executor.info.number_of_services, (size_t) 1);

  // send client request
  int64_t seq;
  cli_req.a = 1;
  cli_req.b = 2;
  rc = rcl_send_request(&client, &cli_req, &seq);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // initialize test results
  _results_initialize_service_client();
  EXPECT_EQ(srv1_cnt, (unsigned int) 0);
  EXPECT_EQ(srv1_value, (unsigned int) 0);
  EXPECT_EQ(srv1_ctxt, (unsigned int) 42);  // input value for context
  EXPECT_EQ(srv1_id, (unsigned int) 0);     // output value of context (in callback)

  // spin executor, which will
  // - receive request from client
  // - call service_callback function
  // - send response message to client
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);

  EXPECT_EQ(srv1_cnt, (unsigned int) 1);  // check that service callback was called
  EXPECT_EQ(srv1_value, (unsigned int) 1);  // check value of 'a' in request message
  EXPECT_EQ(srv1_id, (unsigned int) 42);  // check context value in callback

  // spin executor, which will
  // - receive response message from server
  // - call client_callback
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);

  EXPECT_EQ(cli1_cnt, (unsigned int) 1);  // check that client callback was called
  EXPECT_EQ(cli1_value, (unsigned int) 3);  // check value of 'sum' in response message

  // tear down
  rc = rcl_service_fini(&service, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rcl_client_fini(&client, &this->node);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  example_interfaces__srv__AddTwoInts_Request__fini(&req);
  example_interfaces__srv__AddTwoInts_Response__fini(&resp);
  example_interfaces__srv__AddTwoInts_Request__fini(&cli_req);
  example_interfaces__srv__AddTwoInts_Response__fini(&cli_resp);
}

TEST_F(TestDefaultExecutor, executor_test_subscription_with_context) {
  // This unit test tests, that a subscription with context receives the correct context pointer
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 10, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // prepare a context pointer carrying a value that differs from the published value
  int32_t sub_context_value = 0;
  void * sub_context_ptr = reinterpret_cast<void *>( &sub_context_value );

  std_msgs__msg__Int32__init(&this->pub1_msg);
  this->pub1_msg.data = 42;

  // create a subscription on the same topic as our publisher
  EXPECT_EQ(executor.info.number_of_subscriptions, (size_t) 0) <<
    "number of subscriptions was not initialised at zero";

  rc = rclc_executor_add_subscription_with_context(
    &executor, &this->sub1, &this->sub1_msg,
    &int32_callback_with_context, sub_context_ptr, ON_NEW_DATA);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_subscriptions, (size_t) 1) <<
    "number of subscriptions is expected to be one";

  // run the callback
  rc = rcl_publish(&this->pub1, &this->pub1_msg, nullptr);
  EXPECT_EQ(RCL_RET_OK, rc) << " pub1 not published";
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);

  // check the side effect
  EXPECT_EQ(this->pub1_msg.data, sub_context_value) <<
    "subscription did not alter context value";

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_test_guard_condition) {
  // Test guard_condition.
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 1, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // initialize guard condition
  rcl_guard_condition_t guard_cond = rcl_get_zero_initialized_guard_condition();
  rc = rcl_guard_condition_init(
    &guard_cond, &this->context, rcl_guard_condition_get_default_options());
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // add gc to executor - with invalid arguments
  rc = rclc_executor_add_guard_condition(NULL, &guard_cond, &gc_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_guard_conditions, (size_t) 0);

  rc = rclc_executor_add_guard_condition(&executor, NULL, &gc_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_guard_conditions, (size_t) 0);

  rc = rclc_executor_add_guard_condition(&executor, &guard_cond, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  EXPECT_EQ(executor.info.number_of_guard_conditions, (size_t) 0);

  // add gc to executor - valid arguments
  EXPECT_EQ(executor.info.number_of_guard_conditions, (size_t) 0);
  rc = rclc_executor_add_guard_condition(&executor, &guard_cond, &gc_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_guard_conditions, (size_t) 1);

  // trigger guard condition
  rc = rcl_trigger_guard_condition(&guard_cond);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // spin once - expect that guard condition callback is called
  gc1_cnt = 0;
  EXPECT_EQ(gc1_cnt, (unsigned int) 0);
  std::this_thread::sleep_for(rclc_test_sleep_time);
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);
  EXPECT_EQ(gc1_cnt, (unsigned int) 1);

  // tear down
  rc = rcl_guard_condition_fini(&guard_cond);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, prepare_executor_test) {
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 1, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // initialize guard condition
  rcl_guard_condition_t guard_cond = rcl_get_zero_initialized_guard_condition();
  rc = rcl_guard_condition_init(
    &guard_cond, &this->context, rcl_guard_condition_get_default_options());
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // prepare executor
  rc = rclc_executor_prepare(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // spin once
  rclc_executor_spin_some(&executor, rclc_test_timeout_ns);

  // tear down
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}

TEST_F(TestDefaultExecutor, executor_test_remove_guard_condition) {
  // Test guard_condition.
  rcl_ret_t rc;
  rclc_executor_t executor;
  executor = rclc_executor_get_zero_initialized_executor();
  rc = rclc_executor_init(&executor, &this->context, 1, this->allocator_ptr);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  // initialize guard condition
  rcl_guard_condition_t guard_cond = rcl_get_zero_initialized_guard_condition();
  rc = rcl_guard_condition_init(
    &guard_cond, &this->context, rcl_guard_condition_get_default_options());
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;

  rc = rclc_executor_add_guard_condition(&executor, &guard_cond, &gc_callback);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_guard_conditions, (size_t) 1);

  // test remove guard condition - invalid args
  rc = rclc_executor_remove_guard_condition(&executor, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  rc = rclc_executor_remove_guard_condition(NULL, &guard_cond);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc) << rcl_get_error_string().str;
  rcutils_reset_error();

  // test remove guard condition - valid args
  rc = rclc_executor_remove_guard_condition(&executor, &guard_cond);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  EXPECT_EQ(executor.info.number_of_guard_conditions, (size_t) 0);

  // test remove non-existent guard condition
  rc = rclc_executor_remove_guard_condition(&executor, &guard_cond);
  EXPECT_EQ(RCL_RET_ERROR, rc) << rcl_get_error_string().str;
  rcutils_reset_error();


  // tear down
  rc = rcl_guard_condition_fini(&guard_cond);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
  rc = rclc_executor_fini(&executor);
  EXPECT_EQ(RCL_RET_OK, rc) << rcl_get_error_string().str;
}
