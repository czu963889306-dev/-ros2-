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
#include "rclc/init.h"

TEST(Test, rclc_support_init) {
  rclc_support_t support;
  rcl_ret_t rc;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_OK, rc);
  // after rcl_init context should be valid
  ASSERT_TRUE(rcl_context_is_valid(&support.context));
  EXPECT_EQ(&allocator, support.allocator);
  EXPECT_TRUE(rcl_clock_valid(&support.clock));
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
  // test invalid arguments
  rc = rclc_support_init(nullptr, 0, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_support_init(&support, 0, nullptr, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
}

TEST(Test, rclc_support_init_with_options) {
  rclc_support_t support;
  rcl_ret_t rc;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();

  rc = rcl_init_options_init(&init_options, allocator);
  EXPECT_EQ(RCL_RET_OK, rc);

  rc = rclc_support_init_with_options(&support, 0, nullptr, &init_options, &allocator);
  EXPECT_EQ(RCL_RET_OK, rc);
  // after rcl_init context should be valid
  ASSERT_TRUE(rcl_context_is_valid(&support.context));
  EXPECT_EQ(&allocator, support.allocator);
  EXPECT_TRUE(rcl_clock_valid(&support.clock));
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
  // test invalid arguments
  rc = rclc_support_init_with_options(nullptr, 0, nullptr, &init_options, &allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_support_init_with_options(&support, 0, nullptr, &init_options, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  rc = rclc_support_init_with_options(&support, 0, nullptr, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
}


TEST(Test, rclc_support_fini) {
  rclc_support_t support;
  rcl_ret_t rc;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
  // test invalid arguments
  rc = rclc_support_fini(nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();
  // test calling twice
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_ERROR, rc);
  rcutils_reset_error();
}
