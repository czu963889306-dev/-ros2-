// Copyright 2018-2019 Open Source Robotics Foundation, Inc.
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

#include "./allocator_testing_utils.h"
#include "./time_bomb_allocator_testing_utils.h"
#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/types/array_list.h"

auto failing_allocator = get_failing_allocator();
auto time_bomb_allocator = get_time_bomb_allocator();

class ArrayListTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Reset rcutils error to prevent failure from a previous test case to contaminate
    // another one.
    rcutils_reset_error();

    allocator = rcutils_get_default_allocator();
    list = rcutils_get_zero_initialized_array_list();
  }

  rcutils_allocator_t allocator;
  rcutils_array_list_t list;
};

// This fixture is used for test that want the list pre initialized
class ArrayListPreInitTest : public ArrayListTest
{
protected:
  void SetUp() override
  {
    ArrayListTest::SetUp();
    const rcutils_ret_t ret = rcutils_array_list_init(&list, 2, sizeof(uint32_t), &allocator);
    EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
    rcutils_reset_error();
  }

  void TearDown() override
  {
    const rcutils_ret_t ret = rcutils_array_list_fini(&list);
    EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
    rcutils_reset_error();
  }
};

TEST_F(ArrayListTest, init_list_null_fails) {
  rcutils_ret_t ret = rcutils_array_list_init(NULL, 2, sizeof(uint32_t), &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, init_initial_capacity_zero_fails) {
  rcutils_ret_t ret = rcutils_array_list_init(&list, 0, sizeof(uint32_t), &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, init_data_size_zero_fails) {
  rcutils_ret_t ret = rcutils_array_list_init(&list, 2, 0, &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, init_null_allocator_fails) {
  rcutils_ret_t ret = rcutils_array_list_init(&list, 2, sizeof(uint32_t), NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, fail_allocate_impl) {
  rcutils_allocator_t time_bomb_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(time_bomb_allocator, 0);
  rcutils_ret_t ret = rcutils_array_list_init(&list, 2, sizeof(uint32_t), &time_bomb_allocator);
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, fail_allocate_impl_list) {
  rcutils_allocator_t time_bomb_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(time_bomb_allocator, 1);
  rcutils_ret_t ret = rcutils_array_list_init(&list, 2, sizeof(uint32_t), &time_bomb_allocator);
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, init_success) {
  rcutils_ret_t ret = rcutils_array_list_init(&list, 2, sizeof(uint32_t), &allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_fini(&list);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, fini_list_null) {
  rcutils_ret_t ret = rcutils_array_list_fini(NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, fini_list_not_initialized) {
  rcutils_ret_t ret = rcutils_array_list_fini(&list);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, fini_success) {
  rcutils_ret_t ret;

  ret = rcutils_array_list_init(&list, 2, sizeof(uint32_t), &allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_fini(&list);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, add_list_null_fails) {
  uint32_t data = 22;
  rcutils_ret_t ret = rcutils_array_list_add(NULL, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, add_list_not_initialized_fails) {
  uint32_t data = 22;
  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, add_data_null_fails) {
  rcutils_ret_t ret = rcutils_array_list_add(&list, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, add_success) {
  uint32_t data = 22;
  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, set_list_null_fails) {
  uint32_t data = 22;
  size_t index = 0;
  rcutils_ret_t ret = rcutils_array_list_set(NULL, index, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, set_list_not_initialized_fails) {
  uint32_t data = 22;
  size_t index = 0;
  rcutils_ret_t ret = rcutils_array_list_set(&list, index, &data);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, set_data_null_fails) {
  uint32_t data = 22;
  size_t index = 0;

  // Add something first so we know the index isn't out of bounds
  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_set(&list, index, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, set_data_index_oob_fails) {
  uint32_t data = 22;
  size_t index = 1;

  // Add something first so we know the index isn't out of bounds
  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_set(&list, index, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, set_success_changes_data) {
  uint32_t data = 22;
  uint32_t ret_data = 0;
  size_t index = 0;

  // Add something first so we know the index isn't out of bounds
  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  data = 23;
  ret = rcutils_array_list_set(&list, index, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get(&list, index, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(data, ret_data);
}

TEST_F(ArrayListTest, remove_list_null_fails) {
  size_t index = 0;
  rcutils_ret_t ret = rcutils_array_list_remove(NULL, index);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, remove_list_not_initialized_fails) {
  size_t index = 0;
  rcutils_ret_t ret = rcutils_array_list_remove(&list, index);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, remove_data_index_oob_fails) {
  uint32_t data = 22;
  size_t index = 1;

  // Add something first so we know the index isn't out of bounds
  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_remove(&list, index);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, remove_success_removes_from_list) {
  uint32_t data = 22;
  size_t index = 0;
  size_t size = 0;

  // Add something first so we know the index isn't out of bounds
  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get_size(&list, &size);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)1);

  ret = rcutils_array_list_remove(&list, index);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get_size(&list, &size);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)0);
}

TEST_F(ArrayListPreInitTest, remove_success_removes_from_list_with_multiple_items) {
  uint32_t data = 22;
  size_t index = 0;
  size_t size = 0;
  rcutils_ret_t ret = RCUTILS_RET_OK;

  // Add a few things first so we know the index isn't out of bounds
  for (size_t i = 0; i < 3; ++i) {
    ret = rcutils_array_list_add(&list, &data);
    ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  }

  ret = rcutils_array_list_get_size(&list, &size);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)3);

  ret = rcutils_array_list_remove(&list, index);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get_size(&list, &size);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)2);
}

TEST_F(ArrayListTest, get_list_null_fails) {
  size_t index = 0;
  uint32_t data = 0;
  rcutils_ret_t ret = rcutils_array_list_get(NULL, index, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, get_list_not_initialized_fails) {
  size_t index = 0;
  uint32_t data = 0;
  rcutils_ret_t ret = rcutils_array_list_get(&list, index, &data);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, get_data_index_oob_fails) {
  uint32_t data = 22;
  uint32_t ret_data = 0;
  size_t index = 1;

  // Add something first so we know the index of 0 isn't out of bounds
  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get(&list, index, &ret_data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, get_data_success_returns_data) {
  uint32_t data = 22;
  uint32_t ret_data = 0;
  size_t index = 0;

  rcutils_ret_t ret = rcutils_array_list_add(&list, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get(&list, index, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(data, ret_data);
}

TEST_F(ArrayListTest, get_size_list_null_fails) {
  size_t size = 0;
  rcutils_ret_t ret = rcutils_array_list_get_size(NULL, &size);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListTest, get_size_list_not_initialized_fails) {
  size_t size = 0;
  rcutils_ret_t ret = rcutils_array_list_get_size(&list, &size);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, get_size_size_null_fails) {
  rcutils_ret_t ret = rcutils_array_list_get_size(&list, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, get_size_increases_with_add) {
  size_t size = 0;
  uint32_t data = 22;
  rcutils_ret_t ret;

  ret = rcutils_array_list_get_size(&list, &size);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)0);

  ret = rcutils_array_list_add(&list, &data);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get_size(&list, &size);
  ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)1);
}

// Don't let this one extend from ArrayListPreInitTest so we can control the initial cap
TEST_F(ArrayListTest, add_grow_capacity) {
  rcutils_ret_t ret = rcutils_array_list_init(&list, 2, sizeof(uint32_t), &allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  for (uint32_t i = 0; i < 17; ++i) {
    uint32_t data = i * 2;
    ret = rcutils_array_list_add(&list, &data);
    EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  }

  // Verify all the data is still set after growing
  for (uint32_t i = 0; i < 17; ++i) {
    uint32_t ret_data = 0;
    ret = rcutils_array_list_get(&list, i, &ret_data);
    ASSERT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
    EXPECT_EQ((i * 2), ret_data) << rcutils_get_error_string().str;
  }

  ret = rcutils_array_list_fini(&list);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(ArrayListPreInitTest, remove_preserves_data_around_it) {
  rcutils_ret_t ret;
  size_t size = 0;
  uint32_t ret_data = 0;
  for (uint32_t i = 0; i < 4; ++i) {
    uint32_t data = i * 2;
    ret = rcutils_array_list_add(&list, &data);
    EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  }

  ret = rcutils_array_list_get_size(&list, &size);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)4);

  ret = rcutils_array_list_remove(&list, 2);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get_size(&list, &size);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)3);

  ret = rcutils_array_list_get(&list, 0, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((uint32_t)0, ret_data) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get(&list, 1, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((uint32_t)2, ret_data) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get(&list, 2, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((uint32_t)6, ret_data) << rcutils_get_error_string().str;

  ret = rcutils_array_list_remove(&list, 2);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get_size(&list, &size);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)2);

  ret = rcutils_array_list_get(&list, 0, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((uint32_t)0, ret_data) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get(&list, 1, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((uint32_t)2, ret_data) << rcutils_get_error_string().str;

  ret = rcutils_array_list_remove(&list, 0);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get_size(&list, &size);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)1);

  ret = rcutils_array_list_get(&list, 0, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((uint32_t)2, ret_data) << rcutils_get_error_string().str;

  ret = rcutils_array_list_remove(&list, 0);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_array_list_get_size(&list, &size);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(size, (size_t)0);
}

TEST_F(ArrayListPreInitTest, init_list_twice_fails) {
  rcutils_ret_t ret = rcutils_array_list_init(&list, 2, sizeof(uint32_t), &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

typedef struct allocator_state
{
  bool is_failing;
} allocator_state;

TEST_F(ArrayListTest, list_add_bad_allocator_fails) {
  allocator_state state;
  uint32_t data = 22;
  state.is_failing = false;
  failing_allocator.state = &state;

  EXPECT_EQ(
    RCUTILS_RET_OK, rcutils_array_list_init(&list, 1, sizeof(uint32_t), &failing_allocator));
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_array_list_add(&list, &data));

  state.is_failing = true;
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, rcutils_array_list_add(&list, &data));

  state.is_failing = false;
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_array_list_fini(&list));
}
