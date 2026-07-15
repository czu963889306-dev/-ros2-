// Copyright 2018-2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <algorithm>
#include <cstring>
#include <map>
#include <string>

#include "rcl/security.h"
#include "rcl/error_handling.h"

#include "rcutils/env.h"
#include "rcutils/filesystem.h"

#include "rmw/error_handling.h"
#include "rmw/rmw.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "./allocator_testing_utils.h"
#include "../mocking_utils/patch.hpp"

#define TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME "/test_security_directory"
#define TEST_ENCLAVE "dummy_enclave"
#define TEST_ENCLAVE_ABSOLUTE "/" TEST_ENCLAVE

#ifndef _WIN32
# define PATH_SEPARATOR "/"
#else
# define PATH_SEPARATOR "\\"
#endif

#define TEST_ENCLAVE_MULTIPLE_TOKENS_ABSOLUTE \
  "/group1" TEST_ENCLAVE_ABSOLUTE
#define TEST_ENCLAVE_MULTIPLE_TOKENS_DIR \
  "group1" PATH_SEPARATOR TEST_ENCLAVE

char g_envstring[512] = {0};

static int putenv_wrapper(const char * env_var)
{
#ifdef _WIN32
  return _putenv(env_var);
#else
  return putenv(const_cast<char *>(env_var));
#endif
}

static int unsetenv_wrapper(const char * var_name)
{
#ifdef _WIN32
  // On windows, putenv("VAR=") deletes VAR from environment
  std::string var(var_name);
  var += "=";
  return _putenv(var.c_str());
#else
  return unsetenv(var_name);
#endif
}

class TestGetSecureRoot : public ::testing::Test
{
protected:
  void SetUp() final
  {
    // Reset rcutil error global state in case a previously
    // running test has failed.
    rcl_reset_error();

    // Always make sure the variable we set is unset at the beginning of a test
    unsetenv_wrapper(ROS_SECURITY_KEYSTORE_VAR_NAME);
    unsetenv_wrapper(ROS_SECURITY_ENCLAVE_OVERRIDE);
    unsetenv_wrapper(ROS_SECURITY_STRATEGY_VAR_NAME);
    unsetenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME);
    allocator = rcl_get_default_allocator();
    root_path = nullptr;
    secure_root = nullptr;
    base_lookup_dir_fqn = nullptr;
  }

  void TearDown() final
  {
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      allocator.deallocate(root_path, allocator.state);
    });
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      allocator.deallocate(secure_root, allocator.state);
    });
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      allocator.deallocate(base_lookup_dir_fqn, allocator.state);
    });
  }

  void set_base_lookup_dir_fqn(const char * resource_dir, const char * resource_dir_name)
  {
    base_lookup_dir_fqn = rcutils_join_path(
      resource_dir, resource_dir_name, allocator);
    std::string putenv_input = ROS_SECURITY_KEYSTORE_VAR_NAME "=";
    putenv_input += base_lookup_dir_fqn;
    memcpy(
      g_envstring, putenv_input.c_str(),
      std::min(putenv_input.length(), sizeof(g_envstring) - 1));
    putenv_wrapper(g_envstring);
  }

  rcl_allocator_t allocator;
  char * root_path;
  char * secure_root;
  char * base_lookup_dir_fqn;
};

TEST_F(TestGetSecureRoot, failureScenarios) {
  EXPECT_EQ(
    rcl_get_secure_root(nullptr, &allocator),
    (char *) NULL);
  rcl_reset_error();

  EXPECT_EQ(
    rcl_get_secure_root(TEST_ENCLAVE_ABSOLUTE, &allocator),
    (char *) NULL);
  rcl_reset_error();

  putenv_wrapper(ROS_SECURITY_KEYSTORE_VAR_NAME "=" TEST_RESOURCES_DIRECTORY);

  /* Security directory is set, but there's no matching directory */
  /// Wrong enclave
  EXPECT_EQ(
    rcl_get_secure_root("some_other_enclave", &allocator),
    (char *) NULL);
  rcl_reset_error();
}

TEST_F(TestGetSecureRoot, successScenarios_local_root_enclave) {
  putenv_wrapper(
    ROS_SECURITY_KEYSTORE_VAR_NAME "="
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME);

  secure_root = rcl_get_secure_root("/", &allocator);
  ASSERT_NE(nullptr, secure_root);
  ASSERT_STREQ(
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME PATH_SEPARATOR "enclaves",
    secure_root);
}

TEST_F(TestGetSecureRoot, successScenarios_local_exactMatch) {
  putenv_wrapper(
    ROS_SECURITY_KEYSTORE_VAR_NAME "="
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME);

  secure_root = rcl_get_secure_root(TEST_ENCLAVE_ABSOLUTE, &allocator);
  ASSERT_NE(nullptr, secure_root);
  std::string secure_root_str(secure_root);
  ASSERT_STREQ(
    TEST_ENCLAVE,
    secure_root_str.substr(secure_root_str.size() - strlen(TEST_ENCLAVE)).c_str());
}

