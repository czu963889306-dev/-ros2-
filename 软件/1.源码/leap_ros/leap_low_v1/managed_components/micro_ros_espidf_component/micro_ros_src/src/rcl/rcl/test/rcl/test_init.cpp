// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/arguments.h"
#include "rcl/error_handling.h"
#include "rcl/rcl.h"
#include "rcl/security.h"
#include "rcutils/env.h"
#include "rcutils/format_string.h"
#include "rcutils/snprintf.h"
#include "rcutils/testing/fault_injection.h"

#include "rmw/rmw.h"

#include "./allocator_testing_utils.h"
#include "../mocking_utils/patch.hpp"
#include "../src/rcl/init_options_impl.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_calloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_free;

class CLASSNAME (TestRCLFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
    osrf_testing_tools_cpp::memory_tools::initialize();
    on_unexpected_malloc([]() {ADD_FAILURE() << "UNEXPECTED MALLOC";});
    on_unexpected_realloc([]() {ADD_FAILURE() << "UNEXPECTED REALLOC";});
    on_unexpected_free([]() {ADD_FAILURE() << "UNEXPECTED FREE";});
  }

  void TearDown()
  {
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  }
};

struct FakeTestArgv
{
  FakeTestArgv()
  : allocator(rcl_get_default_allocator()), argc(2)
  {
    this->argv =
      static_cast<char **>(allocator.allocate(2 * sizeof(char *), allocator.state));
    if (!this->argv) {
      throw std::bad_alloc();
    }
    this->argv[0] = rcutils_format_string(allocator, "%s", "foo");
    if (!this->argv[0]) {
      allocator.deallocate(this->argv, allocator.state);
      throw std::bad_alloc();
    }
    this->argv[1] = rcutils_format_string(allocator, "%s", "bar");
    if (!this->argv[1]) {
      allocator.deallocate(this->argv[0], allocator.state);
      allocator.deallocate(this->argv, allocator.state);
      throw std::bad_alloc();
    }
  }

  ~FakeTestArgv()
  {
    if (this->argv) {
      if (this->argc > 0) {
        size_t unsigned_argc = this->argc;
        for (size_t i = 0; i < unsigned_argc; ++i) {
          allocator.deallocate(this->argv[i], allocator.state);
        }
      }
    }
    allocator.deallocate(this->argv, allocator.state);
  }

  rcl_allocator_t allocator;
  int argc;
  char ** argv;

private:
  FakeTestArgv(const FakeTestArgv &) = delete;
};

