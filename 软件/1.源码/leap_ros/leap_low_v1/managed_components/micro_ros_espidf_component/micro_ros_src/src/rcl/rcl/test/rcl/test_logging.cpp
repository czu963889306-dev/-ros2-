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
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcl/error_handling.h"
#include "rcl/logging.h"
#include "rcl/rcl.h"
#include "rcl/subscription.h"
#include "rcl_interfaces/msg/log.h"
#include "rcl_logging_interface/rcl_logging_interface.h"

#include "rcutils/logging_macros.h"

#include "../mocking_utils/patch.hpp"

// Define dummy comparison operators for rcutils_allocator_t type
// to use with the Mimick mocking library
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, ==)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, !=)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, <)
MOCKING_UTILS_BOOL_OPERATOR_RETURNS_FALSE(rcutils_allocator_t, >)

TEST(TestLogging, test_configure_with_bad_arguments) {
  rcl_allocator_t default_allocator = rcl_get_default_allocator();
  const char * argv[] = {"test_logging"};
  const int argc = sizeof(argv) / sizeof(argv[0]);
  rcl_arguments_t global_arguments = rcl_get_zero_initialized_arguments();
  ASSERT_EQ(RCL_RET_OK, rcl_parse_arguments(argc, argv, default_allocator, &global_arguments)) <<
    rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&global_arguments)) << rcl_get_error_string().str;
  });

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_logging_configure(nullptr, &default_allocator));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_logging_configure(&global_arguments, nullptr));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  rcl_allocator_t zero_initialized_allocator = rcutils_get_zero_initialized_allocator();
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_logging_configure(&global_arguments, &zero_initialized_allocator));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_logging_configure_with_output_handler(
      nullptr, &default_allocator, rcl_logging_multiple_output_handler));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_logging_configure_with_output_handler(
      &global_arguments, nullptr, rcl_logging_multiple_output_handler));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_logging_configure_with_output_handler(
      &global_arguments, &default_allocator, nullptr));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_logging_configure_with_output_handler(
      &global_arguments, &zero_initialized_allocator, rcl_logging_multiple_output_handler));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}

TEST(TestLogging, test_logging_rosout_enabled) {
  {
    const char * argv[] = {
      "test_logging", RCL_ROS_ARGS_FLAG, "--enable-" RCL_LOG_ROSOUT_FLAG_SUFFIX};
    const int argc = sizeof(argv) / sizeof(argv[0]);
    rcl_allocator_t default_allocator = rcl_get_default_allocator();
    rcl_arguments_t global_arguments = rcl_get_zero_initialized_arguments();
    ASSERT_EQ(RCL_RET_OK, rcl_parse_arguments(argc, argv, default_allocator, &global_arguments)) <<
      rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&global_arguments)) << rcl_get_error_string().str;
    });
    ASSERT_EQ(RCL_RET_OK, rcl_logging_configure(&global_arguments, &default_allocator)) <<
      rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_logging_fini()) << rcl_get_error_string().str;
    });

    ASSERT_TRUE(rcl_logging_rosout_enabled());
  }

  {
    const char * argv[] = {
      "test_logging", RCL_ROS_ARGS_FLAG, "--disable-" RCL_LOG_ROSOUT_FLAG_SUFFIX};
    const int argc = sizeof(argv) / sizeof(argv[0]);
    rcl_allocator_t default_allocator = rcl_get_default_allocator();
    rcl_arguments_t global_arguments = rcl_get_zero_initialized_arguments();
    ASSERT_EQ(RCL_RET_OK, rcl_parse_arguments(argc, argv, default_allocator, &global_arguments)) <<
      rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&global_arguments)) << rcl_get_error_string().str;
    });
    ASSERT_EQ(RCL_RET_OK, rcl_logging_configure(&global_arguments, &default_allocator)) <<
      rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_logging_fini()) << rcl_get_error_string().str;
    });

    ASSERT_FALSE(rcl_logging_rosout_enabled());
  }
}

TEST(TestLogging, test_failing_external_logging_configure) {
  const char * argv[] = {
    "test_logging", RCL_ROS_ARGS_FLAG,
    "--enable-" RCL_LOG_EXT_LIB_FLAG_SUFFIX};
  const int argc = sizeof(argv) / sizeof(argv[0]);
  rcl_allocator_t default_allocator = rcl_get_default_allocator();
  rcl_arguments_t global_arguments = rcl_get_zero_initialized_arguments();
  ASSERT_EQ(RCL_RET_OK, rcl_parse_arguments(argc, argv, default_allocator, &global_arguments)) <<
    rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&global_arguments)) << rcl_get_error_string().str;
  });

  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rcl_logging_external_initialize, "some error", RCL_LOGGING_RET_ERROR);
    EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_configure(&global_arguments, &default_allocator));
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();

    EXPECT_EQ(RCL_RET_OK, rcl_logging_fini()) << rcl_get_error_string().str;
  }

  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rcl_logging_external_set_logger_level, "some error", RCL_LOGGING_RET_ERROR);
    EXPECT_EQ(RCL_RET_ERROR, rcl_logging_configure(&global_arguments, &default_allocator));
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();

    EXPECT_EQ(RCL_RET_OK, rcl_logging_fini()) << rcl_get_error_string().str;
  }
}

