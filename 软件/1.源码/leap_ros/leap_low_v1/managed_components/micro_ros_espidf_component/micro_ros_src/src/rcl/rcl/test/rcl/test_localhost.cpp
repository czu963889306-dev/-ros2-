// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include "rcl/rcl.h"
#include "rcl/localhost.h"
#include "rmw/localhost.h"
#include "rcutils/env.h"

TEST(TestLocalhost, test_get_localhost_only) {
  ASSERT_TRUE(rcutils_set_env("ROS_LOCALHOST_ONLY", "0"));
  rmw_localhost_only_t localhost_var;
  EXPECT_EQ(RCL_RET_OK, rcl_get_localhost_only(&localhost_var));
  EXPECT_EQ(RMW_LOCALHOST_ONLY_DISABLED, localhost_var);

  ASSERT_TRUE(rcutils_set_env("ROS_LOCALHOST_ONLY", "1"));
  EXPECT_EQ(RCL_RET_OK, rcl_get_localhost_only(&localhost_var));
  EXPECT_EQ(RMW_LOCALHOST_ONLY_ENABLED, localhost_var);

  ASSERT_TRUE(rcutils_set_env("ROS_LOCALHOST_ONLY", "2"));
  EXPECT_EQ(RCL_RET_OK, rcl_get_localhost_only(&localhost_var));
  EXPECT_EQ(RMW_LOCALHOST_ONLY_DISABLED, localhost_var);

  ASSERT_TRUE(rcutils_set_env("ROS_LOCALHOST_ONLY", "Unexpected"));
  EXPECT_EQ(RCL_RET_OK, rcl_get_localhost_only(&localhost_var));
  EXPECT_EQ(RMW_LOCALHOST_ONLY_DISABLED, localhost_var);

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_get_localhost_only(nullptr));
}