/* Tests rcl_init_options_init() and rcl_init_options_fini() functions.
 */
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_rcl_init_options_init) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();

  // fini a not empty options
  rcl_ret_t ret = rcl_init_options_fini(&init_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Expected usage
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });

  // Already init
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_ALREADY_INIT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // nullptr
  ret = rcl_init_options_init(nullptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // nullptr
  ret = rcl_init_options_fini(nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

/* Tests calling rcl_init() with invalid arguments fails.
 */
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_rcl_init_invalid_arguments) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });

  {
    // If argc is not 0, but argv is, it should be an invalid argument.
    rcl_context_t context = rcl_get_zero_initialized_context();
    ret = rcl_init(42, nullptr, &init_options, &context);
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
    rcl_reset_error();
    ASSERT_FALSE(rcl_context_is_valid(&context));
  }
  {
    // If argc is not 0, argv is not null but contains one, it should be an invalid argument.
    rcl_context_t context = rcl_get_zero_initialized_context();
    const char * null_args[] = {"some-arg", nullptr};
    ret = rcl_init(2, null_args, &init_options, &context);
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
    rcl_reset_error();
    ASSERT_FALSE(rcl_context_is_valid(&context));
  }
  {
    // If argc is less than 1, argv is not null, it should be an invalid argument.
    rcl_context_t context = rcl_get_zero_initialized_context();
    const char * some_args[] = {"some-arg"};
    ret = rcl_init(0, some_args, &init_options, &context);
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
    rcl_reset_error();
    ASSERT_FALSE(rcl_context_is_valid(&context));
  }
  {
    // If an invalid ROS arg is given, init should fail.
    rcl_context_t context = rcl_get_zero_initialized_context();
    const char * bad_remap_args[] = {
      "some-arg", RCL_ROS_ARGS_FLAG, RCL_REMAP_FLAG, "name:="};
    const size_t argc = sizeof(bad_remap_args) / sizeof(const char *);
    ret = rcl_init(argc, bad_remap_args, &init_options, &context);
    EXPECT_EQ(RCL_RET_INVALID_ROS_ARGS, ret);
    rcl_reset_error();
    ASSERT_FALSE(rcl_context_is_valid(&context));
  }
  {
    // If an invalid enclave is given, init should fail.
    rcl_context_t context = rcl_get_zero_initialized_context();
    const char * bad_enclave_args[] = {
      "some-arg", RCL_ROS_ARGS_FLAG, RCL_ENCLAVE_FLAG, "1foo"};
    const size_t argc = sizeof(bad_enclave_args) / sizeof(const char *);
    ret = rcl_init(argc, bad_enclave_args, &init_options, &context);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
    ASSERT_FALSE(rcl_context_is_valid(&context));
  }
  {
    // If security keystore is invalid, init should fail.
    ASSERT_TRUE(rcutils_set_env(ROS_SECURITY_ENABLE_VAR_NAME, "true"));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_TRUE(rcutils_set_env(ROS_SECURITY_ENABLE_VAR_NAME, ""));
    });
    ASSERT_TRUE(rcutils_set_env(ROS_SECURITY_STRATEGY_VAR_NAME, "Enforce"));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_TRUE(rcutils_set_env(ROS_SECURITY_STRATEGY_VAR_NAME, ""));
    });
    ASSERT_TRUE(rcutils_set_env(ROS_SECURITY_KEYSTORE_VAR_NAME, "/not/a/real/secure/root"));
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_TRUE(rcutils_set_env(ROS_SECURITY_KEYSTORE_VAR_NAME, ""));
    });
    rcl_context_t context = rcl_get_zero_initialized_context();
    ret = rcl_init(0, nullptr, &init_options, &context);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
    ASSERT_FALSE(rcl_context_is_valid(&context));
  }
  {
    // If either the allocate or deallocate function pointers are not set,
    // it should be invalid arg.
    rcl_context_t context = rcl_get_zero_initialized_context();
    rcl_allocator_t allocator = init_options.impl->allocator;
    init_options.impl->allocator =
      (rcl_allocator_t)rcutils_get_zero_initialized_allocator();
    ret = rcl_init(0, nullptr, &init_options, &context);
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
    rcl_reset_error();
    init_options.impl->allocator = allocator;
    ASSERT_FALSE(rcl_context_is_valid(&context));
  }
  {
    // If the malloc call fails (with some valid arguments to copy),
    // it should be a bad alloc.
    FakeTestArgv test_args;
    rcl_allocator_t allocator = init_options.impl->allocator;
    init_options.impl->allocator = get_failing_allocator();
    rcl_context_t context = rcl_get_zero_initialized_context();
    ret = rcl_init(test_args.argc, test_args.argv, &init_options, &context);
    EXPECT_EQ(RCL_RET_BAD_ALLOC, ret);
    rcl_reset_error();
    init_options.impl->allocator = allocator;
    ASSERT_FALSE(rcl_context_is_valid(&context));
  }
}

/* Tests the rcl_init() and rcl_shutdown() functions.
 */
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_rcl_init_and_shutdown) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_context_t context = rcl_get_zero_initialized_context();
  // A shutdown before an init should fail.
  ret = rcl_shutdown(&context);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();
  ASSERT_FALSE(rcl_context_is_valid(&context));
  // If argc is 0 and argv is nullptr and the allocator is valid, it should succeed.
  ret = rcl_init(0, nullptr, &init_options, &context);
  EXPECT_EQ(RCL_RET_OK, ret);
  ASSERT_TRUE(rcl_context_is_valid(&context));
  // Then shutdown should work.
  ret = rcl_shutdown(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  ASSERT_FALSE(rcl_context_is_valid(&context));
  ret = rcl_context_fini(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  context = rcl_get_zero_initialized_context();
  // Valid argc/argv values and a valid allocator should succeed.
  {
    FakeTestArgv test_args;
    ret = rcl_init(test_args.argc, test_args.argv, &init_options, &context);
    EXPECT_EQ(RCL_RET_OK, ret);
    ASSERT_TRUE(rcl_context_is_valid(&context));
  }
  // Then shutdown should work.
  ret = rcl_shutdown(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  ASSERT_FALSE(rcl_context_is_valid(&context));
  // Then a repeated shutdown should fail.
  ret = rcl_shutdown(&context);
  EXPECT_EQ(ret, RCL_RET_ALREADY_SHUTDOWN);
  ASSERT_FALSE(rcl_context_is_valid(&context));
  rcl_reset_error();
  ret = rcl_context_fini(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  context = rcl_get_zero_initialized_context();
  // A repeat call to shutdown should not work.
  ret = rcl_shutdown(&context);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();
  ASSERT_FALSE(rcl_context_is_valid(&context));
  // Repeat, but valid, calls to rcl_init() should fail.
  {
    FakeTestArgv test_args;
    ret = rcl_init(test_args.argc, test_args.argv, &init_options, &context);
    EXPECT_EQ(RCL_RET_OK, ret);
    ASSERT_TRUE(rcl_context_is_valid(&context));
    ret = rcl_init(test_args.argc, test_args.argv, &init_options, &context);
    EXPECT_EQ(RCL_RET_ALREADY_INIT, ret);
    rcl_reset_error();
    ASSERT_TRUE(rcl_context_is_valid(&context));
  }
  // But shutdown should still work.
  ret = rcl_shutdown(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  ASSERT_FALSE(rcl_context_is_valid(&context));
  ret = rcl_context_fini(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  context = rcl_get_zero_initialized_context();
}

/* Tests rcl_init() deals with internal errors correctly.
 */
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_rcl_init_internal_error) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  FakeTestArgv test_args;
  rcl_context_t context = rcl_get_zero_initialized_context();

  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rmw_init, "internal error", RMW_RET_ERROR);
    ret = rcl_init(test_args.argc, test_args.argv, &init_options, &context);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    EXPECT_FALSE(rcl_context_is_valid(&context));
  }

  RCUTILS_FAULT_INJECTION_TEST(
  {
    ret = rcl_init(test_args.argc, test_args.argv, &init_options, &context);

    int64_t count = rcutils_fault_injection_get_count();
    rcutils_fault_injection_set_count(RCUTILS_FAULT_INJECTION_NEVER_FAIL);

    if (RCL_RET_OK == ret) {
      ASSERT_TRUE(rcl_context_is_valid(&context));
      EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context)) << rcl_get_error_string().str;
      EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context)) << rcl_get_error_string().str;
    } else {
      ASSERT_FALSE(rcl_context_is_valid(&context));
      rcl_reset_error();
    }

    rcutils_fault_injection_set_count(count);
  });
}

