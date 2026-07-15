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

#include <string.h>

#include "rcutils/allocator.h"
#include "rcutils/logging_macros.h"
#include "rcutils/time.h"
#include "rcutils/types/rcutils_ret.h"

size_t g_log_calls = 0;

struct LogEvent
{
  const rcutils_log_location_t * location;
  int severity;
  const char * name;
  rcutils_time_point_value_t timestamp;
  char * message;
};
struct LogEvent g_last_log_event;

void custom_handler(
  const rcutils_log_location_t * location,
  int severity, const char * name, rcutils_time_point_value_t timestamp,
  const char * format, va_list * args)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  g_log_calls += 1;
  g_last_log_event.location = location;
  g_last_log_event.severity = severity;
  g_last_log_event.name = name ? name : "";
  g_last_log_event.timestamp = timestamp;
  if (g_last_log_event.message) {
    allocator.deallocate(g_last_log_event.message, allocator.state);
  }
  const size_t size = 1024;
  g_last_log_event.message = allocator.allocate(size, allocator.state);
  vsnprintf(g_last_log_event.message, size, format, *args);
}

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  g_log_calls = 0;
  if (g_rcutils_logging_initialized) {
    return 1;
  }
  rcutils_ret_t ret = rcutils_logging_initialize();
  if (ret != RCUTILS_RET_OK || !g_rcutils_logging_initialized) {
    return 2;
  }

  rcutils_logging_output_handler_t previous_output_handler = \
    rcutils_logging_get_output_handler();
  rcutils_logging_set_output_handler(custom_handler);

  size_t line_number = __LINE__; RCUTILS_LOG_INFO("empty message");
  if (g_log_calls != 1u) {
    fprintf(stderr, "unexpected number of log calls\n");
    return 3;
  }
  if (NULL == g_last_log_event.location) {
    fprintf(stderr, "location unexpectedly nullptr\n");
    return 4;
  }
  if (strcmp(g_last_log_event.location->function_name, "main")) {
    fprintf(stderr, "function unexpectedly not 'main'\n");
    return 5;
  }
  if (g_last_log_event.location->line_number != line_number) {
    fprintf(stderr, "unexpected line number %zu\n", g_last_log_event.location->line_number);
    return 6;
  }
  if (g_last_log_event.severity != RCUTILS_LOG_SEVERITY_INFO) {
    fprintf(stderr, "severity unexpectedly not RCUTILS_LOG_SEVERITY_INFO\n");
    return 7;
  }
  if (strcmp(g_last_log_event.name, "")) {
    fprintf(stderr, "name unexpectedly not empty string\n");
    return 8;
  }
  if (strcmp(g_last_log_event.message, "empty message")) {
    fprintf(stderr, "message unexpectedly not 'empty message'\n");
    return 9;
  }

  line_number = __LINE__; RCUTILS_LOG_INFO("message %s", "foo");
  if (g_log_calls != 2u) {
    fprintf(stderr, "unexpected number of log calls\n");
    return 10;
  }
  if (NULL == g_last_log_event.location) {
    fprintf(stderr, "location unexpectedly nullptr\n");
    return 11;
  }
  if (strcmp(g_last_log_event.location->function_name, "main")) {
    fprintf(stderr, "function unexpectedly not 'main'\n");
    return 12;
  }
  if (g_last_log_event.location->line_number != line_number) {
    fprintf(stderr, "unexpected line number %zu\n", g_last_log_event.location->line_number);
    return 13;
  }
  if (g_last_log_event.severity != RCUTILS_LOG_SEVERITY_INFO) {
    fprintf(stderr, "severity unexpectedly not RCUTILS_LOG_SEVERITY_INFO\n");
    return 14;
  }
  if (strcmp(g_last_log_event.name, "")) {
    fprintf(stderr, "name unexpectedly not empty string\n");
    return 15;
  }
  if (strcmp(g_last_log_event.message, "message foo")) {
    fprintf(stderr, "message unexpectedly not 'message foo'\n");
    return 16;
  }

  rcutils_logging_set_output_handler(previous_output_handler);
  if (g_last_log_event.message) {
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    allocator.deallocate(g_last_log_event.message, allocator.state);
  }

  ret = rcutils_logging_shutdown();
  if (ret != RCUTILS_RET_OK || g_rcutils_logging_initialized) {
    fprintf(stderr, "rcutils_logging_shutdown() unexpectedly failed\n");
    return 17;
  }
}
