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

#include "rcutils/env.h"
#include "rcutils/strdup.h"

#include "rcl/allocator.h"
#include "rcl/error_handling.h"
#include "rcl/rcl.h"
#include "rcl/rmw_implementation_identifier_check.h"

#include "../mocking_utils/patch.hpp"

TEST(TestRmwCheck, test_rmw_check_id_impl) {
  EXPECT_EQ(RCL_RET_OK, rcl_rmw_implementation_identifier_check());
}

TEST(TestRmwCheck, test_failing_configuration) {
  const char * expected_rmw_impl_env = NULL;
  const char * expected_rmw_id_matches = NULL;

  const char * get_env_var_name = rcutils_get_env(
    RMW_IMPLEMENTATION_ENV_VAR_NAME,
    &expected_rmw_impl_env);
  EXPECT_FALSE(get_env_var_name);

  const char * get_env_id_matches_name = rcutils_get_env(
    RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME,
    &expected_rmw_id_matches);
  EXPECT_FALSE(get_env_id_matches_name);

  // Fail test case, reason: RMW_IMPLEMENTATION_ENV_VAR_NAME set, not matching rmw impl
  EXPECT_TRUE(rcutils_set_env(RMW_IMPLEMENTATION_ENV_VAR_NAME, "some_random_name"));
  EXPECT_TRUE(rcutils_set_env(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, ""));
  EXPECT_EQ(RCL_RET_MISMATCHED_RMW_ID, rcl_rmw_implementation_identifier_check());
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Fail test case, reason: RMW_IMPLEMENTATION_ENV_VAR_NAME set, not matching rmw impl
  EXPECT_TRUE(rcutils_set_env(RMW_IMPLEMENTATION_ENV_VAR_NAME, ""));
  EXPECT_TRUE(rcutils_set_env(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, "some_random_name"));
  EXPECT_EQ(RCL_RET_MISMATCHED_RMW_ID, rcl_rmw_implementation_identifier_check());
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Fail test case, reason: env variables not equal
  EXPECT_TRUE(rcutils_set_env(RMW_IMPLEMENTATION_ENV_VAR_NAME, "some_random_name"));
  EXPECT_TRUE(rcutils_set_env(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, "diff_random"));
  EXPECT_EQ(RCL_RET_ERROR, rcl_rmw_implementation_identifier_check());
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Fail test case, reason: equal env variables do not match rmw impl
  EXPECT_TRUE(rcutils_set_env(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, "some_random_name"));
  EXPECT_TRUE(rcutils_set_env(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, "some_random_name"));
  EXPECT_EQ(RCL_RET_MISMATCHED_RMW_ID, rcl_rmw_implementation_identifier_check());
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Restore env variables set in the test
  EXPECT_TRUE(rcutils_set_env(RMW_IMPLEMENTATION_ENV_VAR_NAME, expected_rmw_impl_env));
  EXPECT_TRUE(rcutils_set_env(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, expected_rmw_id_matches));
}

// Define dummy comparison operators for rcutils_allocator_t type for use with the Mimick Library
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, ==)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, <)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, >)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, !=)

// Mock internal calls to external libraries to fail
TEST(TestRmwCheck, test_mock_rmw_impl_check) {
  {
    // Fail reading RMW_IMPLEMENTATION_ENV_VAR_NAME
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rcutils_get_env, "invalid arg");
    EXPECT_EQ(RCL_RET_ERROR, rcl_rmw_implementation_identifier_check());
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    // Fail copying RMW_IMPLEMENTATION_ENV_VAR_NAME env result
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rcutils_strdup, static_cast<char *>(NULL));
    EXPECT_EQ(RCL_RET_BAD_ALLOC, rcl_rmw_implementation_identifier_check());
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    // Fail reading RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME
    auto mock = mocking_utils::patch(
      "lib:rcl", rcutils_get_env, [](auto, const char ** env_value) {
        static int counter = 1;
        *env_value = "";
        if (counter == 1) {
          counter++;
          return static_cast<const char *>(NULL);
        } else {
          return "argument env_value is null";
        }
      });
    EXPECT_EQ(RCL_RET_ERROR, rcl_rmw_implementation_identifier_check());
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
  {
    // Fail copying RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME env result
    // Set the variable, as is not set by default
    const char * expected_rmw_id_matches = NULL;
    const char * get_env_id_matches_name = rcutils_get_env(
      RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME,
      &expected_rmw_id_matches);
    EXPECT_EQ(NULL, get_env_id_matches_name);
    EXPECT_TRUE(rcutils_set_env(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, "some_random_name"));

    auto mock = mocking_utils::patch(
      "lib:rcl", rcutils_strdup,
      [](const char * str, auto allocator)
      {
        static int counter = 1;
        if (counter == 1) {
          counter++;
          char * dup = static_cast<char *>(
            allocator.allocate(strlen(str) + 1, allocator.state));
          memcpy(dup, str, strlen(str) + 1);
          return dup;
        } else {
          return static_cast<char *>(NULL);
        }
      });
    EXPECT_EQ(RCL_RET_BAD_ALLOC, rcl_rmw_implementation_identifier_check());
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    EXPECT_TRUE(rcutils_set_env(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, expected_rmw_id_matches));
  }
  {
    // Fail reading rmw_impl_identifier
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_get_implementation_identifier, static_cast<char *>(NULL));
    EXPECT_EQ(RCL_RET_ERROR, rcl_rmw_implementation_identifier_check());
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
}