/* Tests rcl_shutdown() deals with internal errors correctly.
 */
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_rcl_shutdown_internal_error) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_context_t context = rcl_get_zero_initialized_context();

  ret = rcl_init(0, nullptr, &init_options, &context);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context)) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context)) << rcl_get_error_string().str;
  });
  EXPECT_TRUE(rcl_context_is_valid(&context));

  auto mock = mocking_utils::patch_to_fail(
    "lib:rcl", rmw_shutdown, "internal error", RMW_RET_ERROR);
  EXPECT_EQ(RCL_RET_ERROR, rcl_shutdown(&context));
  rcl_reset_error();
}

/* Tests the rcl_get_instance_id() function.
 */
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_rcl_get_instance_id) {
  rcl_ret_t ret;
  rcl_context_t context = rcl_get_zero_initialized_context();
  // Instance id should be 0 before rcl_init().
  EXPECT_EQ(0u, rcl_context_get_instance_id(&context));
  ASSERT_FALSE(rcl_context_is_valid(&context));
  // It should still return 0 after an invalid init.
  ret = rcl_init(1, nullptr, nullptr, &context);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(0u, rcl_context_get_instance_id(&context));
  ASSERT_FALSE(rcl_context_is_valid(&context));
  rcl_reset_error();
  // A non-zero instance id should be returned after a valid init.
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  {
    FakeTestArgv test_args;
    ret = rcl_init(test_args.argc, test_args.argv, &init_options, &context);
    EXPECT_EQ(RCL_RET_OK, ret);
    ASSERT_TRUE(rcl_context_is_valid(&context));
  }
  // And it should be allocation free.
  uint64_t first_instance_id;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    first_instance_id = rcl_context_get_instance_id(&context);
  });
  EXPECT_NE(0u, first_instance_id);
  // Repeat calls should return the same.
  EXPECT_EQ(first_instance_id, rcl_context_get_instance_id(&context));
  EXPECT_EQ(true, rcl_context_is_valid(&context));
  // Calling after a shutdown should return 0.
  ret = rcl_shutdown(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  EXPECT_EQ(0u, rcl_context_get_instance_id(&context));
  ASSERT_FALSE(rcl_context_is_valid(&context));
  ret = rcl_context_fini(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  context = rcl_get_zero_initialized_context();
  // It should return a different value after another valid init.
  {
    FakeTestArgv test_args;
    ret = rcl_init(test_args.argc, test_args.argv, &init_options, &context);
    EXPECT_EQ(RCL_RET_OK, ret);
    ASSERT_TRUE(rcl_context_is_valid(&context));
  }
  EXPECT_NE(0u, rcl_context_get_instance_id(&context));
  EXPECT_NE(first_instance_id, rcl_context_get_instance_id(&context));
  ASSERT_TRUE(rcl_context_is_valid(&context));
  // Shutting down a second time should result in 0 again.
  ret = rcl_shutdown(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
  EXPECT_EQ(0u, rcl_context_get_instance_id(&context));
  ASSERT_FALSE(rcl_context_is_valid(&context));
  ret = rcl_context_fini(&context);
  EXPECT_EQ(ret, RCL_RET_OK);
}

TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_rcl_init_options_access) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_init_options_t not_ini_init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });

  rmw_init_options_t * options = rcl_init_options_get_rmw_init_options(&init_options);
  ASSERT_NE(nullptr, options);
  EXPECT_EQ(0u, options->instance_id);
  EXPECT_EQ(nullptr, options->impl);
  EXPECT_EQ(NULL, rcl_init_options_get_rmw_init_options(nullptr));
  EXPECT_EQ(NULL, rcl_init_options_get_rmw_init_options(&not_ini_init_options));

  const rcl_allocator_t * options_allocator = rcl_init_options_get_allocator(&init_options);
  EXPECT_TRUE(rcutils_allocator_is_valid(options_allocator));
  EXPECT_EQ(NULL, rcl_init_options_get_allocator(nullptr));
  EXPECT_EQ(NULL, rcl_init_options_get_allocator(&not_ini_init_options));

  size_t domain_id;
  ret = rcl_init_options_get_domain_id(NULL, &domain_id);
  ASSERT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_init_options_get_domain_id(&not_ini_init_options, &domain_id);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_init_options_get_domain_id(&init_options, NULL);
  ASSERT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_init_options_get_domain_id(NULL, NULL);
  ASSERT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_init_options_set_domain_id(NULL, domain_id);
  ASSERT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_init_options_set_domain_id(&not_ini_init_options, domain_id);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  ret = rcl_init_options_get_domain_id(&init_options, &domain_id);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_DEFAULT_DOMAIN_ID, domain_id);
  ret = rcl_init_options_set_domain_id(&init_options, static_cast<size_t>(0u));
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ret = rcl_init_options_get_domain_id(&init_options, &domain_id);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(0U, domain_id);

  rcl_init_options_t init_options_dst = rcl_get_zero_initialized_init_options();

  // nullptr copy cases
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_init_options_copy(nullptr, &init_options_dst));
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_init_options_copy(&init_options, nullptr));

  // Expected usage copy
  ASSERT_EQ(RCL_RET_OK, rcl_init_options_copy(&init_options, &init_options_dst));
  ret = rcl_init_options_get_domain_id(&init_options_dst, &domain_id);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(0U, domain_id);
  EXPECT_EQ(RCL_RET_ALREADY_INIT, rcl_init_options_copy(&init_options, &init_options_dst));
  EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options_dst));
}

