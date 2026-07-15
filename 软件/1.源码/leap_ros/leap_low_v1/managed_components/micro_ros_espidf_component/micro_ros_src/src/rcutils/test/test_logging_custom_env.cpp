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

#include "./allocator_testing_utils.h"
#include "./mocking_utils/patch.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcutils/logging.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

TEST(CLASSNAME(TestLoggingCustomEnv, RMW_IMPLEMENTATION), test_logging) {
  EXPECT_FALSE(g_rcutils_logging_initialized);
  ASSERT_EQ(RCUTILS_RET_OK, rcutils_logging_initialize());
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCUTILS_RET_OK, rcutils_logging_shutdown());
  });
  EXPECT_TRUE(g_rcutils_logging_initialized);
  g_rcutils_logging_default_logger_level = RCUTILS_LOG_SEVERITY_DEBUG;

  rcutils_ret_t ret;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcutils_char_array_t msg_buf, output_buf;
  ret = rcutils_char_array_init(&msg_buf, 1024, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ret = rcutils_char_array_init(&output_buf, 1024, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  rcutils_time_point_value_t now = 0;

  ret = rcutils_logging_format_message(
    NULL, RCUTILS_LOG_SEVERITY_FATAL, NULL, now, msg_buf.buffer, &output_buf);
  EXPECT_EQ(RCUTILS_RET_OK, ret);

  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_fini(&output_buf));
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_fini(&msg_buf));
}

TEST(CLASSNAME(TestLoggingCustomEnv, RMW_IMPLEMENTATION), test_logging_with_buffering_issues) {
  auto mock = mocking_utils::patch("lib:rcutils", setvbuf, [](auto && ...) {return -1;});
  EXPECT_FALSE(g_rcutils_logging_initialized);
  EXPECT_EQ(RCUTILS_RET_ERROR, rcutils_logging_initialize());
  EXPECT_FALSE(g_rcutils_logging_initialized);
}
