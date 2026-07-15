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

#include <gmock/gmock.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "rcutils/logging_macros.h"
#include "rcutils/time.h"

using ::testing::EndsWith;

size_t g_log_calls = 0;

struct LogEvent
{
  const rcutils_log_location_t * location;
  int level;
  std::string name;
  rcutils_time_point_value_t timestamp;
  std::string message;
};
LogEvent g_last_log_event;

class TestLoggingMacros : public ::testing::Test
{
public:
  void SetUp()
  {
    g_log_calls = 0;
    g_last_log_event.message = "SetUp";
    EXPECT_FALSE(g_rcutils_logging_initialized);
    ASSERT_EQ(RCUTILS_RET_OK, rcutils_logging_initialize());
    EXPECT_TRUE(g_rcutils_logging_initialized);
    g_rcutils_logging_default_logger_level = RCUTILS_LOG_SEVERITY_DEBUG;
    EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, g_rcutils_logging_default_logger_level);

    auto rcutils_logging_console_output_handler = [](
      const rcutils_log_location_t * location,
      int level, const char * name, rcutils_time_point_value_t timestamp,
      const char * format, va_list * args) -> void
      {
        g_log_calls += 1;
        g_last_log_event.location = location;
        g_last_log_event.level = level;
        g_last_log_event.name = name ? name : "";
        g_last_log_event.timestamp = timestamp;
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, *args);
        g_last_log_event.message = buffer;
      };
    rcutils_logging_set_output_handler(rcutils_logging_console_output_handler);
  }

  void TearDown()
  {
    EXPECT_EQ(RCUTILS_RET_OK, rcutils_logging_shutdown());
  }
};

TEST_F(TestLoggingMacros, test_logging_named) {
  for (int i : {1, 2, 3}) {
    RCUTILS_LOG_DEBUG_NAMED("name", "message %d", i);
  }
  EXPECT_EQ(3u, g_log_calls);
  EXPECT_TRUE(g_last_log_event.location != NULL);
  if (g_last_log_event.location) {
    EXPECT_STREQ("TestBody", g_last_log_event.location->function_name);
    EXPECT_THAT(g_last_log_event.location->file_name, EndsWith("test_logging_macros.cpp"));
    EXPECT_GT(g_last_log_event.location->line_number, 1u);
  }
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_DEBUG, g_last_log_event.level);
  EXPECT_EQ("name", g_last_log_event.name);
  EXPECT_EQ("message 3", g_last_log_event.message);
}

TEST_F(TestLoggingMacros, test_logging_once) {
  for (int i : {1, 2, 3}) {
    RCUTILS_LOG_INFO_ONCE("message %d", i);
  }
  EXPECT_EQ(1u, g_log_calls);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_INFO, g_last_log_event.level);
  EXPECT_EQ("", g_last_log_event.name);
  EXPECT_EQ("message 1", g_last_log_event.message);
}

TEST_F(TestLoggingMacros, test_logging_expression) {
  for (int i : {1, 2, 3, 4, 5, 6}) {
    RCUTILS_LOG_INFO_EXPRESSION(i % 3, "message %d", i);
  }
  EXPECT_EQ(4u, g_log_calls);
  EXPECT_EQ("message 5", g_last_log_event.message);
}

int g_counter = 0;
bool g_function_called = false;

bool not_divisible_by_three()
{
  g_function_called = true;
  return (g_counter % 3) != 0;
}

TEST_F(TestLoggingMacros, test_logging_function) {
  g_counter = 0;
  g_function_called = false;

  // check that evaluation of a specified function does not occur if the severity is not enabled
  g_rcutils_logging_default_logger_level = RCUTILS_LOG_SEVERITY_INFO;
  for (int i : {0, 1}) {  // cover both true and false return values
    g_counter = i;
    RCUTILS_LOG_DEBUG_FUNCTION(&not_divisible_by_three, "message %d", i);
  }
  EXPECT_EQ(0u, g_log_calls);
  EXPECT_FALSE(g_function_called);
  g_rcutils_logging_default_logger_level = RCUTILS_LOG_SEVERITY_DEBUG;

  for (int i : {1, 2, 3, 4, 5, 6}) {
    g_counter = i;
    RCUTILS_LOG_INFO_FUNCTION(&not_divisible_by_three, "message %d", i);
  }
  EXPECT_EQ(4u, g_log_calls);
  EXPECT_TRUE(g_function_called);
  EXPECT_EQ("message 5", g_last_log_event.message);
}

