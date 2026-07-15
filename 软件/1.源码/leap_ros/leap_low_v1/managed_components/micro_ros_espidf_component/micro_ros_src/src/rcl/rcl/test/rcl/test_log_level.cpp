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

#include "rcl/allocator.h"
#include "rcl/rcl.h"
#include "rcl/log_level.h"
#include "rcl/error_handling.h"
#include "rcutils/logging.h"

#include "./arg_macros.hpp"
#include "./allocator_testing_utils.h"
#include "../mocking_utils/patch.hpp"

int setup_and_parse_log_level_args(const char * log_level_string)
{
  rcl_arguments_t local_arguments = rcl_get_zero_initialized_arguments();
  const char * local_argv[] = {"process_name", "--ros-args", "--log-level", log_level_string};
  unsigned int local_argc = (sizeof(local_argv) / sizeof(const char *));
  return rcl_parse_arguments(
    local_argc, local_argv, rcl_get_default_allocator(), &local_arguments);
}

TEST(TestLogLevel, error_log_level) {
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args(":=debug"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("debug,"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl:=debug,"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl:=debug,,"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl:="));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl:=,"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args(":"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args(":="));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl:"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl="));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl=debug"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl:=:="));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl=debug,"));
  rcl_reset_error();
  ASSERT_EQ(RCL_RET_INVALID_ROS_ARGS, setup_and_parse_log_level_args("rcl:,"));
  rcl_reset_error();
}

#define GET_LOG_LEVEL_FROM_ARGUMENTS(log_levels, ...) \
  { \
    rcl_ret_t ret; \
    rcl_arguments_t local_arguments = rcl_get_zero_initialized_arguments(); \
    const char * local_argv[] = {__VA_ARGS__}; \
    unsigned int local_argc = (sizeof(local_argv) / sizeof(const char *)); \
    ret = rcl_parse_arguments( \
      local_argc, local_argv, rcl_get_default_allocator(), &local_arguments); \
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
    ret = rcl_arguments_get_log_levels(&local_arguments, &log_levels); \
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT( \
    { \
      ASSERT_EQ(RCL_RET_OK, rcl_arguments_fini(&local_arguments)); \
    }); \
  }

TEST(TestLogLevel, no_log_level) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(log_levels, "process_name");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(0ul, log_levels.num_logger_settings);
}

TEST(TestLogLevel, default_log_level) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.default_logger_level);
  EXPECT_EQ(0ul, log_levels.num_logger_settings);
}

TEST(TestLogLevel, logger_log_level_debug) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "rcl:=debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(1ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.logger_settings[0].level);
}

TEST(TestLogLevel, logger_log_level_info) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "rcl:=info");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(1ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, log_levels.logger_settings[0].level);
}

TEST(TestLogLevel, multiple_log_level_with_default_at_front) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "debug", "--log-level", "rcl:=debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.default_logger_level);
  EXPECT_EQ(1ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.logger_settings[0].level);
}

TEST(TestLogLevel, multiple_log_level_with_default_at_back) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "rcl:=debug", "--log-level", "debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.default_logger_level);
  EXPECT_EQ(1ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.logger_settings[0].level);
}

TEST(TestLogLevel, multiple_log_level_rightmost_prevail) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "debug", "--log-level", "info",
    "--log-level", "rcl:=debug", "--log-level", "rcl:=info");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, log_levels.default_logger_level);
  EXPECT_EQ(1ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, log_levels.logger_settings[0].level);
}

TEST(TestLogLevel, multiple_log_level_names) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "debug", "--log-level", "rcl:=debug",
    "--log-level", "test:=info");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.default_logger_level);
  EXPECT_EQ(2ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.logger_settings[0].level);
  EXPECT_STREQ("test", log_levels.logger_settings[1].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, log_levels.logger_settings[1].level);
}

TEST(TestLogLevel, log_level_dot_logger_name) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "test.abc:=info");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(1ul, log_levels.num_logger_settings);
  EXPECT_STREQ("test.abc", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, log_levels.logger_settings[0].level);
}

TEST(TestLogLevel, log_level_init_fini) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  rcl_allocator_t allocator = rcl_get_default_allocator();

  // Test zero size ini/fini
  const size_t zero_count = 0;
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_log_levels_init(&log_levels, &allocator, zero_count));
  EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));

  const size_t capacity_count = 1;
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_log_levels_init(&log_levels, &allocator, capacity_count));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_log_levels_init(nullptr, &allocator, capacity_count));
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_log_levels_init(&log_levels, nullptr, capacity_count));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_log_levels_init(&log_levels, &allocator, capacity_count));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  rcl_allocator_t bad_allocator = get_failing_allocator();
  rcl_log_levels_t empty_log_levels = rcl_get_zero_initialized_log_levels();
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC, rcl_log_levels_init(&empty_log_levels, &bad_allocator, capacity_count));

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_log_levels_fini(nullptr));
}

