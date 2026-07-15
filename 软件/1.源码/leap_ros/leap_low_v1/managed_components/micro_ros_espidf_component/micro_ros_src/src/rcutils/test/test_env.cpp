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

#include <string>

#include "rcutils/env.h"
#include "rcutils/error_handling.h"

TEST(TestEnv, test_set_env) {
  const char * res = nullptr;

  // Invalid cases
  EXPECT_FALSE(rcutils_set_env(nullptr, nullptr));
  rcutils_reset_error();
  EXPECT_FALSE(rcutils_set_env("=INVALID_ENV_VAR=", nullptr));
  rcutils_reset_error();
  EXPECT_FALSE(rcutils_set_env("=INVALID_ENV_VAR=", "InvalidEnvValue"));
  rcutils_reset_error();

  // Ensure we're starting clean
  ASSERT_EQ(nullptr, rcutils_get_env("NEW_ENV_VAR", &res));
  ASSERT_STREQ("", res);

  // Simple set
  ASSERT_TRUE(rcutils_set_env("NEW_ENV_VAR", "NewEnvValue"));
  ASSERT_STREQ(nullptr, rcutils_get_env("NEW_ENV_VAR", &res));
  EXPECT_STREQ(res, "NewEnvValue");

  // Re-set
  ASSERT_TRUE(rcutils_set_env("NEW_ENV_VAR", "DifferentEnvValue"));
  ASSERT_STREQ(nullptr, rcutils_get_env("NEW_ENV_VAR", &res));
  EXPECT_STREQ(res, "DifferentEnvValue");

  // Un-set
  ASSERT_TRUE(rcutils_set_env("NEW_ENV_VAR", nullptr));
  ASSERT_EQ(nullptr, rcutils_get_env("NEW_ENV_VAR", &res));
  EXPECT_STREQ("", res);

  // Un-set again
  ASSERT_TRUE(rcutils_set_env("NEW_ENV_VAR", nullptr));
  ASSERT_EQ(nullptr, rcutils_get_env("NEW_ENV_VAR", &res));
  EXPECT_STREQ("", res);
}

/* Tests rcutils_get_env.
 *
 * Expected environment variables must be set by the calling code:
 *
 *   - EMPTY_TEST=
 *   - NORMAL_TEST=foo
 *
 * These are set in the call to `ament_add_gtest()` in the `CMakeLists.txt`.
 */
TEST(TestEnv, test_get_env) {
  const char * env;
  const char * ret;
  ret = rcutils_get_env("NORMAL_TEST", NULL);
  EXPECT_STREQ("argument env_value is null", ret);
  ret = rcutils_get_env(NULL, &env);
  EXPECT_STREQ("argument env_name is null", ret);
  ret = rcutils_get_env("SHOULD_NOT_EXIST_TEST", &env);
  EXPECT_FALSE(ret);
  EXPECT_STREQ("", env);
  ret = rcutils_get_env("NORMAL_TEST", &env);
  EXPECT_FALSE(ret);
  EXPECT_FALSE(NULL == env);
  EXPECT_STREQ("foo", env);
  ret = rcutils_get_env("EMPTY_TEST", &env);
  EXPECT_FALSE(ret);
  EXPECT_FALSE(NULL == env);
  EXPECT_STREQ("", env);
}

TEST(TestEnv, test_get_home) {
  EXPECT_STRNE(NULL, rcutils_get_home_dir());
  const char * home = NULL;

#ifdef _WIN32
  // Assert pre-condition that USERPROFILE is defined
  const char * ret = rcutils_get_env("USERPROFILE", &home);
  ASSERT_EQ(NULL, ret);

  // Check USERPROFILE is not defined
  EXPECT_TRUE(rcutils_set_env("USERPROFILE", NULL));
  EXPECT_EQ(NULL, rcutils_get_home_dir());
#else
  // Assert pre-condition that HOME is defined
  const char * ret = rcutils_get_env("HOME", &home);
  ASSERT_EQ(NULL, ret);

  // Check HOME is not defined
  EXPECT_TRUE(rcutils_set_env("HOME", NULL));
  EXPECT_EQ(NULL, rcutils_get_home_dir());
#endif
}