TEST_F(TestLoggingMacros, test_logging_skipfirst) {
  for (uint32_t i : {1u, 2u, 3u, 4u, 5u}) {
    RCUTILS_LOG_WARN_SKIPFIRST("message %u", i);
    EXPECT_EQ(i - 1, g_log_calls);
  }
}

TEST_F(TestLoggingMacros, test_logging_throttle) {
  using namespace std::chrono_literals;
  const auto start = std::chrono::system_clock::now();
  const auto throttle_time = 200ms;
  bool first = true;
  while (true) {
    auto is_before_throttle_end =
      ((std::chrono::system_clock::now() - start) < throttle_time);
    RCUTILS_LOG_ERROR_THROTTLE(
      RCUTILS_STEADY_TIME, throttle_time.count(), first ? "first" : "other");
    first = false;
    auto is_still_before_throttle_end =
      ((std::chrono::system_clock::now() - start) < throttle_time);
    // If the other LOG call happens around the timeout, we don't know
    // know if it got throttled or not, so we will loop one more time
    if (is_before_throttle_end == is_still_before_throttle_end) {
      if (is_before_throttle_end) {
        EXPECT_EQ("first", g_last_log_event.message);
        EXPECT_EQ(g_log_calls, 1u);
      } else {
        break;
      }
    }
    std::this_thread::sleep_for(5ms);
  }
  EXPECT_EQ("other", g_last_log_event.message);
  EXPECT_EQ(g_log_calls, 2u);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_ERROR, g_last_log_event.level);
  EXPECT_EQ("", g_last_log_event.name);
}

TEST_F(TestLoggingMacros, test_logging_skipfirst_throttle) {
  using namespace std::chrono_literals;
  const auto start = std::chrono::system_clock::now();
  const auto throttle_time = 200ms;
  bool first = true;
  while (true) {
    auto is_before_throttle_end =
      ((std::chrono::system_clock::now() - start) < throttle_time);
    RCUTILS_LOG_FATAL_SKIPFIRST_THROTTLE(
      RCUTILS_STEADY_TIME, throttle_time.count(), first ? "first" : "other");
    first = false;
    auto is_still_before_throttle_end =
      ((std::chrono::system_clock::now() - start) < throttle_time);
    // If the other LOG call happens around the timeout, we don't know
    // know if it got throttled or not, so we will loop one more time
    if (is_before_throttle_end == is_still_before_throttle_end) {
      if (is_before_throttle_end) {
        EXPECT_EQ("SetUp", g_last_log_event.message);
        EXPECT_EQ(g_log_calls, 0u);
      } else {
        break;
      }
    }
    std::this_thread::sleep_for(5ms);
  }
  EXPECT_EQ("other", g_last_log_event.message);
  EXPECT_EQ(g_log_calls, 1u);
  EXPECT_EQ(RCUTILS_LOG_SEVERITY_FATAL, g_last_log_event.level);
  EXPECT_EQ("", g_last_log_event.name);
}

TEST_F(TestLoggingMacros, test_logger_hierarchy) {
  ASSERT_EQ(
    RCUTILS_RET_OK,
    rcutils_logging_set_logger_level(
      "rcutils_test_logging_macros_cpp", RCUTILS_LOG_SEVERITY_WARN));
  RCUTILS_LOG_INFO_NAMED("rcutils_test_logging_macros_cpp.testing.x.y.x", "message");
  // check that no call was made to the underlying log function
  EXPECT_EQ(0u, g_log_calls);

  // check that nameless log calls get the default level
  rcutils_logging_set_default_logger_level(RCUTILS_LOG_SEVERITY_INFO);
  RCUTILS_LOG_DEBUG("message");
  EXPECT_EQ(0u, g_log_calls);
}