TEST(TestLogLevel, logger_log_level_copy) {
  // Init to debug level to test before copy
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  GET_LOG_LEVEL_FROM_ARGUMENTS(
    log_levels, "process_name", "--ros-args",
    "--log-level", "rcl:=debug");
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(1ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.logger_settings[0].level);

  // Expected usage
  rcl_log_levels_t copied_log_levels = rcl_get_zero_initialized_log_levels();
  EXPECT_EQ(RCL_RET_OK, rcl_log_levels_copy(&log_levels, &copied_log_levels));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&copied_log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, copied_log_levels.default_logger_level);
  EXPECT_EQ(log_levels.default_logger_level, copied_log_levels.default_logger_level);
  EXPECT_EQ(1ul, copied_log_levels.num_logger_settings);
  EXPECT_EQ(log_levels.num_logger_settings, copied_log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", copied_log_levels.logger_settings[0].name);
  EXPECT_STREQ(log_levels.logger_settings[0].name, copied_log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, copied_log_levels.logger_settings[0].level);
  EXPECT_EQ(log_levels.logger_settings[0].level, copied_log_levels.logger_settings[0].level);

  // Bad usage
  rcl_log_levels_t empty_log_levels = rcl_get_zero_initialized_log_levels();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_log_levels_copy(nullptr, &empty_log_levels));
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_log_levels_copy(&log_levels, nullptr));
  // Already copied
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_log_levels_copy(&log_levels, &copied_log_levels));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // null alloc
  rcl_allocator_t saved_allocator = log_levels.allocator;
  log_levels.allocator = {NULL, NULL, NULL, NULL, NULL};
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_log_levels_copy(&log_levels, &empty_log_levels));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // bad allocation
  rcl_allocator_t bad_allocator = get_failing_allocator();
  log_levels.allocator = bad_allocator;
  EXPECT_EQ(RCL_RET_BAD_ALLOC, rcl_log_levels_copy(&log_levels, &empty_log_levels));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  log_levels.allocator = saved_allocator;
}

TEST(TestLogLevel, test_add_logger_setting) {
  rcl_log_levels_t log_levels = rcl_get_zero_initialized_log_levels();
  rcl_allocator_t allocator = rcl_get_default_allocator();
  size_t logger_count = 2u;
  EXPECT_EQ(
    RCL_RET_OK, rcl_log_levels_init(&log_levels, &allocator, logger_count));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_log_levels_fini(&log_levels));
  });
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(0ul, log_levels.num_logger_settings);

  // Invalid arguments
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_log_levels_add_logger_setting(nullptr, "rcl", RCUTILS_LOG_SEVERITY_DEBUG));

  rcl_log_levels_t not_ini_log_levels = rcl_get_zero_initialized_log_levels();
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_log_levels_add_logger_setting(&not_ini_log_levels, "rcl", RCUTILS_LOG_SEVERITY_DEBUG));

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_log_levels_add_logger_setting(&log_levels, nullptr, RCUTILS_LOG_SEVERITY_DEBUG));

  rcl_allocator_t saved_allocator = log_levels.allocator;
  log_levels.allocator = {NULL, NULL, NULL, NULL, NULL};
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_log_levels_add_logger_setting(&log_levels, "rcl", RCUTILS_LOG_SEVERITY_DEBUG));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  rcl_allocator_t bad_allocator = get_failing_allocator();
  log_levels.allocator = bad_allocator;
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC,
    rcl_log_levels_add_logger_setting(&log_levels, "rcl", RCUTILS_LOG_SEVERITY_DEBUG));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  log_levels.allocator = saved_allocator;

  // Expected usage
  EXPECT_EQ(
    RCL_RET_OK, rcl_log_levels_add_logger_setting(&log_levels, "rcl", RCUTILS_LOG_SEVERITY_DEBUG));
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(1ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.logger_settings[0].level);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_log_levels_add_logger_setting(&log_levels, "rcutils", RCUTILS_LOG_SEVERITY_INFO));
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(2ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, log_levels.logger_settings[0].level);
  EXPECT_STREQ("rcutils", log_levels.logger_settings[1].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, log_levels.logger_settings[1].level);

  // Can't add more than logger_count
  EXPECT_EQ(
    RCL_RET_ERROR,
    rcl_log_levels_add_logger_setting(&log_levels, "rmw", RCUTILS_LOG_SEVERITY_DEBUG));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  EXPECT_EQ(2ul, log_levels.num_logger_settings);

  // Replacing saved logger
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_log_levels_add_logger_setting(&log_levels, "rcl", RCUTILS_LOG_SEVERITY_INFO));
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_UNSET, log_levels.default_logger_level);
  EXPECT_EQ(2ul, log_levels.num_logger_settings);
  EXPECT_STREQ("rcl", log_levels.logger_settings[0].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, log_levels.logger_settings[0].level);
  EXPECT_STREQ("rcutils", log_levels.logger_settings[1].name);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, log_levels.logger_settings[1].level);
}

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  // We need to do rcutils_logging_shutdown() since the tests above may
  // have initialized logging.
  if (rcutils_logging_shutdown() != RCUTILS_RET_OK) {
    fprintf(stderr, "Failed shutting down rcutils logging\n");
  }
  return ret;
}
