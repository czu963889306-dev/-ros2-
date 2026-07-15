// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#include "rcl/rcl.h"
#include "rcl/guard_condition.h"

#include "./failing_allocator_functions.hpp"
#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"

#include "rmw/rmw.h"

#include "../mocking_utils/patch.hpp"

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

class CLASSNAME (TestGuardConditionFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
    osrf_testing_tools_cpp::memory_tools::initialize();
    on_unexpected_malloc([]() {ASSERT_FALSE(true) << "UNEXPECTED MALLOC";});
    on_unexpected_realloc([]() {ASSERT_FALSE(true) << "UNEXPECTED REALLOC";});
    on_unexpected_calloc([]() {ASSERT_FALSE(true) << "UNEXPECTED CALLOC";});
    on_unexpected_free([]() {ASSERT_FALSE(true) << "UNEXPECTED FREE";});
  }

  void TearDown()
  {
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  }
};

/* Tests the guard condition accessors, i.e. rcl_guard_condition_get_* functions.
 */
TEST_F(
  CLASSNAME(TestGuardConditionFixture, RMW_IMPLEMENTATION), test_rcl_guard_condition_accessors) {
  osrf_testing_tools_cpp::memory_tools::enable_monitoring_in_all_threads();

  rcl_ret_t ret;

  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
    ASSERT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options));
  });
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  // Setup automatic rcl_shutdown()
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });

  // Create a zero initialized guard_condition (but not initialized).
  rcl_guard_condition_t zero_guard_condition = rcl_get_zero_initialized_guard_condition();

  // Create a normal guard_condition.
  rcl_guard_condition_options_t default_options = rcl_guard_condition_get_default_options();
  rcl_guard_condition_t guard_condition = rcl_get_zero_initialized_guard_condition();
  ret = rcl_guard_condition_init(&guard_condition, &context, default_options);
  ASSERT_EQ(RCL_RET_OK, ret);
  // Setup automatic finalization of guard condition.
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
    rcl_ret_t ret = rcl_guard_condition_fini(&guard_condition);
    EXPECT_EQ(RCL_RET_OK, ret);
  });

  // Test rcl_guard_condition_get_options().
  const rcl_guard_condition_options_t * actual_options;
  actual_options = rcl_guard_condition_get_options(nullptr);
  EXPECT_EQ(nullptr, actual_options);
  rcl_reset_error();
  actual_options = rcl_guard_condition_get_options(&zero_guard_condition);
  EXPECT_EQ(nullptr, actual_options);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    actual_options = rcl_guard_condition_get_options(&guard_condition);
  });
  EXPECT_NE(nullptr, actual_options);
  if (actual_options) {
    EXPECT_EQ(default_options.allocator.allocate, actual_options->allocator.allocate);
  }
  // Test rcl_guard_condition_get_rmw_handle().
  rmw_guard_condition_t * gc_handle;
  gc_handle = rcl_guard_condition_get_rmw_handle(nullptr);
  EXPECT_EQ(nullptr, gc_handle);
  rcl_reset_error();
  gc_handle = rcl_guard_condition_get_rmw_handle(&zero_guard_condition);
  EXPECT_EQ(nullptr, gc_handle);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    gc_handle = rcl_guard_condition_get_rmw_handle(&guard_condition);
  });
  EXPECT_NE(nullptr, gc_handle);
}

/* Tests the guard condition life cycle, including rcl_guard_condition_init/fini().
 */
