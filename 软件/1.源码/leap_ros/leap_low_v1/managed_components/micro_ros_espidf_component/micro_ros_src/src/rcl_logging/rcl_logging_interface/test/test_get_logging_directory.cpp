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

#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "rcpputils/filesystem_helper.hpp"
#include "rcpputils/env.hpp"
#include "rcutils/allocator.h"
#include "rcutils/env.h"
#include "rcutils/error_handling.h"

#include "rcl_logging_interface/rcl_logging_interface.h"

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

TEST(test_logging_directory, directory)
{
  RestoreEnvVar home_var("HOME");
  RestoreEnvVar userprofile_var("USERPROFILE");
  ASSERT_EQ(true, rcutils_set_env("HOME", nullptr));
  ASSERT_EQ(true, rcutils_set_env("USERPROFILE", nullptr));
  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", nullptr));
  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", nullptr));

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  // Invalid argument if given a nullptr
  EXPECT_EQ(
    RCL_LOGGING_RET_INVALID_ARGUMENT, rcl_logging_get_logging_directory(allocator, nullptr));
  EXPECT_TRUE(rcutils_error_is_set());
  rcutils_reset_error();
  // Invalid argument if the C string is not nullptr
  char * could_leak = const_cast<char *>("/could/be/leaked");
  EXPECT_EQ(
    RCL_LOGGING_RET_INVALID_ARGUMENT, rcl_logging_get_logging_directory(allocator, &could_leak));
  EXPECT_TRUE(rcutils_error_is_set());
  rcutils_reset_error();

  // Fails without any relevant env vars at all (HOME included)
  char * directory = nullptr;
  EXPECT_EQ(RCL_LOGGING_RET_ERROR, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_TRUE(rcutils_error_is_set());
  rcutils_reset_error();
  directory = nullptr;

  // Default case without ROS_LOG_DIR or ROS_HOME being set (but with HOME)
  rcpputils::fs::path fake_home("/fake_home_dir");
  ASSERT_EQ(true, rcutils_set_env("HOME", fake_home.string().c_str()));
  ASSERT_EQ(true, rcutils_set_env("USERPROFILE", fake_home.string().c_str()));
  rcpputils::fs::path default_dir = fake_home / ".ros" / "log";
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, default_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;

  // Use $ROS_LOG_DIR if it is set
  const char * my_log_dir_raw = "/my/ros_log_dir";
  rcpputils::fs::path my_log_dir(my_log_dir_raw);
  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", my_log_dir.string().c_str()));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, my_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  // Make sure it converts path separators when necessary
  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", my_log_dir_raw));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, my_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  // Setting ROS_HOME won't change anything since ROS_LOG_DIR is used first
  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", "/this/wont/be/used"));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, my_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", nullptr));
  // Empty is considered unset
  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", ""));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, default_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  // Make sure '~' is expanded to the home directory
  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", "~/logdir"));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  rcpputils::fs::path fake_log_dir = fake_home / "logdir";
  EXPECT_STREQ(directory, fake_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  // But it should only be expanded if it's at the beginning
  rcpputils::fs::path prefixed_fake_log_dir("/prefix/~/logdir");
  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", prefixed_fake_log_dir.string().c_str()));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, prefixed_fake_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", "~"));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, fake_home.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  rcpputils::fs::path home_trailing_slash(fake_home.string() + "/");
  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", "~/"));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, home_trailing_slash.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;

  ASSERT_EQ(true, rcutils_set_env("ROS_LOG_DIR", nullptr));

  // Without ROS_LOG_DIR, use $ROS_HOME/log
  rcpputils::fs::path fake_ros_home = fake_home / ".fakeroshome";
  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", fake_ros_home.string().c_str()));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  rcpputils::fs::path fake_ros_home_log_dir = fake_ros_home / "log";
  EXPECT_STREQ(directory, fake_ros_home_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  // Make sure it converts path separators when necessary
  const char * my_ros_home_raw = "/my/ros/home";
  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", my_ros_home_raw));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  rcpputils::fs::path my_ros_home_log_dir = rcpputils::fs::path(my_ros_home_raw) / "log";
  EXPECT_STREQ(directory, my_ros_home_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  // Empty is considered unset
  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", ""));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, default_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  // Make sure '~' is expanded to the home directory
  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", "~/.fakeroshome"));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, fake_ros_home_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;
  // But it should only be expanded if it's at the beginning
  rcpputils::fs::path prefixed_fake_ros_home("/prefix/~/.fakeroshome");
  rcpputils::fs::path prefixed_fake_ros_home_log_dir = prefixed_fake_ros_home / "log";
  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", prefixed_fake_ros_home.string().c_str()));
  EXPECT_EQ(RCL_LOGGING_RET_OK, rcl_logging_get_logging_directory(allocator, &directory));
  EXPECT_STREQ(directory, prefixed_fake_ros_home_log_dir.string().c_str());
  allocator.deallocate(directory, allocator.state);
  directory = nullptr;

  ASSERT_EQ(true, rcutils_set_env("ROS_HOME", nullptr));
}
