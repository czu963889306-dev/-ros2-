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

#include "./allocator_testing_utils.h"
#include "./mocking_utils/patch.hpp"

#include "rcutils/allocator.h"
#include "rcutils/env.h"
#include "rcutils/error_handling.h"
#include "rcutils/shared_library.h"

class TestSharedLibrary : public ::testing::Test
{
protected:
  void SetUp() final
  {
    // Reset rcutil error global state in case a previously
    // running test has failed.
    rcutils_reset_error();
    lib = rcutils_get_zero_initialized_shared_library();
  }
  rcutils_shared_library_t lib;
  char library_path[1024];
};

TEST_F(TestSharedLibrary, basic_load) {
  rcutils_ret_t ret;

  // checking rcutils_get_zero_initialized_shared_library
  ASSERT_STRNE(lib.library_path, "");
  EXPECT_TRUE(lib.lib_pointer == NULL);
  EXPECT_FALSE(rcutils_is_shared_library_loaded(&lib));

  // Check debug name works first because rcutils_load_shared_library should be called on
  // non-debug symbol name
  ret = rcutils_get_platform_library_name(
    RCUTILS_STRINGIFY(SHARED_LIBRARY_UNDER_TEST), library_path, 1024, true);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_get_platform_library_name(
    RCUTILS_STRINGIFY(SHARED_LIBRARY_UNDER_TEST), library_path, 1024, false);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // getting shared library
  ret = rcutils_load_shared_library(&lib, library_path, rcutils_get_default_allocator());
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_TRUE(rcutils_is_shared_library_loaded(&lib));

  // unload shared_library
  ret = rcutils_unload_shared_library(&lib);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // checking if we have unloaded and freed memory
  ASSERT_STRNE(lib.library_path, "");
  EXPECT_TRUE(lib.lib_pointer == NULL);
}

TEST_F(TestSharedLibrary, bad_load) {
  rcutils_ret_t ret;
  ret = rcutils_get_platform_library_name("non_existing_library", library_path, 1024, false);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_load_shared_library(&lib, library_path, rcutils_get_default_allocator());
  ASSERT_EQ(RCUTILS_RET_ERROR, ret);
}

TEST_F(TestSharedLibrary, fail_allocator) {
  rcutils_ret_t ret;
  ret = rcutils_get_platform_library_name(
    RCUTILS_STRINGIFY(SHARED_LIBRARY_UNDER_TEST), library_path, 1024, false);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_allocator_t failing_allocator = get_failing_allocator();
  ret = rcutils_load_shared_library(&lib, library_path, failing_allocator);
  ASSERT_EQ(RCUTILS_RET_BAD_ALLOC, ret);
}

TEST_F(TestSharedLibrary, load_two_times) {
  rcutils_ret_t ret;

  ret = rcutils_get_platform_library_name(
    RCUTILS_STRINGIFY(SHARED_LIBRARY_UNDER_TEST), library_path, 1024, false);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // getting shared library
  ret = rcutils_load_shared_library(&lib, library_path, rcutils_get_default_allocator());
  EXPECT_EQ(RCUTILS_RET_OK, ret);

  // getting shared library
  rcutils_shared_library_t clone = rcutils_get_zero_initialized_shared_library();
  ret = rcutils_load_shared_library(&clone, library_path, rcutils_get_default_allocator());
  EXPECT_EQ(RCUTILS_RET_OK, ret);

  // unload shared_library
  ret = rcutils_unload_shared_library(&clone);
  EXPECT_EQ(RCUTILS_RET_OK, ret);

  // unload shared_library
  ret = rcutils_unload_shared_library(&lib);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
}

TEST_F(TestSharedLibrary, error_load) {
  rcutils_ret_t ret;

  ret = rcutils_load_shared_library(&lib, NULL, rcutils_get_default_allocator());
  ASSERT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcutils_get_platform_library_name(
    RCUTILS_STRINGIFY(SHARED_LIBRARY_UNDER_TEST), library_path, 1024, false);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_load_shared_library(
    &lib,
    library_path, rcutils_get_zero_initialized_allocator());
  ASSERT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcutils_load_shared_library(
    &lib, library_path, rcutils_get_default_allocator());
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_load_shared_library(
    &lib, library_path, rcutils_get_default_allocator());
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  ret = rcutils_unload_shared_library(&lib);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
}

TEST_F(TestSharedLibrary, error_unload) {
  rcutils_ret_t ret;

  ret = rcutils_get_platform_library_name(
    RCUTILS_STRINGIFY(SHARED_LIBRARY_UNDER_TEST), library_path, 1024, false);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_load_shared_library(&lib, library_path, rcutils_get_default_allocator());
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // unload shared_library
  ret = rcutils_unload_shared_library(&lib);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // unload again shared_library
  ret = rcutils_unload_shared_library(&lib);
  ASSERT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
}

TEST_F(TestSharedLibrary, error_symbol) {
  rcutils_ret_t ret;
  bool is_symbol = rcutils_has_symbol(&lib, "symbol");
  EXPECT_FALSE(is_symbol);

  void * symbol = rcutils_get_symbol(&lib, "print_name");
  EXPECT_TRUE(symbol == NULL);

  ret = rcutils_get_platform_library_name(
    RCUTILS_STRINGIFY(SHARED_LIBRARY_UNDER_TEST), library_path, 1024, false);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ret = rcutils_load_shared_library(&lib, library_path, rcutils_get_default_allocator());
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  is_symbol = rcutils_has_symbol(&lib, "symbol");
  EXPECT_FALSE(is_symbol);

  // unload shared_library
  ret = rcutils_unload_shared_library(&lib);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
}

TEST_F(TestSharedLibrary, basic_symbol) {
  void * symbol;
  bool ret;

  symbol = rcutils_get_symbol(nullptr, "symbol");
  EXPECT_TRUE(symbol == NULL);

  rcutils_reset_error();

  ret = rcutils_has_symbol(nullptr, "symbol");
  EXPECT_FALSE(ret);

  ret = rcutils_get_platform_library_name(
    RCUTILS_STRINGIFY(SHARED_LIBRARY_UNDER_TEST), library_path, 1024, false);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // getting shared library
  ret = rcutils_load_shared_library(&lib, library_path, rcutils_get_default_allocator());
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  symbol = rcutils_get_symbol(&lib, "symbol");
  EXPECT_TRUE(symbol == NULL);

  symbol = rcutils_get_symbol(&lib, "print_name");
  EXPECT_TRUE(symbol != NULL);

  ret = rcutils_has_symbol(&lib, "print_name");
  EXPECT_TRUE(ret);

  // unload shared_library
  ret = rcutils_unload_shared_library(&lib);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
}
