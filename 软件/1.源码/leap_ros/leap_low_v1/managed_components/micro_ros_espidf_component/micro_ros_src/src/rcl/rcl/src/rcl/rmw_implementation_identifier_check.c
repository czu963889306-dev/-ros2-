// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rcl/allocator.h"
#include "rcl/error_handling.h"
#include "rcutils/env.h"
#include "rcutils/logging_macros.h"
#include "rcutils/strdup.h"
#include "rmw/rmw.h"

#include "rcl/types.h"

#include "rcl/rmw_implementation_identifier_check.h"

// Extracted this portable method of doing a "shared library constructor" from SO:
//   http://stackoverflow.com/a/2390626/671658
// Initializer/finalizer sample for MSVC and GCC/Clang.
// 2010-2016 Joe Lowe. Released into the public domain.
#if defined(_MSC_VER)
  #pragma section(".CRT$XCU", read)
  #define INITIALIZER2_(f, p) \
  static void f(void); \
  __declspec(allocate(".CRT$XCU"))void(*f ## _)(void) = f; \
  __pragma(comment(linker, "/include:" p #f "_")) \
  static void f(void)
  #ifdef _WIN64
    #define INITIALIZER(f) INITIALIZER2_(f, "")
  #else
    #define INITIALIZER(f) INITIALIZER2_(f, "_")
  #endif
#else
  #define INITIALIZER(f) \
  static void f(void) __attribute__((constructor)); \
  static void f(void)
#endif

rcl_ret_t rcl_rmw_implementation_identifier_check(void)
{
  // If the environment variable RMW_IMPLEMENTATION is set, or
  // the environment variable RCL_ASSERT_RMW_ID_MATCHES is set,
  // check that the result of `rmw_get_implementation_identifier` matches.
  rcl_ret_t ret = RCL_RET_OK;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  char * expected_rmw_impl = NULL;
  const char * expected_rmw_impl_env = NULL;
  const char * get_env_error_str = rcutils_get_env(
    RMW_IMPLEMENTATION_ENV_VAR_NAME,
    &expected_rmw_impl_env);
  if (NULL != get_env_error_str) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Error getting env var '" RCUTILS_STRINGIFY(RMW_IMPLEMENTATION_ENV_VAR_NAME) "': %s\n",
      get_env_error_str);
    return RCL_RET_ERROR;
  }
  if (strlen(expected_rmw_impl_env) > 0) {
    // Copy the environment variable so it doesn't get over-written by the next getenv call.
    expected_rmw_impl = rcutils_strdup(expected_rmw_impl_env, allocator);
    if (!expected_rmw_impl) {
      RCL_SET_ERROR_MSG("allocation failed");
      return RCL_RET_BAD_ALLOC;
    }
  }

  char * asserted_rmw_impl = NULL;
  const char * asserted_rmw_impl_env = NULL;
  get_env_error_str = rcutils_get_env(
    RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME, &asserted_rmw_impl_env);
  if (NULL != get_env_error_str) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Error getting env var '"
      RCUTILS_STRINGIFY(RCL_ASSERT_RMW_ID_MATCHES_ENV_VAR_NAME) "': %s\n",
      get_env_error_str);
    ret = RCL_RET_ERROR;
    goto cleanup;
  }
  if (strlen(asserted_rmw_impl_env) > 0) {
    // Copy the environment variable so it doesn't get over-written by the next getenv call.
    asserted_rmw_impl = rcutils_strdup(asserted_rmw_impl_env, allocator);
    if (!asserted_rmw_impl) {
      RCL_SET_ERROR_MSG("allocation failed");
      ret = RCL_RET_BAD_ALLOC;
      goto cleanup;
    }
  }

  // If both environment variables are set, and they do not match, print an error and exit.
  if (expected_rmw_impl && asserted_rmw_impl && strcmp(expected_rmw_impl, asserted_rmw_impl) != 0) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Values of RMW_IMPLEMENTATION ('%s') and RCL_ASSERT_RMW_ID_MATCHES ('%s') environment "
      "variables do not match, exiting with %d.",
      expected_rmw_impl, asserted_rmw_impl, RCL_RET_ERROR
    );
    ret = RCL_RET_ERROR;
    goto cleanup;
  }

  // Collapse the expected_rmw_impl and asserted_rmw_impl variables so only expected_rmw_impl needs
  // to be used from now on.
  if (expected_rmw_impl && asserted_rmw_impl) {
    // The strings at this point must be equal.
    // No need for asserted_rmw_impl anymore, free the memory.
    allocator.deallocate(asserted_rmw_impl, allocator.state);
    asserted_rmw_impl = NULL;
  } else {
    // One or none are set.
    // If asserted_rmw_impl has contents, move it over to expected_rmw_impl.
    if (asserted_rmw_impl) {
      expected_rmw_impl = asserted_rmw_impl;
      asserted_rmw_impl = NULL;
    }
  }

  // If either environment variable is set, and it does not match, print an error and exit.
  if (expected_rmw_impl) {
    const char * actual_rmw_impl_id = rmw_get_implementation_identifier();
    const rcutils_error_string_t rmw_error_msg = rcl_get_error_string();
    rcl_reset_error();
    if (!actual_rmw_impl_id) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Error getting RMW implementation identifier / RMW implementation not installed "
        "(expected identifier of '%s'), with error message '%s', exiting with %d.",
        expected_rmw_impl,
        rmw_error_msg.str,
        RCL_RET_ERROR
      );
      ret = RCL_RET_ERROR;
      goto cleanup;
    }
    if (strcmp(actual_rmw_impl_id, expected_rmw_impl) != 0) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Expected RMW implementation identifier of '%s' but instead found '%s', exiting with %d.",
        expected_rmw_impl,
        actual_rmw_impl_id,
        RCL_RET_MISMATCHED_RMW_ID
      );
      ret = RCL_RET_MISMATCHED_RMW_ID;
      goto cleanup;
    }
  }
  ret = RCL_RET_OK;
// fallthrough
cleanup:
  allocator.deallocate(expected_rmw_impl, allocator.state);
  allocator.deallocate(asserted_rmw_impl, allocator.state);
  return ret;
}

INITIALIZER(initialize) {
  rcl_ret_t ret = rcl_rmw_implementation_identifier_check();
  if (ret != RCL_RET_OK) {
    RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "%s\n", rcl_get_error_string().str);
    exit(ret);
  }
}

#ifdef __cplusplus
}
#endif
