// Copyright (c) 2020 - for my_handle_counterrmation on the respective copyright owner
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
#include <gtest/gtest.h>
#include "rclc/executor_handle.h"


TEST(Test, executor_handle_counters_zero_init) {
  rcl_ret_t rc;

  rclc_executor_handle_counters_t my_handle_counter;
  size_t zero = 0;
  rc = rclc_executor_handle_counters_zero_init(&my_handle_counter);
  EXPECT_EQ(rc, RCL_RET_OK);
  EXPECT_EQ(my_handle_counter.number_of_subscriptions, zero);
  EXPECT_EQ(my_handle_counter.number_of_guard_conditions, zero);
  EXPECT_EQ(my_handle_counter.number_of_timers, zero);
  EXPECT_EQ(my_handle_counter.number_of_clients, zero);
  EXPECT_EQ(my_handle_counter.number_of_services, zero);
  EXPECT_EQ(my_handle_counter.number_of_events, zero);

  // test null pointer
  rc = rclc_executor_handle_counters_zero_init(nullptr);
  EXPECT_EQ(rc, RCL_RET_INVALID_ARGUMENT);
  rcutils_reset_error();
}

TEST(Test, executor_handle_init) {
  rcl_ret_t rc;

  rclc_executor_handle_t handle;
  size_t max_handles = 10;
  rc = rclc_executor_handle_init(&handle, max_handles);
  EXPECT_EQ(rc, RCL_RET_OK);
  EXPECT_EQ(handle.type, RCLC_NONE);
  EXPECT_EQ(handle.invocation, ON_NEW_DATA);

  EXPECT_EQ(handle.subscription, nullptr);
  EXPECT_EQ(handle.timer, nullptr);
  EXPECT_EQ(handle.client, nullptr);
  EXPECT_EQ(handle.service, nullptr);
  EXPECT_EQ(handle.gc, nullptr);

  EXPECT_EQ(handle.data, nullptr);
  EXPECT_EQ(handle.subscription_callback, nullptr);
  EXPECT_EQ(handle.index, max_handles);
  EXPECT_EQ(handle.initialized, false);
  EXPECT_EQ(handle.data_available, false);

  // test null pointer
  rc = rclc_executor_handle_init(NULL, max_handles);
  EXPECT_EQ(rc, RCL_RET_INVALID_ARGUMENT);
  rcutils_reset_error();
}

TEST(Test, executor_handle_clear) {
  rcl_ret_t rc;

  rclc_executor_handle_t handle;
  size_t max_handles = 10;  // assumption: max_handles > 1
  rc = rclc_executor_handle_init(&handle, max_handles);

  // setup dummy handle
  handle.initialized = true;
  handle.index = 0;

  rc = rclc_executor_handle_clear(&handle, max_handles - 1);
  EXPECT_EQ(rc, RCL_RET_OK);
  EXPECT_EQ(handle.index, max_handles - 1);
  EXPECT_EQ(handle.initialized, false);

  // test null pointer
  rc = rclc_executor_handle_clear(nullptr, max_handles);
  EXPECT_EQ(rc, RCL_RET_INVALID_ARGUMENT);
  rcutils_reset_error();
}

TEST(Test, executor_handle_print) {
  rcl_ret_t rc;

  rclc_executor_handle_t handle;
  size_t max_handles = 10;
  rc = rclc_executor_handle_init(&handle, max_handles);
  EXPECT_EQ(rc, RCL_RET_OK);
  rc = rclc_executor_handle_print(&handle);
  EXPECT_EQ(rc, RCL_RET_OK);

  // test null pointer
  rc = rclc_executor_handle_print(nullptr);
  EXPECT_EQ(rc, RCL_RET_INVALID_ARGUMENT);
  rcutils_reset_error();
}

TEST(Test, executor_handle_get_ptr) {
  rcl_ret_t rc;

  rclc_executor_handle_t handle;
  size_t max_handles = 10;
  rc = rclc_executor_handle_init(&handle, max_handles);
  EXPECT_EQ(rc, RCL_RET_OK);
  rcutils_reset_error();
  // test null pointer
  rcl_subscription_t sub = rcl_get_zero_initialized_subscription();
  rcl_timer_t timer = rcl_get_zero_initialized_timer();
  rcl_client_t client = rcl_get_zero_initialized_client();
  rcl_service_t service = rcl_get_zero_initialized_service();
  rcl_guard_condition_t gc = rcl_get_zero_initialized_guard_condition();
  void * ptr;
  ptr = rclc_executor_handle_get_ptr(nullptr);
  EXPECT_EQ(ptr, nullptr);

  sub = rcl_get_zero_initialized_subscription();
  handle.type = RCLC_SUBSCRIPTION;
  handle.subscription = &sub;
  ptr = rclc_executor_handle_get_ptr(&handle);
  EXPECT_EQ(ptr, &sub);
  rcutils_reset_error();

  timer = rcl_get_zero_initialized_timer();
  handle.type = RCLC_TIMER;
  handle.timer = &timer;
  ptr = rclc_executor_handle_get_ptr(&handle);
  EXPECT_EQ(ptr, &timer);
  rcutils_reset_error();

  client = rcl_get_zero_initialized_client();
  handle.type = RCLC_CLIENT;
  handle.client = &client;
  ptr = rclc_executor_handle_get_ptr(&handle);
  EXPECT_EQ(ptr, &client);
  rcutils_reset_error();

  service = rcl_get_zero_initialized_service();
  handle.type = RCLC_SERVICE;
  handle.service = &service;
  ptr = rclc_executor_handle_get_ptr(&handle);
  EXPECT_EQ(ptr, &service);
  rcutils_reset_error();

  gc = rcl_get_zero_initialized_guard_condition();
  handle.type = RCLC_GUARD_CONDITION;
  handle.gc = &gc;
  ptr = rclc_executor_handle_get_ptr(&handle);
  EXPECT_EQ(ptr, &gc);
  rcutils_reset_error();
}
