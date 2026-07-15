// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__ARG_MACROS_HPP_
#define RCL__ARG_MACROS_HPP_

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcl/error_handling.h"
#include "rcl/rcl.h"
#include "rcutils/strdup.h"

/// Helper to get around non-const args passed to rcl_init().
char **
copy_args(int argc, const char ** args)
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  char ** copy = static_cast<char **>(allocator.allocate(sizeof(char *) * argc, allocator.state));
  for (int i = 0; i < argc; ++i) {
    copy[i] = rcutils_strdup(args[i], allocator);
  }
  return copy;
}

/// Destroy args allocated by copy_args.
void
destroy_args(int argc, char ** args)
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  for (int i = 0; i < argc; ++i) {
    allocator.deallocate(args[i], allocator.state);
  }
  allocator.deallocate(args, allocator.state);
}

#define SCOPE_GLOBAL_ARGS(argc, argv, ...) \
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options(); \
  ASSERT_EQ(RCL_RET_OK, rcl_init_options_init(&init_options, rcl_get_default_allocator())) \
    << rcl_get_error_string().str; \
  rcl_context_t context = rcl_get_zero_initialized_context(); \
  { \
    const char * const_argv[] = {__VA_ARGS__}; \
    argc = (sizeof(const_argv) / sizeof(const char *)); \
    argv = copy_args(argc, const_argv); \
    rcl_ret_t ret = rcl_init(argc, argv, &init_options, &context); \
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
  } \
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT( \
  { \
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str; \
    destroy_args(argc, argv); \
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context)) << rcl_get_error_string().str; \
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context)) << rcl_get_error_string().str; \
  })

#define SCOPE_ARGS(local_arguments, ...) \
  { \
    local_arguments = rcl_get_zero_initialized_arguments(); \
    const char * local_argv[] = {__VA_ARGS__}; \
    unsigned int local_argc = (sizeof(local_argv) / sizeof(const char *)); \
    rcl_ret_t ret = rcl_parse_arguments( \
      local_argc, local_argv, rcl_get_default_allocator(), &local_arguments); \
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
  } \
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT( \
  { \
    ASSERT_EQ(RCL_RET_OK, rcl_arguments_fini(&local_arguments)); \
  })

#endif  // RCL__ARG_MACROS_HPP_
