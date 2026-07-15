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

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcutils/logging.h"

TEST(test_logging_logger_is_enabled_for, test_logging_logger_is_enabled_for) {
  bool ret;
  ret = rcutils_logging_logger_is_enabled_for(
    "rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_DEBUG);
  ASSERT_FALSE(ret);

  rcutils_ret_t ret_set_severity;
  ret_set_severity =
    rcutils_logging_set_logger_level("rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_DEBUG);
  ASSERT_EQ(RCUTILS_RET_OK, ret_set_severity);
  ret = rcutils_logging_logger_is_enabled_for(
    "rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_DEBUG);
  ASSERT_TRUE(ret);

  ret_set_severity =
    rcutils_logging_set_logger_level("rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_INFO);
  ASSERT_EQ(RCUTILS_RET_OK, ret_set_severity);
  ret = rcutils_logging_logger_is_enabled_for(
    "rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_INFO);
  ASSERT_TRUE(ret);

  ret_set_severity =
    rcutils_logging_set_logger_level("rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_WARN);
  ASSERT_EQ(RCUTILS_RET_OK, ret_set_severity);
  ret = rcutils_logging_logger_is_enabled_for(
    "rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_WARN);
  ASSERT_TRUE(ret);

  ret_set_severity =
    rcutils_logging_set_logger_level("rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_ERROR);
  ASSERT_EQ(RCUTILS_RET_OK, ret_set_severity);
  ret = rcutils_logging_logger_is_enabled_for(
    "rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_ERROR);
  ASSERT_TRUE(ret);

  ret_set_severity =
    rcutils_logging_set_logger_level("rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_FATAL);
  ASSERT_EQ(RCUTILS_RET_OK, ret_set_severity);
  ret = rcutils_logging_logger_is_enabled_for(
    "rmw_fastrtps_cpp", RCUTILS_LOG_SEVERITY_FATAL);
  ASSERT_TRUE(ret);
}

TEST(test_logging_logger_is_enabled_for, test_logger_is_enabled_for) {
  EXPECT_FALSE(
    rcutils_logging_logger_is_enabled_for("rcutils_test_loggers", RCUTILS_LOG_SEVERITY_DEBUG));
  EXPECT_TRUE(
    rcutils_logging_logger_is_enabled_for("rcutils_test_loggers", RCUTILS_LOG_SEVERITY_FATAL));

  ASSERT_EQ(RCUTILS_RET_OK, rcutils_logging_initialize());
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCUTILS_RET_OK, rcutils_logging_shutdown());
  });

  EXPECT_TRUE(
    rcutils_logging_logger_is_enabled_for("rcutils_test_loggers", RCUTILS_LOG_SEVERITY_INFO));

  ASSERT_EQ(
    RCUTILS_RET_OK,
    rcutils_logging_set_logger_level(
      "rcutils_test_loggers", RCUTILS_LOG_SEVERITY_WARN));

  EXPECT_FALSE(
    rcutils_logging_logger_is_enabled_for("rcutils_test_loggers", RCUTILS_LOG_SEVERITY_INFO));

  EXPECT_FALSE(rcutils_logging_logger_is_enabled_for("rcutils_test_loggers", -1));
  EXPECT_TRUE(rcutils_logging_logger_is_enabled_for("rcutils_test_loggers", 1000));

  // These should all resort to default logging severity
  EXPECT_FALSE(rcutils_logging_logger_is_enabled_for("", RCUTILS_LOG_SEVERITY_DEBUG));
  EXPECT_TRUE(rcutils_logging_logger_is_enabled_for("", RCUTILS_LOG_SEVERITY_FATAL));

  EXPECT_FALSE(rcutils_logging_logger_is_enabled_for(NULL, RCUTILS_LOG_SEVERITY_DEBUG));
  EXPECT_TRUE(rcutils_logging_logger_is_enabled_for(NULL, RCUTILS_LOG_SEVERITY_FATAL));

  EXPECT_TRUE(
    rcutils_logging_logger_is_enabled_for("name_that_doesn't_exist", RCUTILS_LOG_SEVERITY_FATAL));
  EXPECT_FALSE(
    rcutils_logging_logger_is_enabled_for("name_that_doesn't_exist", RCUTILS_LOG_SEVERITY_DEBUG));

  EXPECT_TRUE(
    rcutils_logging_logger_is_enabled_for("name.that.doesn't.exist", RCUTILS_LOG_SEVERITY_FATAL));
  EXPECT_FALSE(
    rcutils_logging_logger_is_enabled_for("name.that.doesn't.exist", RCUTILS_LOG_SEVERITY_DEBUG));
}
