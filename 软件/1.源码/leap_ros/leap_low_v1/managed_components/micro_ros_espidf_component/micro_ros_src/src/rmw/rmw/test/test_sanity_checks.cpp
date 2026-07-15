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

#include "rmw/error_handling.h"
#include "rmw/sanity_checks.h"

TEST(rmw_sanity_checks, check_zero_rmw_string_array)
{
  EXPECT_EQ(rmw_check_zero_rmw_string_array(nullptr), RMW_RET_ERROR);
  rmw_reset_error();

  // size is not 0
  rcutils_string_array_t string_array;
  string_array.size = 1;
  EXPECT_EQ(rmw_check_zero_rmw_string_array(&string_array), RMW_RET_ERROR);
  string_array.size = 0;
  rmw_reset_error();

  // data is not nullptr
  char s[20] = "hello there";
  char * data[2] = {&s[0], &s[1]};
  string_array.data = &data[0];
  EXPECT_EQ(rmw_check_zero_rmw_string_array(&string_array), RMW_RET_ERROR);
  rmw_reset_error();

  // Ok
  string_array = rcutils_get_zero_initialized_string_array();
  EXPECT_EQ(rmw_check_zero_rmw_string_array(&string_array), RMW_RET_OK);
}
