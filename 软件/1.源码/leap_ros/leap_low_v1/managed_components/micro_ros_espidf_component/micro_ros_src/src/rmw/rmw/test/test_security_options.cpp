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

#include "./time_bomb_allocator_testing_utils.h"
#include "rmw/error_handling.h"
#include "rmw/security_options.h"

TEST(rmw_security_options, get_zero_init)
{
  rmw_security_options_t options = rmw_get_zero_initialized_security_options();
  EXPECT_EQ(options.enforce_security, RMW_SECURITY_ENFORCEMENT_PERMISSIVE);
  EXPECT_EQ(options.security_root_path, nullptr);
}

TEST(rmw_security_options, get_default_init)
{
  rmw_security_options_t options = rmw_get_default_security_options();
  EXPECT_EQ(options.enforce_security, RMW_SECURITY_ENFORCEMENT_PERMISSIVE);
  EXPECT_EQ(options.security_root_path, nullptr);
}

TEST(rmw_security_options, options_copy) {
  rmw_security_options_t source = rmw_get_default_security_options();
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  EXPECT_EQ(RMW_RET_OK, rmw_security_options_set_root_path("root_path", &allocator, &source));

  rmw_security_options_t destination = rmw_get_zero_initialized_security_options();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_security_options_copy(nullptr, &allocator, &destination));

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_security_options_copy(&source, nullptr, &destination));

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_security_options_copy(&source, &allocator, nullptr));

  rcutils_allocator_t failing_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(failing_allocator, 0);
  EXPECT_EQ(
    RMW_RET_BAD_ALLOC,
    rmw_security_options_copy(&source, &failing_allocator, &destination));

  EXPECT_EQ(
    RMW_RET_OK,
    rmw_security_options_copy(&source, &allocator, &destination));

  EXPECT_EQ(RMW_RET_OK, rmw_security_options_fini(&source, &allocator));
  EXPECT_EQ(RMW_RET_OK, rmw_security_options_fini(&destination, &allocator));
}

TEST(rmw_security_options, security_root_path) {
  rmw_security_options_t options = rmw_get_default_security_options();
  EXPECT_EQ(options.enforce_security, RMW_SECURITY_ENFORCEMENT_PERMISSIVE);
  EXPECT_EQ(options.security_root_path, nullptr);

  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_security_options_set_root_path(nullptr, &allocator, &options));
  EXPECT_EQ(options.security_root_path, nullptr);
  rmw_reset_error();

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_security_options_set_root_path("root_path", nullptr, &options));
  EXPECT_EQ(options.security_root_path, nullptr);
  rmw_reset_error();

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_security_options_set_root_path("root_path", &allocator, nullptr));
  EXPECT_EQ(options.security_root_path, nullptr);
  rmw_reset_error();

  EXPECT_EQ(RMW_RET_OK, rmw_security_options_set_root_path("root_path", &allocator, &options));
  EXPECT_STREQ(options.security_root_path, "root_path");

  EXPECT_EQ(RMW_RET_OK, rmw_security_options_fini(&options, &allocator));
}

TEST(rmw_security_options, security_options_fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_security_options_fini(nullptr, &allocator));

  rmw_security_options_t options = rmw_get_zero_initialized_security_options();
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_security_options_fini(&options, nullptr));

  EXPECT_EQ(RMW_RET_OK, rmw_security_options_fini(&options, &allocator));

  options = rmw_get_default_security_options();
  EXPECT_EQ(RMW_RET_OK, rmw_security_options_fini(&options, &allocator));
}
