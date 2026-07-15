// Copyright (c) 2019 - for information on the respective copyright owner
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
#include <rclc/rclc.h>

void my_callback(rcl_timer_t * timer, int64_t last_call)
{
  RCLC_UNUSED(timer);
  RCLC_UNUSED(last_call);
}

TEST(Test, rclc_timer_init_default) {
  rclc_support_t support;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  const char * my_name = "test_name";
  const char * my_namespace = "test_namespace";
  rcl_node_t node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&node, my_name, my_namespace, &support);

  // test with valid arguments

  rcl_timer_t timer = rcl_get_zero_initialized_timer();
  rc = rclc_timer_init_default(&timer, &support, 10000000, my_callback);
  EXPECT_EQ(RCL_RET_OK, rc);

  // tests with invalid arguments
  rc = rclc_timer_init_default(nullptr, &support, 10000000, my_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_timer_init_default(&timer, nullptr, 10000000, my_callback);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // clean up
  rc = rcl_timer_fini(&timer);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}