TEST_F(TestGetSecureRoot, successScenarios_local_exactMatch_multipleTokensName) {
  putenv_wrapper(
    ROS_SECURITY_KEYSTORE_VAR_NAME "="
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME);

  secure_root = rcl_get_secure_root(
    TEST_ENCLAVE_MULTIPLE_TOKENS_ABSOLUTE, &allocator);
  ASSERT_NE(nullptr, secure_root);
  std::string secure_root_str(secure_root);
  ASSERT_STREQ(
    TEST_ENCLAVE,
    secure_root_str.substr(secure_root_str.size() - strlen(TEST_ENCLAVE)).c_str());
}

TEST_F(TestGetSecureRoot, nodeSecurityEnclaveOverride_validEnclave) {
  putenv_wrapper(
    ROS_SECURITY_KEYSTORE_VAR_NAME "="
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME);

  /* Specify a valid enclave */
  putenv_wrapper(ROS_SECURITY_ENCLAVE_OVERRIDE "=" TEST_ENCLAVE_ABSOLUTE);
  root_path = rcl_get_secure_root(
    "name shouldn't matter", &allocator);
  ASSERT_STREQ(
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME
    PATH_SEPARATOR "enclaves" PATH_SEPARATOR TEST_ENCLAVE,
    root_path);
}

TEST_F(TestGetSecureRoot, nodeSecurityEnclaveOverride_invalidEnclave) {
  putenv_wrapper(
    ROS_SECURITY_KEYSTORE_VAR_NAME "="
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME);

  /* The override provided should exist. Providing correct node/namespace/root dir won't help
   * if the node override is invalid. */
  putenv_wrapper(
    ROS_SECURITY_ENCLAVE_OVERRIDE
    "=TheresN_oWayThi_sEnclave_Exists_hence_this_should_fail");
  EXPECT_EQ(
    rcl_get_secure_root(TEST_ENCLAVE_ABSOLUTE, &allocator),
    (char *) NULL);
}

TEST_F(TestGetSecureRoot, test_get_security_options) {
  /* The override provided should exist. Providing correct enclave name/root dir
   * won't help if the node override is invalid. */
  rmw_security_options_t options = rmw_get_zero_initialized_security_options();
  putenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME "=false");
  rcl_ret_t ret = rcl_get_security_options_from_environment(
    "doesn't matter at all", &allocator, &options);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(RMW_SECURITY_ENFORCEMENT_PERMISSIVE, options.enforce_security);
  EXPECT_EQ(NULL, options.security_root_path);

  putenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME "=true");
  putenv_wrapper(ROS_SECURITY_STRATEGY_VAR_NAME "=Enforce");
  putenv_wrapper(
    ROS_SECURITY_KEYSTORE_VAR_NAME "="
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME);

  putenv_wrapper(
    ROS_SECURITY_ENCLAVE_OVERRIDE "=" TEST_ENCLAVE_MULTIPLE_TOKENS_ABSOLUTE);
  ret = rcl_get_security_options_from_environment(
    "doesn't matter at all", &allocator, &options);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(RMW_SECURITY_ENFORCEMENT_ENFORCE, options.enforce_security);
  EXPECT_STREQ(
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME
    PATH_SEPARATOR "enclaves" PATH_SEPARATOR TEST_ENCLAVE_MULTIPLE_TOKENS_DIR,
    options.security_root_path);
  EXPECT_EQ(RMW_RET_OK, rmw_security_options_fini(&options, &allocator));

  options = rmw_get_zero_initialized_security_options();
  unsetenv_wrapper(ROS_SECURITY_ENCLAVE_OVERRIDE);
  putenv_wrapper(
    ROS_SECURITY_KEYSTORE_VAR_NAME "="
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME);
  ret = rcl_get_security_options_from_environment(
    TEST_ENCLAVE_ABSOLUTE, &allocator, &options);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(RMW_SECURITY_ENFORCEMENT_ENFORCE, options.enforce_security);
  EXPECT_STREQ(
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME
    PATH_SEPARATOR "enclaves" PATH_SEPARATOR TEST_ENCLAVE,
    options.security_root_path);
  EXPECT_EQ(RMW_RET_OK, rmw_security_options_fini(&options, &allocator));
}

TEST_F(TestGetSecureRoot, test_rcl_security_enabled) {
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_security_enabled(nullptr));
  rcl_reset_error();

  {
    bool use_security;
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rcutils_get_env, "internal error");
    EXPECT_EQ(RCL_RET_ERROR, rcl_security_enabled(&use_security));
    rcl_reset_error();
  }

  {
    bool use_security = false;
    putenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME "=true");
    EXPECT_EQ(RCL_RET_OK, rcl_security_enabled(&use_security));
    EXPECT_TRUE(use_security);
    unsetenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME);
  }

  {
    bool use_security = true;
    putenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME "=false");
    EXPECT_EQ(RCL_RET_OK, rcl_security_enabled(&use_security));
    EXPECT_FALSE(use_security);
    unsetenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME);
  }

  {
    bool use_security = true;
    putenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME "=foo");
    EXPECT_EQ(RCL_RET_OK, rcl_security_enabled(&use_security));
    EXPECT_FALSE(use_security);
    unsetenv_wrapper(ROS_SECURITY_ENABLE_VAR_NAME);
  }

  {
    bool use_security = true;
    EXPECT_EQ(RCL_RET_OK, rcl_security_enabled(&use_security));
    EXPECT_FALSE(use_security);
  }
}