TEST(TestLogging, test_failing_logger_level_configure) {
  const char * argv[] = {
    "test_logging", RCL_ROS_ARGS_FLAG,
    RCL_LOG_LEVEL_FLAG, ROS_PACKAGE_NAME ":=info"};
  const int argc = sizeof(argv) / sizeof(argv[0]);
  rcl_allocator_t default_allocator = rcl_get_default_allocator();
  rcl_arguments_t global_arguments = rcl_get_zero_initialized_arguments();
  ASSERT_EQ(RCL_RET_OK, rcl_parse_arguments(argc, argv, default_allocator, &global_arguments)) <<
    rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&global_arguments)) << rcl_get_error_string().str;
  });

  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rcutils_logging_set_logger_level, "failed to allocate", RCUTILS_RET_ERROR);
    EXPECT_EQ(RCL_RET_ERROR, rcl_logging_configure(&global_arguments, &default_allocator));
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();

    EXPECT_EQ(RCL_RET_OK, rcl_logging_fini()) << rcl_get_error_string().str;
  }
}

TEST(TestLogging, test_failing_external_logging) {
  const char * argv[] = {
    "test_logging", RCL_ROS_ARGS_FLAG,
    "--disable-" RCL_LOG_STDOUT_FLAG_SUFFIX,
    "--enable-" RCL_LOG_EXT_LIB_FLAG_SUFFIX,
    RCL_LOG_LEVEL_FLAG, ROS_PACKAGE_NAME ":=DEBUG"
  };
  const int argc = sizeof(argv) / sizeof(argv[0]);
  rcl_allocator_t default_allocator = rcl_get_default_allocator();
  rcl_arguments_t global_arguments = rcl_get_zero_initialized_arguments();
  ASSERT_EQ(RCL_RET_OK, rcl_parse_arguments(argc, argv, default_allocator, &global_arguments)) <<
    rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&global_arguments)) << rcl_get_error_string().str;
  });
  ASSERT_EQ(RCL_RET_OK, rcl_logging_configure(&global_arguments, &default_allocator)) <<
    rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_logging_fini()) << rcl_get_error_string().str;
  });

  bool log_seen = false;
  int severity_seen = RCUTILS_LOG_SEVERITY_UNSET;
  std::string logger_name_seen;
  std::string log_message_seen;
  auto log_mock = mocking_utils::patch(
    "lib:rcl", rcl_logging_external_log,
    [&](int severity, const char * name, const char * message) {
      severity_seen = severity;
      logger_name_seen = name;
      log_message_seen = message;
      log_seen = true;
    });

  constexpr char log_message[] = "Test message";
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, log_message);
  EXPECT_TRUE(log_seen);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, severity_seen);
  EXPECT_EQ(ROS_PACKAGE_NAME, logger_name_seen);
  EXPECT_TRUE(log_message_seen.find(log_message) != std::string::npos) <<
    "Expected '" << log_message << "' within '" << log_message_seen << "'";

  std::stringstream stderr_sstream;
  auto fwrite_mock = mocking_utils::patch(
    "lib:rcl", fwrite,
    [&](const void * ptr, size_t size, size_t count, FILE * stream)
    {
      if (sizeof(char) == size && stderr == stream) {
        stderr_sstream << std::string(reinterpret_cast<const char *>(ptr), count);
      }
      return count;
    });

  constexpr char stderr_message[] = "internal error";
#ifdef MOCKING_UTILS_SUPPORT_VA_LIST
  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rcutils_char_array_vsprintf,
      stderr_message, RCUTILS_RET_ERROR);

    log_seen = false;
    RCUTILS_LOG_INFO_NAMED(ROS_PACKAGE_NAME, log_message);
    EXPECT_TRUE(log_seen);
    EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, severity_seen);
    EXPECT_EQ(ROS_PACKAGE_NAME, logger_name_seen);
    EXPECT_TRUE(stderr_sstream.str().find(stderr_message) != std::string::npos) <<
      "Expected '" << stderr_message << "' within '" << stderr_sstream.str() << "'";
    stderr_sstream.str("");
  }
#endif

  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rcutils_logging_format_message,
      stderr_message, RCUTILS_RET_ERROR);

    log_seen = false;
    RCUTILS_LOG_WARN_NAMED(ROS_PACKAGE_NAME, log_message);
    EXPECT_TRUE(log_seen);
    EXPECT_EQ(RCUTILS_LOG_SEVERITY_WARN, severity_seen);
    EXPECT_EQ(ROS_PACKAGE_NAME, logger_name_seen);
    EXPECT_TRUE(stderr_sstream.str().find(stderr_message) != std::string::npos) <<
      "Expected '" << stderr_message << "' within '" << stderr_sstream.str() << "'";
    stderr_sstream.str("");
  }

  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rcutils_char_array_fini,
      stderr_message, RCUTILS_RET_ERROR);

    log_seen = false;
    RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, log_message);
    EXPECT_TRUE(log_seen);
    EXPECT_EQ(RCUTILS_LOG_SEVERITY_ERROR, severity_seen);
    EXPECT_EQ(ROS_PACKAGE_NAME, logger_name_seen);
    EXPECT_TRUE(log_message_seen.find(log_message) != std::string::npos) <<
      "Expected '" << log_message << "' within '" << log_message_seen << "'";
    EXPECT_TRUE(stderr_sstream.str().find(stderr_message) != std::string::npos) <<
      "Expected '" << stderr_message << "' within '" << stderr_sstream.str() << "'";
    stderr_sstream.str("");
  }
}
