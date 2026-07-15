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

#include <gtest/gtest.h>
#include <string.h>

#include "./allocator_testing_utils.h"
#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/types/char_array.h"

#include "./mocking_utils/patch.hpp"

class ArrayCharTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Reset rcutils error to prevent failure from a previous test case to contaminate
    // another one.
    rcutils_reset_error();

    allocator = rcutils_get_default_allocator();
    char_array = rcutils_get_zero_initialized_char_array();
  }

  rcutils_allocator_t allocator;
  rcutils_char_array_t char_array;
};

static rcutils_ret_t example_logger(
  rcutils_char_array_t * char_array,
  const char * format, ...)
{
  rcutils_ret_t status;
  va_list args;
  va_start(args, format);
  status = rcutils_char_array_vsprintf(char_array, format, args);
  va_end(args);
  return status;
}

TEST_F(ArrayCharTest, default_initialization) {
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_init(&char_array, 0, &allocator));
  EXPECT_EQ(0lu, char_array.buffer_capacity);
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_fini(&char_array));
  EXPECT_EQ(0lu, char_array.buffer_capacity);
  EXPECT_FALSE(char_array.buffer);
}

TEST_F(ArrayCharTest, resize) {
  rcutils_ret_t ret = rcutils_char_array_init(&char_array, 5, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  char_array.buffer_length = static_cast<std::size_t>(
    snprintf(char_array.buffer, char_array.buffer_capacity, "1234") + 1);
  EXPECT_STREQ("1234", char_array.buffer);

  ret = rcutils_char_array_resize(&char_array, 0);
  ASSERT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(5lu, char_array.buffer_capacity);
  EXPECT_EQ(5lu, char_array.buffer_length);
  rcutils_reset_error();

  ret = rcutils_char_array_resize(&char_array, 6);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  EXPECT_EQ(6lu, char_array.buffer_capacity);
  EXPECT_EQ(5lu, char_array.buffer_length);

  ret = rcutils_char_array_resize(&char_array, 11);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  EXPECT_EQ(11lu, char_array.buffer_capacity);
  EXPECT_EQ(5lu, char_array.buffer_length);

  // Resize to same capacity, nothing to be done
  ret = rcutils_char_array_resize(&char_array, 11);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  EXPECT_EQ(11lu, char_array.buffer_capacity);
  EXPECT_EQ(5lu, char_array.buffer_length);

  char_array.buffer_length = static_cast<std::size_t>(
    snprintf(char_array.buffer, char_array.buffer_capacity, "0987654321") + 1);
  EXPECT_STREQ("0987654321", char_array.buffer);

  ret = rcutils_char_array_resize(&char_array, 3);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  EXPECT_EQ(3lu, char_array.buffer_capacity);
  EXPECT_EQ(3lu, char_array.buffer_length);
  EXPECT_EQ('0', char_array.buffer[0]);
  EXPECT_EQ('9', char_array.buffer[1]);
  EXPECT_EQ('8', char_array.buffer[2]);
  // the other fields are garbage.

  // cleanup only 3 fields
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_fini(&char_array));

  allocator = get_failing_allocator();
  ret = rcutils_char_array_init(&char_array, 0, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // Force a failing allocation on resizing
  char_array.owns_buffer = false;
  ret = rcutils_char_array_resize(&char_array, 3);
  ASSERT_EQ(RCUTILS_RET_BAD_ALLOC, ret);

  // Nothing to cleanup, but it should be ok
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_fini(&char_array));
}

TEST_F(ArrayCharTest, vsprintf_fail) {
  rcutils_allocator_t failing_allocator = get_failing_allocator();
  rcutils_ret_t ret = rcutils_char_array_init(&char_array, 10, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  char_array.allocator = failing_allocator;
  ret = example_logger(&char_array, "Long string for the case %d", 2);
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, ret);
  rcutils_reset_error();
  char_array.allocator = allocator;

  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_fini(&char_array));
}

TEST_F(ArrayCharTest, strcpy) {
  rcutils_allocator_t failing_allocator = get_failing_allocator();
  rcutils_ret_t ret = rcutils_char_array_init(&char_array, 8, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_strcpy(&char_array, "1234"));
  EXPECT_STREQ("1234", char_array.buffer);
  EXPECT_EQ(5lu, char_array.buffer_length);

  char_array.allocator = failing_allocator;
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, rcutils_char_array_strcpy(&char_array, "123456789"));
  rcutils_reset_error();

  char_array.allocator = allocator;
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_fini(&char_array));
}

TEST_F(ArrayCharTest, strcat) {
  rcutils_allocator_t failing_allocator = get_failing_allocator();
  rcutils_ret_t ret = rcutils_char_array_init(&char_array, 8, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_strcpy(&char_array, "1234"));
  EXPECT_STREQ("1234", char_array.buffer);
  EXPECT_EQ(5lu, char_array.buffer_length);

  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_strcat(&char_array, "56"));
  EXPECT_STREQ("123456", char_array.buffer);
  EXPECT_EQ(7lu, char_array.buffer_length);

  char_array.allocator = failing_allocator;
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, rcutils_char_array_strcat(&char_array, "7890"));
  rcutils_reset_error();

  char_array.allocator = allocator;
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_char_array_fini(&char_array));
}