// Define dummy comparison operators for rcutils_allocator_t type for use with the Mimick Library
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, ==)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, <)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, >)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, !=)

// Tests rcl_init_options_init() mocked to fail
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_mocked_rcl_init_options_ini) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  auto mock = mocking_utils::patch_and_return("lib:rcl", rmw_init_options_init, RMW_RET_ERROR);
  EXPECT_EQ(RCL_RET_ERROR, rcl_init_options_init(&init_options, rcl_get_default_allocator()));
  rcl_reset_error();
}

// Tests rcl_init_options_fini() mocked to fail
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_mocked_rcl_init_options_fini) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  auto mock = mocking_utils::inject_on_return("lib:rcl", rmw_init_options_fini, RMW_RET_ERROR);
  EXPECT_EQ(RCL_RET_ERROR, rcl_init_options_fini(&init_options));
  rcl_reset_error();
  auto mock_ok = mocking_utils::inject_on_return("lib:rcl", rmw_init_options_fini, RMW_RET_OK);
  EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options));
}

// Mock rcl_init_options_copy to fail
TEST_F(CLASSNAME(TestRCLFixture, RMW_IMPLEMENTATION), test_rcl_init_options_copy_fail_rmw_copy) {
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_init_options_t init_options_dst = rcl_get_zero_initialized_init_options();
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    // dst is in a invalid state after failed copy
    EXPECT_EQ(
      RCL_RET_INVALID_ARGUMENT,
      rcl_init_options_fini(&init_options_dst)) << rcl_get_error_string().str;
    rcl_reset_error();
    auto mock_ok = mocking_utils::patch_and_return("lib:rcl", rmw_init_options_fini, RMW_RET_OK);
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options_dst)) << rcl_get_error_string().str;
  });

  // rmw_init_options_copy error is logged
  auto mock = mocking_utils::patch_and_return("lib:rcl", rmw_init_options_copy, RMW_RET_ERROR);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_init_options_copy(&init_options, &init_options_dst));
  rcl_reset_error();
}
