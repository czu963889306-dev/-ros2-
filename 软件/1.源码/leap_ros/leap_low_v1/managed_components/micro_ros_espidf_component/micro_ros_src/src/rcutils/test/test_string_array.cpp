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
#include "rcutils/types/string_array.h"

#ifdef _WIN32
  #define strdup _strdup
#endif

TEST(test_string_array, boot_string_array) {
  auto allocator = rcutils_get_default_allocator();
  auto failing_allocator = get_failing_allocator();
  rcutils_ret_t ret = RCUTILS_RET_OK;

  // UNDEFINED BEHAVIOR
  // rcutils_string_array_t sa00;
  // rcutils_string_array_fini(&sa00);

  rcutils_string_array_t sa0 = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_fini(&sa0);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, rcutils_string_array_init(NULL, 2, &allocator));
  rcutils_reset_error();
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, rcutils_string_array_init(&sa0, 2, NULL));
  rcutils_reset_error();
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, rcutils_string_array_init(&sa0, 2, &failing_allocator));
  rcutils_reset_error();

  rcutils_string_array_t sa1 = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_init(&sa1, 3, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ret = rcutils_string_array_fini(&sa1);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t sa2 = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_init(&sa2, 2, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  sa2.data[0] = strdup("Hello");
  sa2.data[1] = strdup("World");
  ret = rcutils_string_array_fini(&sa2);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  rcutils_string_array_t sa3 = rcutils_get_zero_initialized_string_array();
  ASSERT_EQ(RCUTILS_RET_OK, rcutils_string_array_init(&sa3, 3, &allocator));
  sa3.allocator.allocate = NULL;
  ASSERT_EQ(RCUTILS_RET_INVALID_ARGUMENT, rcutils_string_array_fini(NULL));
  rcutils_reset_error();
  ASSERT_EQ(RCUTILS_RET_INVALID_ARGUMENT, rcutils_string_array_fini(&sa3));
  rcutils_reset_error();
  sa3.allocator = allocator;
  ASSERT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&sa3));

  rcutils_string_array_t sa4 = rcutils_get_zero_initialized_string_array();
  ASSERT_EQ(RCUTILS_RET_OK, rcutils_string_array_init(&sa4, 0, &allocator));
  ASSERT_EQ(0u, sa4.size);
  ASSERT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&sa4));
}

TEST(test_string_array, string_array_cmp) {
  auto allocator = rcutils_get_default_allocator();
  rcutils_ret_t ret = RCUTILS_RET_OK;
  int res = 0;

  // Initialize some string arrays
  rcutils_string_array_t sa0 = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_init(&sa0, 3, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  sa0.data[0] = strdup("foo");
  sa0.data[1] = strdup("bar");
  sa0.data[2] = strdup("baz");

  rcutils_string_array_t sa1 = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_init(&sa1, 3, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  sa1.data[0] = strdup("foo");
  sa1.data[1] = strdup("bar");
  sa1.data[2] = strdup("baz");

  rcutils_string_array_t sa2 = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_init(&sa2, 3, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  sa2.data[0] = strdup("foo");
  sa2.data[1] = strdup("baz");
  sa2.data[2] = strdup("bar");

  rcutils_string_array_t sa3 = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_init(&sa3, 2, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  sa3.data[0] = strdup("foo");
  sa3.data[1] = strdup("bar");

  rcutils_string_array_t empty_string_array = rcutils_get_zero_initialized_string_array();
  rcutils_string_array_t incomplete_string_array = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_init(&incomplete_string_array, 3, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // Test failure cases
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, rcutils_string_array_cmp(NULL, &sa0, &res));
  rcutils_reset_error();
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, rcutils_string_array_cmp(&sa0, NULL, &res));
  rcutils_reset_error();
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, rcutils_string_array_cmp(&sa0, &sa1, NULL));
  rcutils_reset_error();
  EXPECT_EQ(RCUTILS_RET_ERROR, rcutils_string_array_cmp(&sa0, &incomplete_string_array, &res));
  rcutils_reset_error();

  // Test success cases
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&sa0, &sa1, &res));
  EXPECT_EQ(res, 0);
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&sa1, &sa0, &res));
  EXPECT_EQ(res, 0);
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&sa0, &sa2, &res));
  EXPECT_LT(res, 0);
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&sa2, &sa0, &res));
  EXPECT_GT(res, 0);
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&sa0, &sa3, &res));
  EXPECT_GT(res, 0);
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&sa3, &sa0, &res));
  EXPECT_LT(res, 0);
  // Test transitivity
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&sa3, &sa2, &res));
  EXPECT_LT(res, 0);
  // Test empty
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&sa0, &empty_string_array, &res));
  EXPECT_GT(res, 0);
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_string_array_cmp(&empty_string_array, &sa0, &res));
  EXPECT_LT(res, 0);

  ret = rcutils_string_array_fini(&sa0);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ret = rcutils_string_array_fini(&sa1);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ret = rcutils_string_array_fini(&sa2);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ret = rcutils_string_array_fini(&sa3);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ret = rcutils_string_array_fini(&incomplete_string_array);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
}

