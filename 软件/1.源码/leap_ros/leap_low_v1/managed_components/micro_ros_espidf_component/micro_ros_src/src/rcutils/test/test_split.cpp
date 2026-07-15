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

#include "gtest/gtest.h"

#include "./allocator_testing_utils.h"
#include "./time_bomb_allocator_testing_utils.h"
#include "rcutils/error_handling.h"
#include "rcutils/split.h"
#include "rcutils/types/string_array.h"

#define ENABLE_LOGGING 1

#if ENABLE_LOGGING
#define LOG(expected, actual) { \
    printf("Expected: %s Actual: %s\n", expected, actual);}
#else
#define LOG(X, arg) {}
#endif

rcutils_string_array_t test_split(const char * str, char delimiter, size_t expected_token_size)
{
  rcutils_string_array_t tokens = rcutils_get_zero_initialized_string_array();
  rcutils_ret_t ret = rcutils_split(
    str, delimiter, rcutils_get_default_allocator(), &tokens);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
  fprintf(stderr, "Received %zu tokens\n", tokens.size);
  EXPECT_EQ(expected_token_size, tokens.size);
  for (size_t i = 0; i < tokens.size; ++i) {
    EXPECT_NE((size_t)0, strlen(tokens.data[i]));
  }
  return tokens;
}

rcutils_string_array_t test_split_last(
  const char * str, char delimiter, size_t expected_token_size)
{
  rcutils_string_array_t tokens = rcutils_get_zero_initialized_string_array();
  rcutils_ret_t ret = rcutils_split_last(
    str, delimiter, rcutils_get_default_allocator(), &tokens);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
  EXPECT_EQ(expected_token_size, tokens.size);
  for (size_t i = 0; i < tokens.size; ++i) {
    EXPECT_NE((size_t)0, strlen(tokens.data[i]));
  }
  return tokens;
}

TEST(test_split, split) {
  rcutils_ret_t ret = RCUTILS_RET_OK;
  rcutils_string_array_t tokens_fail;
  EXPECT_EQ(
    RCUTILS_RET_INVALID_ARGUMENT,
    rcutils_split("Test", '/', rcutils_get_default_allocator(), NULL));

  // Allocating string_array->data fails
  rcutils_allocator_t time_bomb_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(time_bomb_allocator, 0);
  EXPECT_EQ(
    RCUTILS_RET_ERROR,
    rcutils_split("Test", '/', time_bomb_allocator, &tokens_fail));

  // Allocating string_array->data[0] fails
  set_time_bomb_allocator_malloc_count(time_bomb_allocator, 1);
  EXPECT_EQ(
    RCUTILS_RET_ERROR,
    rcutils_split("hello/world", '/', time_bomb_allocator, &tokens_fail));

  rcutils_string_array_t tokens0 = test_split("", '/', 0);
  ret = rcutils_string_array_fini(&tokens0);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens00 = test_split(NULL, '/', 0);
  ret = rcutils_string_array_fini(&tokens00);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens1 = test_split("hello_world", '/', 1);
  EXPECT_STREQ("hello_world", tokens1.data[0]);
  ret = rcutils_string_array_fini(&tokens1);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  rcutils_string_array_t tokens2 = test_split("hello/world", '/', 2);
  EXPECT_STREQ("hello", tokens2.data[0]);
  EXPECT_STREQ("world", tokens2.data[1]);
  ret = rcutils_string_array_fini(&tokens2);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens3 = test_split("/hello/world", '/', 2);
  EXPECT_STREQ("hello", tokens3.data[0]);
  EXPECT_STREQ("world", tokens3.data[1]);
  ret = rcutils_string_array_fini(&tokens3);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens4 = test_split("hello/world/", '/', 2);
  EXPECT_STREQ("hello", tokens4.data[0]);
  EXPECT_STREQ("world", tokens4.data[1]);
  ret = rcutils_string_array_fini(&tokens4);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens5 = test_split("hello//world", '/', 2);
  EXPECT_STREQ("hello", tokens5.data[0]);
  EXPECT_STREQ("world", tokens5.data[1]);
  ret = rcutils_string_array_fini(&tokens5);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens6 = test_split("/hello//world", '/', 2);
  EXPECT_STREQ("hello", tokens6.data[0]);
  EXPECT_STREQ("world", tokens6.data[1]);
  ret = rcutils_string_array_fini(&tokens6);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens7 = test_split("my/hello/world", '/', 3);
  EXPECT_STREQ("my", tokens7.data[0]);
  EXPECT_STREQ("hello", tokens7.data[1]);
  EXPECT_STREQ("world", tokens7.data[2]);
  ret = rcutils_string_array_fini(&tokens7);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens8 = test_split("/my/hello/world", '/', 3);
  EXPECT_STREQ("my", tokens8.data[0]);
  EXPECT_STREQ("hello", tokens8.data[1]);
  EXPECT_STREQ("world", tokens8.data[2]);
  ret = rcutils_string_array_fini(&tokens8);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens9 = test_split("/my/hello/world/", '/', 3);
  EXPECT_STREQ("my", tokens9.data[0]);
  EXPECT_STREQ("hello", tokens9.data[1]);
  EXPECT_STREQ("world", tokens9.data[2]);
  ret = rcutils_string_array_fini(&tokens9);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens10 = test_split("/my//hello//world//", '/', 3);
  EXPECT_STREQ("my", tokens10.data[0]);
  EXPECT_STREQ("hello", tokens10.data[1]);
  EXPECT_STREQ("world", tokens10.data[2]);
  ret = rcutils_string_array_fini(&tokens10);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens11 = test_split("///my//hello//world/////", '/', 3);
  EXPECT_STREQ("my", tokens11.data[0]);
  EXPECT_STREQ("hello", tokens11.data[1]);
  EXPECT_STREQ("world", tokens11.data[2]);
  ret = rcutils_string_array_fini(&tokens11);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
}

