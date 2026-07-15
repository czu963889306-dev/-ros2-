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
#include "rmw/init_options.h"

TEST(rmw_init_options, get_zero_initialized_init_options)
{
  const rmw_init_options_t options = rmw_get_zero_initialized_init_options();
  EXPECT_EQ(options.instance_id, 0u);
  EXPECT_EQ(options.implementation_identifier, nullptr);
  EXPECT_EQ(options.impl, nullptr);
}
