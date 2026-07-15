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

#include "rcl/domain_id.h"
#include "rcl/error_handling.h"
#include "rcutils/env.h"

#include "../mocking_utils/patch.hpp"

TEST(TestGetDomainId, test_nominal) {
  ASSERT_TRUE(rcutils_set_env("ROS_DOMAIN_ID", "42"));
  size_t domain_id = RCL_DEFAULT_DOMAIN_ID;
  EXPECT_EQ(RCL_RET_OK, rcl_get_default_domain_id(&domain_id));
  EXPECT_EQ(42u, domain_id);

  ASSERT_TRUE(rcutils_set_env("ROS_DOMAIN_ID", ""));
  domain_id = RCL_DEFAULT_DOMAIN_ID;
  EXPECT_EQ(RCL_RET_OK, rcl_get_default_domain_id(&domain_id));
  EXPECT_EQ(RCL_DEFAULT_DOMAIN_ID, domain_id);

  ASSERT_TRUE(rcutils_set_env("ROS_DOMAIN_ID", "0000"));
  domain_id = RCL_DEFAULT_DOMAIN_ID;
  EXPECT_EQ(RCL_RET_OK, rcl_get_default_domain_id(&domain_id));
  EXPECT_EQ(0u, domain_id);

  ASSERT_TRUE(rcutils_set_env("ROS_DOMAIN_ID", "0   not really"));
  domain_id = RCL_DEFAULT_DOMAIN_ID;
  EXPECT_EQ(RCL_RET_ERROR, rcl_get_default_domain_id(&domain_id));
  rcl_reset_error();
  EXPECT_EQ(RCL_DEFAULT_DOMAIN_ID, domain_id);

  ASSERT_TRUE(rcutils_set_env("ROS_DOMAIN_ID", "998446744073709551615"));
  domain_id = RCL_DEFAULT_DOMAIN_ID;
  EXPECT_EQ(RCL_RET_ERROR, rcl_get_default_domain_id(&domain_id));
  rcl_reset_error();
  EXPECT_EQ(RCL_DEFAULT_DOMAIN_ID, domain_id);

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_get_default_domain_id(nullptr));
}

TEST(TestGetDomainId, test_mock_get_default_domain_id) {
  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rcutils_get_env, "argument env_name is null");
  size_t domain_id = RCL_DEFAULT_DOMAIN_ID;
  EXPECT_EQ(RCL_RET_ERROR, rcl_get_default_domain_id(&domain_id));
  EXPECT_EQ(RCL_DEFAULT_DOMAIN_ID, domain_id);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}
