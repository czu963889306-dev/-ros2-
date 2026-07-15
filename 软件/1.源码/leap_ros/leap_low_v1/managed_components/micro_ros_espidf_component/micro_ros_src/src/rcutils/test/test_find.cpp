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

#include <stdint.h>

#include "gtest/gtest.h"

#include "rcutils/find.h"

#define ENABLE_LOGGING 1

#if ENABLE_LOGGING
#define LOG(expected, actual) do { \
    printf("Expected: %zu Actual: %zu\n", expected, actual); \
} while (0)
#else
#define LOG(X, arg) do { \
    (void)(X); \
    (void)(arg); \
} while (0)
#endif

size_t test_find(const char * str, char delimiter, size_t expected_pos)
{
  size_t actual_pos = rcutils_find(str, delimiter);
  EXPECT_EQ(expected_pos, actual_pos);
  return actual_pos;
}

size_t test_findn(const char * str, char delimiter, size_t str_len, size_t expected_pos)
{
  size_t actual_pos = rcutils_findn(str, delimiter, str_len);
  EXPECT_EQ(expected_pos, actual_pos);
  return actual_pos;
}

size_t test_find_last(const char * str, char delimiter, size_t expected_pos)
{
  size_t actual_pos = rcutils_find_last(str, delimiter);
  EXPECT_EQ(expected_pos, actual_pos);
  return actual_pos;
}

size_t test_find_lastn(const char * str, char delimiter, size_t str_len, size_t expected_pos)
{
  size_t actual_pos = rcutils_find_lastn(str, delimiter, str_len);
  EXPECT_EQ(expected_pos, actual_pos);
  return actual_pos;
}

TEST(test_find, find) {
  size_t ret0 = test_find("", '/', SIZE_MAX);
  // We cast SIZE_MAX to a size_t here (and below) to shut up warnings on macOS.
  // The problem on macOS is that size_t is a long unsigned int, while SIZE_MAX
  // is an unsigned long long.  While the two are compatible on 64-bit, they are
  // not considered the "same" by the compiler, and throw a warning.  Just cast
  // SIZE_MAX to size_t to make the compiler happy.
  LOG((size_t)SIZE_MAX, ret0);

  size_t ret00 = test_find(NULL, '/', SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret00);

  size_t ret1 = test_find("hello_world", '/', SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret1);

  size_t ret2 = test_find("hello/world", '/', 5);
  LOG((size_t)5, ret2);

  size_t ret3 = test_find("/hello/world", '/', 0);
  LOG((size_t)0, ret3);

  size_t ret4 = test_find("hello/world/", '/', 5);
  LOG((size_t)5, ret4);

  size_t ret5 = test_find("hello//world", '/', 5);
  LOG((size_t)5, ret5);

  size_t ret6 = test_find("/hello//world", '/', 0);
  LOG((size_t)0, ret6);
}

TEST(test_find, findn) {
  size_t ret0 = test_findn("", '/', 0, SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret0);

  size_t ret1 = test_findn(NULL, '/', 10, SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret1);

  size_t ret2 = test_findn("hello_world", '/', strlen("hello_world"), SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret2);

  size_t ret3 = test_findn("hello/world", '/', strlen("hello/world"), 5);
  LOG((size_t)5, ret3);

  size_t ret4 = test_findn("hello///", '/', strlen("hello/"), 5);
  LOG((size_t)5, ret4);
}

TEST(test_find, find_last) {
  size_t ret0 = test_find_last("", '/', SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret0);

  size_t ret00 = test_find_last(NULL, '/', SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret00);

  size_t ret1 = test_find_last("hello_world", '/', SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret1);

  size_t ret2 = test_find_last("hello/world", '/', 5);
  LOG((size_t)5, ret2);

  size_t ret3 = test_find_last("/hello/world", '/', 6);
  LOG((size_t)6, ret3);

  size_t ret4 = test_find_last("hello/world/", '/', strlen("hello_world/") - 1);
  LOG((size_t)strlen("hello_world/") - 1, ret4);

  size_t ret5 = test_find_last("hello//world", '/', 6);
  LOG((size_t)6, ret5);

  size_t ret6 = test_find_last("/hello//world", '/', 7);
  LOG((size_t)7, ret6);
}

TEST(test_find, find_lastn) {
  size_t ret0 = test_find_lastn("", '/', 0, SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret0);

  size_t ret1 = test_find_lastn(NULL, '/', 10, SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret1);

  size_t ret2 = test_find_lastn("hello_world", '/', strlen("hello_world"), SIZE_MAX);
  LOG((size_t)SIZE_MAX, ret2);

  size_t ret3 = test_find_lastn("hello/world", '/', strlen("hello/world"), 5);
  LOG((size_t)5, ret3);

  size_t ret4 = test_find_lastn("/hello/world", '/', strlen("/hello"), 0);
  LOG((size_t)0, ret4);

  size_t ret5 = test_find_lastn(
    "hello/world///", '/', strlen("hello/world/"), strlen("hello/world/") - 1);
  LOG((size_t)strlen("hello/world/") - 1, ret5);
}