TEST_F(
  CLASSNAME(TestGuardConditionFixture, RMW_IMPLEMENTATION), test_rcl_guard_condition_life_cycle) {
  rcl_ret_t ret;
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_guard_condition_t guard_condition = rcl_get_zero_initialized_guard_condition();
  rcl_guard_condition_options_t default_options = rcl_guard_condition_get_default_options();
  // Trying to init before rcl_init() should fail.
  ret = rcl_guard_condition_init(&guard_condition, &context, default_options);
  ASSERT_EQ(RCL_RET_NOT_INIT, ret) << "Expected RCL_RET_NOT_INIT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
    ASSERT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options));
  });
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });
  // Try invalid arguments.
  ret = rcl_guard_condition_init(nullptr, &context, default_options);
  ASSERT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << "Expected RCL_RET_INVALID_ARGUMENT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Now with nullptr for context.
  ret = rcl_guard_condition_init(&guard_condition, nullptr, default_options);
  ASSERT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << "Expected RCL_RET_INVALID_ARGUMENT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Try with invalid allocator.
  rcl_guard_condition_options_t options_with_invalid_allocator =
    rcl_guard_condition_get_default_options();
  options_with_invalid_allocator.allocator.allocate = nullptr;
  options_with_invalid_allocator.allocator.deallocate = nullptr;
  options_with_invalid_allocator.allocator.reallocate = nullptr;
  ret = rcl_guard_condition_init(&guard_condition, &context, options_with_invalid_allocator);
  ASSERT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << "Expected RCL_RET_INVALID_ARGUMENT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Try with failing allocator.
  rcl_guard_condition_options_t options_with_failing_allocator =
    rcl_guard_condition_get_default_options();
  options_with_failing_allocator.allocator.allocate = failing_malloc;
  options_with_failing_allocator.allocator.reallocate = failing_realloc;
  options_with_failing_allocator.allocator.zero_allocate = failing_calloc;
  ret = rcl_guard_condition_init(&guard_condition, &context, options_with_failing_allocator);
  ASSERT_EQ(RCL_RET_BAD_ALLOC, ret) << "Expected RCL_RET_BAD_ALLOC";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Try init but force an internal error.
  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rmw_create_guard_condition, "internal error", nullptr);
    ret = rcl_guard_condition_init(&guard_condition, &context, default_options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }

  // Try fini with invalid arguments.
  ret = rcl_guard_condition_fini(nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << "Expected RCL_RET_INVALID_ARGUMENT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Try fini with an uninitialized guard_condition.
  ret = rcl_guard_condition_fini(&guard_condition);
  EXPECT_EQ(RCL_RET_OK, ret);
  // Try a normal init and fini.
  ret = rcl_guard_condition_init(&guard_condition, &context, default_options);
  EXPECT_EQ(RCL_RET_OK, ret);
  ret = rcl_guard_condition_fini(&guard_condition);
  EXPECT_EQ(RCL_RET_OK, ret);
  // Try normal init and fini, but force an internal error on first try.
  {
    auto mock = mocking_utils::inject_on_return(
      "lib:rcl", rmw_destroy_guard_condition, RMW_RET_ERROR);
    ret = rcl_guard_condition_init(&guard_condition, &context, default_options);
    EXPECT_EQ(RCL_RET_OK, ret);
    ret = rcl_guard_condition_fini(&guard_condition);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }
  ret = rcl_guard_condition_fini(&guard_condition);
  EXPECT_EQ(RCL_RET_OK, ret);
  // Try repeated init and fini calls.
  ret = rcl_guard_condition_init(&guard_condition, &context, default_options);
  EXPECT_EQ(RCL_RET_OK, ret);
  ret = rcl_guard_condition_init(&guard_condition, &context, default_options);
  EXPECT_EQ(RCL_RET_ALREADY_INIT, ret) << "Expected RCL_RET_ALREADY_INIT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  ret = rcl_guard_condition_fini(&guard_condition);
  EXPECT_EQ(RCL_RET_OK, ret);
  rcl_reset_error();
  ret = rcl_guard_condition_fini(&guard_condition);
  EXPECT_EQ(RCL_RET_OK, ret);
  rcl_reset_error();
}

/* Tests trigger_guard_condition with bad arguments
 */
TEST_F(
  CLASSNAME(TestGuardConditionFixture, RMW_IMPLEMENTATION), test_rcl_guard_condition_bad_arg) {
  rcl_guard_condition_t zero_guard_condition = rcl_get_zero_initialized_guard_condition();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_trigger_guard_condition(nullptr));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_trigger_guard_condition(&zero_guard_condition));
  rcl_reset_error();
}