TEST_F(TestGetSecureRoot, test_rcl_get_enforcement_policy) {
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_get_enforcement_policy(nullptr));
  rcl_reset_error();

  {
    rmw_security_enforcement_policy_t policy;
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rcutils_get_env, "internal error");
    EXPECT_EQ(RCL_RET_ERROR, rcl_get_enforcement_policy(&policy));
    rcl_reset_error();
  }

  {
    rmw_security_enforcement_policy_t policy = RMW_SECURITY_ENFORCEMENT_PERMISSIVE;
    putenv_wrapper(ROS_SECURITY_STRATEGY_VAR_NAME "=Enforce");
    EXPECT_EQ(RCL_RET_OK, rcl_get_enforcement_policy(&policy));
    EXPECT_EQ(RMW_SECURITY_ENFORCEMENT_ENFORCE, policy);
    unsetenv_wrapper(ROS_SECURITY_STRATEGY_VAR_NAME);
  }

  {
    rmw_security_enforcement_policy_t policy = RMW_SECURITY_ENFORCEMENT_ENFORCE;
    EXPECT_EQ(RCL_RET_OK, rcl_get_enforcement_policy(&policy));
    EXPECT_EQ(RMW_SECURITY_ENFORCEMENT_PERMISSIVE, policy);
  }

  {
    rmw_security_enforcement_policy_t policy = RMW_SECURITY_ENFORCEMENT_ENFORCE;
    putenv_wrapper(ROS_SECURITY_STRATEGY_VAR_NAME "=foo");
    EXPECT_EQ(RCL_RET_OK, rcl_get_enforcement_policy(&policy));
    EXPECT_EQ(RMW_SECURITY_ENFORCEMENT_PERMISSIVE, policy);
    unsetenv_wrapper(ROS_SECURITY_STRATEGY_VAR_NAME);
  }

  {
    rmw_security_enforcement_policy_t policy = RMW_SECURITY_ENFORCEMENT_ENFORCE;
    putenv_wrapper(ROS_SECURITY_STRATEGY_VAR_NAME "=ENFORCE");
    EXPECT_EQ(RCL_RET_OK, rcl_get_enforcement_policy(&policy));
    EXPECT_EQ(RMW_SECURITY_ENFORCEMENT_PERMISSIVE, policy);
    unsetenv_wrapper(ROS_SECURITY_STRATEGY_VAR_NAME);
  }
}

TEST_F(TestGetSecureRoot, test_rcl_get_secure_root_with_bad_arguments) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  EXPECT_EQ(nullptr, rcl_get_secure_root(nullptr, &allocator));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_EQ(nullptr, rcl_get_secure_root("test", nullptr));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  rcl_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  EXPECT_EQ(nullptr, rcl_get_secure_root("test", &invalid_allocator));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}

TEST_F(TestGetSecureRoot, test_rcl_get_secure_root_with_internal_errors) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_allocator_t failing_allocator = get_time_bombed_allocator();

  std::map<std::string, std::string> env;
  auto mock = mocking_utils::patch(
    "lib:rcl", rcutils_get_env,
    [&](const char * name, const char ** value) -> const char * {
      if (env.count(name) == 0) {
        return "internal error";
      }
      *value = env[name].c_str();
      return nullptr;
    });

  // fail to get ROS_SECURITY_KEYSTORE_VAR_NAME from environment
  EXPECT_EQ(nullptr, rcl_get_secure_root("test", &allocator));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  env[ROS_SECURITY_KEYSTORE_VAR_NAME] =
    TEST_RESOURCES_DIRECTORY TEST_SECURITY_DIRECTORY_RESOURCES_DIR_NAME;

  // fail to copy ROS_SECURITY_KEYSTORE_VAR_NAME value
  set_time_bombed_allocator_count(failing_allocator, 0);
  EXPECT_EQ(nullptr, rcl_get_secure_root("test", &failing_allocator));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // fail to get ROS_SECURITY_ENCLAVE_OVERRIDE from environment
  EXPECT_EQ(nullptr, rcl_get_secure_root("test", &allocator));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  env[ROS_SECURITY_ENCLAVE_OVERRIDE] = TEST_ENCLAVE_ABSOLUTE;

  // fail to copy ROS_SECURITY_ENCLAVE_OVERRIDE value
  set_time_bombed_allocator_count(failing_allocator, 1);
  EXPECT_EQ(nullptr, rcl_get_secure_root("test", &failing_allocator));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}
