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

#include "gmock/gmock.h"
#include "rmw/convert_rcutils_ret_to_rmw_ret.h"

TEST(test_convert_rcutils_ret_to_rmw_ret, convert_rcutils_ret)
{
  // Types that currently have a 1-to-1 correspondance. Add below as mappings are added
  EXPECT_EQ(rmw_convert_rcutils_ret_to_rmw_ret(RCUTILS_RET_OK), RMW_RET_OK);
  EXPECT_EQ(
    rmw_convert_rcutils_ret_to_rmw_ret(RCUTILS_RET_INVALID_ARGUMENT), RMW_RET_INVALID_ARGUMENT);
  EXPECT_EQ(rmw_convert_rcutils_ret_to_rmw_ret(RCUTILS_RET_BAD_ALLOC), RMW_RET_BAD_ALLOC);
  EXPECT_EQ(rmw_convert_rcutils_ret_to_rmw_ret(RCUTILS_RET_ERROR), RMW_RET_ERROR);

  // Some of the types that don't have mappings in rmw
  EXPECT_EQ(rmw_convert_rcutils_ret_to_rmw_ret(RCUTILS_RET_WARN), RMW_RET_ERROR);
  EXPECT_EQ(rmw_convert_rcutils_ret_to_rmw_ret(RCUTILS_RET_NOT_ENOUGH_SPACE), RMW_RET_ERROR);
  EXPECT_EQ(rmw_convert_rcutils_ret_to_rmw_ret(RCUTILS_RET_NOT_INITIALIZED), RMW_RET_ERROR);
  EXPECT_EQ(rmw_convert_rcutils_ret_to_rmw_ret(RCUTILS_RET_NOT_FOUND), RMW_RET_ERROR);
}
