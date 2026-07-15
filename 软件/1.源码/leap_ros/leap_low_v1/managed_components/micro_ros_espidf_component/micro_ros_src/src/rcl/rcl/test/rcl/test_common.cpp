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
#include "./common.h"

// This function is not part of the public API
TEST(TestCommonFunctionality, test_rmw_ret_to_rcl_ret) {
  EXPECT_EQ(RCL_RET_OK, rcl_convert_rmw_ret_to_rcl_ret(RMW_RET_OK));
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_convert_rmw_ret_to_rcl_ret(RMW_RET_INVALID_ARGUMENT));
  EXPECT_EQ(RCL_RET_BAD_ALLOC, rcl_convert_rmw_ret_to_rcl_ret(RMW_RET_BAD_ALLOC));
  EXPECT_EQ(RCL_RET_UNSUPPORTED, rcl_convert_rmw_ret_to_rcl_ret(RMW_RET_UNSUPPORTED));
  EXPECT_EQ(
    RCL_RET_NODE_NAME_NON_EXISTENT,
    rcl_convert_rmw_ret_to_rcl_ret(RMW_RET_NODE_NAME_NON_EXISTENT));

  // Default behavior
  EXPECT_EQ(RCL_RET_ERROR, rcl_convert_rmw_ret_to_rcl_ret(RMW_RET_ERROR));
  EXPECT_EQ(RCL_RET_ERROR, rcl_convert_rmw_ret_to_rcl_ret(RMW_RET_TIMEOUT));
  EXPECT_EQ(RCL_RET_ERROR, rcl_convert_rmw_ret_to_rcl_ret(RMW_RET_INCORRECT_RMW_IMPLEMENTATION));
}