TEST(test_split, split_last) {
  rcutils_ret_t ret = RCUTILS_RET_OK;
  rcutils_string_array_t tokens_fail;

  // Allocating string_array fails
  rcutils_allocator_t time_bomb_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_calloc_count(time_bomb_allocator, 0);
  EXPECT_EQ(
    RCUTILS_RET_BAD_ALLOC,
    rcutils_split_last("Test", '/', time_bomb_allocator, &tokens_fail));

  // Allocating string_array->data[0] fails
  set_time_bomb_allocator_malloc_count(time_bomb_allocator, 0);
  EXPECT_EQ(
    RCUTILS_RET_BAD_ALLOC,
    rcutils_split_last("Test", '/', time_bomb_allocator, &tokens_fail));

  // Allocating string_array fails, found_last != string_size
  set_time_bomb_allocator_calloc_count(time_bomb_allocator, 0);
  EXPECT_EQ(
    RCUTILS_RET_BAD_ALLOC,
    rcutils_split_last("hello/world", '/', time_bomb_allocator, &tokens_fail));

  // Allocating string_array->data[0] fails, found_last != string_size
  set_time_bomb_allocator_malloc_count(time_bomb_allocator, 0);
  EXPECT_EQ(
    RCUTILS_RET_BAD_ALLOC,
    rcutils_split_last("hello/world", '/', time_bomb_allocator, &tokens_fail));

  // Allocating string_array->data[1] fails, found_last != string_size
  set_time_bomb_allocator_malloc_count(time_bomb_allocator, 1);
  EXPECT_EQ(
    RCUTILS_RET_BAD_ALLOC,
    rcutils_split_last("hello/world", '/', time_bomb_allocator, &tokens_fail));

  rcutils_string_array_t tokens0 = test_split_last("", '/', 0);
  ret = rcutils_string_array_fini(&tokens0);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens00 = test_split_last(NULL, '/', 0);
  ret = rcutils_string_array_fini(&tokens00);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens1 = test_split_last("hello_world", '/', 1);
  LOG("hello_world", tokens1.data[0]);
  EXPECT_STREQ("hello_world", tokens1.data[0]);
  ret = rcutils_string_array_fini(&tokens1);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens2 = test_split_last("hello/world", '/', 2);
  EXPECT_STREQ("hello", tokens2.data[0]);
  LOG("hello", tokens2.data[0]);
  EXPECT_STREQ("world", tokens2.data[1]);
  LOG("world", tokens2.data[1]);
  ret = rcutils_string_array_fini(&tokens2);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens3 = test_split_last("/hello/world", '/', 2);
  EXPECT_STREQ("hello", tokens3.data[0]);
  LOG("hello", tokens3.data[0]);
  EXPECT_STREQ("world", tokens3.data[1]);
  ret = rcutils_string_array_fini(&tokens3);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens4 = test_split_last("hello/world/", '/', 2);
  EXPECT_STREQ("hello", tokens4.data[0]);
  EXPECT_STREQ("world", tokens4.data[1]);
  ret = rcutils_string_array_fini(&tokens4);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens5 = test_split_last("hello//world/", '/', 2);
  EXPECT_STREQ("hello", tokens5.data[0]);
  LOG("hello", tokens5.data[0]);
  EXPECT_STREQ("world", tokens5.data[1]);
  LOG("world", tokens5.data[1]);
  ret = rcutils_string_array_fini(&tokens5);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens6 = test_split_last("/hello//world", '/', 2);
  EXPECT_STREQ("hello", tokens6.data[0]);
  EXPECT_STREQ("world", tokens6.data[1]);
  ret = rcutils_string_array_fini(&tokens6);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens7 = test_split_last("my/hello//world", '/', 2);
  EXPECT_STREQ("my/hello", tokens7.data[0]);
  EXPECT_STREQ("world", tokens7.data[1]);
  ret = rcutils_string_array_fini(&tokens7);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t tokens8 = test_split_last("/my/hello//world/", '/', 2);
  EXPECT_STREQ("my/hello", tokens8.data[0]);
  EXPECT_STREQ("world", tokens8.data[1]);
  ret = rcutils_string_array_fini(&tokens8);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
}
