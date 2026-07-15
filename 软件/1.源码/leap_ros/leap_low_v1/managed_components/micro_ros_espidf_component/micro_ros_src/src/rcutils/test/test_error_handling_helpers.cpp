// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#include <string>

#include "gmock/gmock.h"

#include "osrf_testing_tools_cpp/memory_tools/gtest_quickstart.hpp"

#include "../src/error_handling_helpers.h"

TEST(test_error_handling, copy_string) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  char buffer[10];
  memset(buffer, '?', sizeof(buffer));
  size_t written;

  // normal truncation
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    written = __rcutils_copy_string(buffer, 3, "0123456789");
  });
  EXPECT_EQ(written, 2u);
  EXPECT_EQ(strnlen(buffer, sizeof(buffer)), 2u);
  EXPECT_STREQ(buffer, "01");

  // normal truncation, 1 short of buffer length
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    written = __rcutils_copy_string(buffer, 9, "0123456789");
  });
  EXPECT_EQ(written, 8u);
  EXPECT_EQ(strnlen(buffer, sizeof(buffer)), 8u);
  EXPECT_STREQ(buffer, "01234567");

  // input smaller than buffer, 1 short of buffer length
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    written = __rcutils_copy_string(buffer, 9, "");
  });
  EXPECT_EQ(written, 0u);
  EXPECT_EQ(strnlen(buffer, sizeof(buffer)), 0u);
  EXPECT_STREQ(buffer, "");

  // copy where src and dst overlap (testing use of memmove vs memcpy)
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    written = __rcutils_copy_string(buffer, sizeof(buffer), "1234567890");
  });
  EXPECT_EQ(written, 9u);
  EXPECT_STREQ(buffer, "123456789");
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    written = __rcutils_copy_string(buffer, sizeof(buffer), buffer + 3);
  });
  EXPECT_EQ(written, 6u);
  EXPECT_EQ(strnlen(buffer, sizeof(buffer)), (sizeof(buffer) - 1) - 3);
  EXPECT_STREQ(buffer, "456789");
}

TEST(test_error_handling, reverse_str) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  {
    char buffer[] = "even";
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_reverse_str(buffer, strnlen(buffer, sizeof(buffer)));
    });
    EXPECT_STREQ(buffer, "neve");
  }
  {
    char buffer[] = "reverseme";
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_reverse_str(buffer, strnlen(buffer, sizeof(buffer)));
    });
    EXPECT_STREQ(buffer, "emesrever");
  }
  {
    char buffer[] = "a";
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_reverse_str(buffer, strnlen(buffer, sizeof(buffer)));
    });
    EXPECT_STREQ(buffer, "a");
  }
  {
    char buffer[] = "reverseme";
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_reverse_str(buffer, 3);
    });
    EXPECT_STREQ(buffer, "vererseme");
  }
  {
    char buffer[] = "doesntmatter";
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_reverse_str(buffer, 0);
    });
    EXPECT_STREQ(buffer, "doesntmatter");
  }
}

TEST(test_error_handling, convert_uint64_t_into_c_str) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  {
    uint64_t number = UINT64_MAX;
    char buffer[21];
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_convert_uint64_t_into_c_str(number, buffer, sizeof(buffer));
    });
    EXPECT_STREQ(buffer, "18446744073709551615");
  }
  {
    uint64_t number = 0;
    char buffer[21];
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_convert_uint64_t_into_c_str(number, buffer, sizeof(buffer));
    });
    EXPECT_STREQ(buffer, "0");
  }
  {
    uint64_t number = 42;
    char buffer[21];
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_convert_uint64_t_into_c_str(number, buffer, sizeof(buffer));
    });
    EXPECT_STREQ(buffer, "42");
  }
  {
    uint64_t number = INT64_MAX;
    char buffer[21];
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      __rcutils_convert_uint64_t_into_c_str(number, buffer, sizeof(buffer));
    });
    EXPECT_STREQ(buffer, "9223372036854775807");
  }
}

TEST(test_error_handling, format_error_string) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  rcutils_error_string_t error_string {""};
  rcutils_error_state_t error_state {"test error message", "/path/to/source", 42};

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    __rcutils_format_error_string(&error_string, &error_state);
  });
}
