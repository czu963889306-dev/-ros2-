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

#include <benchmark/benchmark.h>
#include <cassert>
#include <string>
#include <vector>

#include "../allocator_testing_utils.h"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcutils/logging.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

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

static void benchmark_logging(benchmark::State & state)
{
  for (auto _ : state) {
    auto ret_value = rcutils_logging_initialize();
    (void) ret_value;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      ret_value = rcutils_logging_shutdown();
      (void) ret_value;
    });
    g_rcutils_logging_default_logger_level = RCUTILS_LOG_SEVERITY_DEBUG;

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

    rcutils_logging_output_handler_t original_function = rcutils_logging_get_output_handler();
    rcutils_logging_set_output_handler(rcutils_logging_console_output_handler);
    int original_level = rcutils_logging_get_default_logger_level();

    // check all attributes for a debug log message
    rcutils_log_location_t location = {"func", "file", 42u};
    rcutils_log(&location, RCUTILS_LOG_SEVERITY_DEBUG, "name1", "message %d", 11);
    rcutils_logging_set_default_logger_level(RCUTILS_LOG_SEVERITY_ERROR);
    rcutils_log(NULL, RCUTILS_LOG_SEVERITY_INFO, "name2", "message %d", 22);
    rcutils_logging_set_default_logger_level(RCUTILS_LOG_SEVERITY_UNSET);
    rcutils_log(NULL, RCUTILS_LOG_SEVERITY_INFO, "name3", "message %d", 33);
    rcutils_log(NULL, RCUTILS_LOG_SEVERITY_WARN, "", "%s", "");
    rcutils_log(NULL, RCUTILS_LOG_SEVERITY_ERROR, "", "%s", "");
    rcutils_log(NULL, RCUTILS_LOG_SEVERITY_FATAL, NULL, "%s", "");

    // restore original state
    rcutils_logging_set_default_logger_level(original_level);
    rcutils_logging_set_output_handler(original_function);
  }
}

BENCHMARK(benchmark_logging);