TEST(test_string_array, string_array_resize) {
  auto allocator = rcutils_get_default_allocator();
  auto failing_allocator = get_failing_allocator();
  auto invalid_allocator = rcutils_get_zero_initialized_allocator();
  auto time_bomb_allocator = get_time_bomb_allocator();
  rcutils_ret_t ret;

  ret = rcutils_string_array_resize(nullptr, 8);
  ASSERT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  // Start with 8 elements
  rcutils_string_array_t sa0;
  ret = rcutils_string_array_init(&sa0, 8, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  for (size_t i = 0; i < sa0.size; i++) {
    const char val[] = {static_cast<char>('a' + i), '\0'};
    sa0.data[i] = strdup(val);
  }

  // Resize to same size (hot path)
  ret = rcutils_string_array_resize(&sa0, sa0.size);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // Grow to 16 (with allocation failure)
  sa0.allocator = failing_allocator;
  ret = rcutils_string_array_resize(&sa0, 16);
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, ret);
  EXPECT_EQ(8u, sa0.size);
  rcutils_reset_error();

  // Grow to 16 (with invalid allocator)
  sa0.allocator = invalid_allocator;
  ret = rcutils_string_array_resize(&sa0, 16);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(8u, sa0.size);
  rcutils_reset_error();

  // Grow to 16
  sa0.allocator = allocator;
  ret = rcutils_string_array_resize(&sa0, 16);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ASSERT_EQ(16u, sa0.size);

  // Check that existing data is intact
  for (size_t i = 0; i < 8; i++) {
    const char val[] = {static_cast<char>('a' + i), '\0'};
    EXPECT_STREQ(val, sa0.data[i]);
  }

  // Check that new elements are empty
  for (size_t i = 8; i < sa0.size; i++) {
    const char val[] = {static_cast<char>('a' + i), '\0'};
    EXPECT_STREQ(nullptr, sa0.data[i]);
    sa0.data[i] = strdup(val);
  }

  // Shrink to 4 (with allocation failure)
  sa0.allocator = failing_allocator;
  ret = rcutils_string_array_resize(&sa0, 4);
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, ret);
  EXPECT_EQ(16u, sa0.size);
  rcutils_reset_error();

  // Shrink to 4 (with delayed allocation failure)
  set_time_bomb_allocator_realloc_count(time_bomb_allocator, 0);
  sa0.allocator = time_bomb_allocator;
  ret = rcutils_string_array_resize(&sa0, 4);
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, ret);
  EXPECT_EQ(16u, sa0.size);
  rcutils_reset_error();

  // Shrink to 4 (with invalid allocator)
  sa0.allocator = invalid_allocator;
  ret = rcutils_string_array_resize(&sa0, 4);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(16u, sa0.size);
  rcutils_reset_error();

  // Shrink to 4
  sa0.allocator = allocator;
  ret = rcutils_string_array_resize(&sa0, 4);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  ASSERT_EQ(4u, sa0.size);

  // Check that existing data is intact
  for (size_t i = 0; i < sa0.size; i++) {
    const char val[] = {static_cast<char>('a' + i), '\0'};
    EXPECT_STREQ(val, sa0.data[i]);
  }

  // Shrink to 0
  ret = rcutils_string_array_resize(&sa0, 0);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
  EXPECT_EQ(0u, sa0.size);

  sa0.allocator = allocator;
  ret = rcutils_string_array_fini(&sa0);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
}

TEST(test_string_array, string_array_sort) {
  auto allocator = rcutils_get_default_allocator();
  rcutils_ret_t ret = RCUTILS_RET_OK;

  ret = rcutils_string_array_sort(nullptr);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  rcutils_string_array_t sa0 = rcutils_get_zero_initialized_string_array();
  ret = rcutils_string_array_sort(&sa0);
  EXPECT_EQ(RCUTILS_RET_OK, ret);

  ret = rcutils_string_array_init(&sa0, 8, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  // Already in order
  for (size_t i = 0; i < sa0.size; i++) {
    const char val[] = {static_cast<char>('a' + i), '\0'};
    sa0.data[i] = strdup(val);
  }

  ret = rcutils_string_array_sort(&sa0);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
  for (size_t i = 0; i < sa0.size; i++) {
    const char val[] = {static_cast<char>('a' + i), '\0'};
    EXPECT_STREQ(val, sa0.data[i]);
  }

  // Reverse order
  for (size_t i = 0; i < sa0.size; i++) {
    const char val[] = {static_cast<char>('a' + sa0.size - 1 - i), '\0'};
    sa0.allocator.deallocate(sa0.data[i], sa0.allocator.state);
    sa0.data[i] = strdup(val);
  }

  ret = rcutils_string_array_sort(&sa0);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
  for (size_t i = 0; i < sa0.size; i++) {
    const char val[] = {static_cast<char>('a' + i), '\0'};
    EXPECT_STREQ(val, sa0.data[i]);
  }

  // Make some entries empty
  for (size_t i = 0; i < sa0.size / 2; i++) {
    sa0.allocator.deallocate(sa0.data[i], sa0.allocator.state);
    sa0.data[i] = nullptr;
  }

  ret = rcutils_string_array_sort(&sa0);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
  for (size_t i = 0; i < sa0.size / 2; i++) {
    const char val[] = {static_cast<char>('a' + i + (sa0.size / 2)), '\0'};
    EXPECT_STREQ(val, sa0.data[i]);
  }
  for (size_t i = sa0.size / 2; i < sa0.size; i++) {
    EXPECT_STREQ(nullptr, sa0.data[i]);
  }

  // Already in order, with empty entries
  ret = rcutils_string_array_sort(&sa0);
  EXPECT_EQ(RCUTILS_RET_OK, ret);
  for (size_t i = 0; i < sa0.size / 2; i++) {
    const char val[] = {static_cast<char>('a' + i + (sa0.size / 2)), '\0'};
    EXPECT_STREQ(val, sa0.data[i]);
  }
  for (size_t i = sa0.size / 2; i < sa0.size; i++) {
    EXPECT_STREQ(nullptr, sa0.data[i]);
  }

  ASSERT_EQ(RCUTILS_RET_OK, rcutils_string_array_fini(&sa0));
}
