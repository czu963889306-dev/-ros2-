// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include "osrf_testing_tools_cpp/memory_tools/gtest_quickstart.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/context.h"
#include "rcl/error_handling.h"
#include "rcl/init.h"

#include "rmw/rmw.h"

#include "../mocking_utils/patch.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestContextFixture, RMW_IMPLEMENTATION) : public ::testing::Test {};

// Test the rcl_context_t's normal function.
// Note: that init/fini are tested in test_init.cpp.
TEST_F(CLASSNAME(TestContextFixture, RMW_IMPLEMENTATION), nominal) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;

  // This prevents memory allocations when setting error states in the future.
  rcl_ret_t ret = rcl_initialize_error_handling_thread_local_storage(rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK);

  // initialization with rcl_init
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(ret, RCL_RET_OK);
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(ret, RCL_RET_OK);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_shutdown(&context);
    EXPECT_EQ(ret, RCL_RET_OK);
    ret = rcl_context_fini(&context);
    EXPECT_EQ(ret, RCL_RET_OK);
  });

  // test rcl_context_get_init_options
  const rcl_init_options_t * init_options_ptr;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    init_options_ptr = rcl_context_get_init_options(nullptr);
  });
  EXPECT_EQ(init_options_ptr, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    init_options_ptr = rcl_context_get_init_options(&context);
  });
  EXPECT_NE(init_options_ptr, nullptr) << rcl_get_error_string().str;
  rcl_reset_error();

  // test rcl_context_get_instance_id
  rcl_context_instance_id_t instance_id;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    instance_id = rcl_context_get_instance_id(nullptr);
  });
  EXPECT_EQ(instance_id, 0UL);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    instance_id = rcl_context_get_instance_id(&context);
  });
  EXPECT_NE(instance_id, 0UL) << rcl_get_error_string().str;
  rcl_reset_error();

  // test rcl_context_get_domain_id
  size_t domain_id;

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_context_get_domain_id(&context, nullptr));
  });
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_context_get_domain_id(nullptr, &domain_id));
  });
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_context_get_domain_id(&context, &domain_id));
  });

  // test rcl_context_is_valid
  bool is_valid;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    is_valid = rcl_context_is_valid(nullptr);
  });
  EXPECT_FALSE(is_valid);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    is_valid = rcl_context_is_valid(&context);
  });
  EXPECT_TRUE(is_valid) << rcl_get_error_string().str;
  rcl_reset_error();

  // test rcl_context_get_rmw_context
  rmw_context_t * rmw_context_ptr;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rmw_context_ptr = rcl_context_get_rmw_context(nullptr);
  });
  EXPECT_EQ(rmw_context_ptr, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rmw_context_ptr = rcl_context_get_rmw_context(&context);
  });
  EXPECT_NE(rmw_context_ptr, nullptr) << rcl_get_error_string().str;
  rcl_reset_error();

  ret = rcl_init_options_fini(&init_options);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(CLASSNAME(TestContextFixture, RMW_IMPLEMENTATION), bad_fini) {
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_context_fini(nullptr));
  rcl_reset_error();

  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });

  rcl_context_t context = rcl_get_zero_initialized_context();

  ret = rcl_context_fini(&context);
  EXPECT_EQ(RCL_RET_OK, ret);

  ret = rcl_init(0, nullptr, &init_options, &context);
  EXPECT_EQ(RCL_RET_OK, ret);

  ret = rcl_context_fini(&context);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  rcl_reset_error();

  ret = rcl_shutdown(&context);
  EXPECT_EQ(ret, RCL_RET_OK);

  {
    auto mock = mocking_utils::inject_on_return(
      "lib:rcl", rmw_context_fini, RMW_RET_ERROR);
    EXPECT_EQ(RCL_RET_ERROR, rcl_context_fini(&context));
    rcl_reset_error();
  }
}
