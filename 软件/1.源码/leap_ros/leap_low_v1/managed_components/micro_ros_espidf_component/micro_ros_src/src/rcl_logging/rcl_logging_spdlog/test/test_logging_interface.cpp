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

#include <limits.h>
#include <fstream>
#include <string>

#include "gmock/gmock.h"

#include "rcpputils/filesystem_helper.hpp"
#include "rcpputils/env.hpp"
#include "rcutils/allocator.h"
#include "rcutils/env.h"
#include "rcutils/error_handling.h"
#include "rcutils/logging.h"
#include "rcutils/testing/fault_injection.h"

#include "fixtures.hpp"
#include "rcl_logging_interface/rcl_logging_interface.h"

const int logger_levels[] =
{
  RCUTILS_LOG_SEVERITY_UNSET,
  RCUTILS_LOG_SEVERITY_DEBUG,
  RCUTILS_LOG_SEVERITY_INFO,
  RCUTILS_LOG_SEVERITY_WARN,
  RCUTILS_LOG_SEVERITY_ERROR,
  RCUTILS_LOG_SEVERITY_FATAL,
};

// This is a helper class that resets an environment
// variable when leaving scope
class RestoreEnvVar
{
public:
  explicit RestoreEnvVar(const std::string & name)
  : name_(name),
    value_(rcpputils::get_env_var(name.c_str()))
  {
  }

  ~RestoreEnvVar()
  {
    if (!rcutils_set_env(name_.c_str(), value_.c_str())) {
      std::cerr << "Failed to restore value of environment variable: " << name_ << std::endl;
    }
  }

private:
  const std::string name_;
  const std::string value_;
};

TEST_F(LoggingTest, init_invalid)
{
  // Config files are not supported by spdlog
  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_external_initialize("anything", allocator));
  rcutils_reset_error();
  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_external_initialize(nullptr, bad_allocator));
  rcutils_reset_error();
  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_external_initialize(nullptr, invalid_allocator));
  rcutils_reset_error();
}

TEST_F(LoggingTest, init_failure)
{
  RestoreEnvVar home_var("HOME");
  RestoreEnvVar userprofile_var("USERPROFILE");

  // No home directory to write log to
  ASSERT_EQ(true, rcutils_set_env("HOME", nullptr));
  ASSERT_EQ(true, rcutils_set_env("USERPROFILE", nullptr));
  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_external_initialize(nullptr, allocator));
  rcutils_reset_error();

  // Force failure to create directories
  rcpputils::fs::path fake_home = rcpputils::fs::current_path() / "fake_home_dir";
  ASSERT_TRUE(rcpputils::fs::create_directories(fake_home));
  ASSERT_EQ(true, rcutils_set_env("HOME", fake_home.string().c_str()));

  // ...fail to create .ros dir
  rcpputils::fs::path ros_dir = fake_home / ".ros";
  std::fstream(ros_dir.string(), std::ios_base::out).close();
  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_external_initialize(nullptr, allocator));
  ASSERT_TRUE(rcpputils::fs::remove(ros_dir));

  // ...fail to create .ros/log dir
  ASSERT_TRUE(rcpputils::fs::create_directories(ros_dir));
  rcpputils::fs::path ros_log_dir = ros_dir / "log";
  std::fstream(ros_log_dir.string(), std::ios_base::out).close();
  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_external_initialize(nullptr, allocator));
  ASSERT_TRUE(rcpputils::fs::remove(ros_log_dir));
  ASSERT_TRUE(rcpputils::fs::remove(ros_dir));

  ASSERT_TRUE(rcpputils::fs::remove(fake_home));
}

TEST_F(LoggingTest, init_old_flushing_behavior)
{
  RestoreEnvVar env_var("RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR");
  rcpputils::set_env_var("RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR", "1");

  ASSERT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, allocator));

  std::stringstream expected_log;
  for (int level : logger_levels) {
    EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_set_logger_level(nullptr, level));

    for (int severity : logger_levels) {
      std::stringstream ss;
      ss << "Message of severity " << severity << " at level " << level;
      rcl_logging_external_log(severity, nullptr, ss.str().c_str());

      if (severity >= level) {
        expected_log << ss.str() << std::endl;
      } else if (severity == 0 && level == 10) {
        // This is a special case - not sure what the right behavior is
        expected_log << ss.str() << std::endl;
      }
    }
  }

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());

  std::string log_file_path = find_single_log().string();
  std::ifstream log_file(log_file_path);
  std::stringstream actual_log;
  actual_log << log_file.rdbuf();
  EXPECT_EQ(
    expected_log.str(),
    actual_log.str()) << "Unexpected log contents in " << log_file_path;
}

TEST_F(LoggingTest, init_explicit_new_flush_behavior)
{
  RestoreEnvVar env_var("RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR");
  rcpputils::set_env_var("RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR", "0");

  ASSERT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, allocator));

  std::stringstream expected_log;
  for (int level : logger_levels) {
    EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_set_logger_level(nullptr, level));

    for (int severity : logger_levels) {
      std::stringstream ss;
      ss << "Message of severity " << severity << " at level " << level;
      rcl_logging_external_log(severity, nullptr, ss.str().c_str());

      if (severity >= level) {
        expected_log << ss.str() << std::endl;
      } else if (severity == 0 && level == 10) {
        // This is a special case - not sure what the right behavior is
        expected_log << ss.str() << std::endl;
      }
    }
  }

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());

  std::string log_file_path = find_single_log().string();
  std::ifstream log_file(log_file_path);
  std::stringstream actual_log;
  actual_log << log_file.rdbuf();
  EXPECT_EQ(
    expected_log.str(),
    actual_log.str()) << "Unexpected log contents in " << log_file_path;
}

TEST_F(LoggingTest, init_invalid_flush_setting)
{
  RestoreEnvVar env_var("RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR");
  rcpputils::set_env_var("RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR", "invalid");

  ASSERT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_external_initialize(nullptr, allocator));
  std::string error_state_str = rcutils_get_error_string().str;
  using ::testing::HasSubstr;
  ASSERT_THAT(
    error_state_str,
    HasSubstr("unrecognized value:"));
  rcutils_reset_error();
}

TEST_F(LoggingTest, full_cycle)
{
  ASSERT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, allocator));

  // Make sure we can call initialize more than once
  ASSERT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_initialize(nullptr, allocator));

  std::stringstream expected_log;
  for (int level : logger_levels) {
    EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_set_logger_level(nullptr, level));

    for (int severity : logger_levels) {
      std::stringstream ss;
      ss << "Message of severity " << severity << " at level " << level;
      rcl_logging_external_log(severity, nullptr, ss.str().c_str());

      if (severity >= level) {
        expected_log << ss.str() << std::endl;
      } else if (severity == 0 && level == 10) {
        // This is a special case - not sure what the right behavior is
        expected_log << ss.str() << std::endl;
      }
    }
  }

  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());

  std::string log_file_path = find_single_log().string();
  std::ifstream log_file(log_file_path);
  std::stringstream actual_log;
  actual_log << log_file.rdbuf();
  EXPECT_EQ(
    expected_log.str(),
    actual_log.str()) << "Unexpected log contents in " << log_file_path;
}

TEST_F(LoggingTest, init_fini_maybe_fail_test)
{
  RCUTILS_FAULT_INJECTION_TEST(
  {
    if (RCL_LOGGING_RET_OK == rcl_logging_external_initialize(nullptr, allocator)) {
      EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_external_shutdown());
    } else {
      EXPECT_TRUE(rcutils_error_is_set());
      rcutils_reset_error();
    }
  });
}
