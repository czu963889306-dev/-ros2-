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

#include "./time_bomb_allocator_testing_utils.h"
#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/types/hash_map.h"

size_t test_hash_map_uint32_hash_func(const void * key)
{
  const uint32_t * key_ptr = reinterpret_cast<const uint32_t *>(key);
  return *key_ptr * 73;
}

int test_uint32_cmp(const void * val1, const void * val2)
{
  const uint32_t * cval1 = (const uint32_t *)val1;
  const uint32_t * cval2 = (const uint32_t *)val2;
  if (*cval1 < *cval2) {
    return -1;
  } else if (*cval2 > *cval1) {
    return 1;
  } else {
    return 0;
  }
}

class HashMapBaseTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    allocator = rcutils_get_default_allocator();
    map = rcutils_get_zero_initialized_hash_map();
    rcutils_reset_error();
  }

  // void TearDown() override {}

  rcutils_allocator_t allocator;
  rcutils_hash_map_t map;
};

// This fixture is used for test that want the map pre initialized
class HashMapPreInitTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    allocator = rcutils_get_default_allocator();
    map = rcutils_get_zero_initialized_hash_map();
    rcutils_ret_t ret = rcutils_hash_map_init(
      &map, 2, sizeof(uint32_t), sizeof(uint32_t),
      test_hash_map_uint32_hash_func, test_uint32_cmp, &allocator);
    EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
    rcutils_reset_error();
  }

  void TearDown() override
  {
    rcutils_ret_t ret = rcutils_hash_map_fini(&map);
    EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  }

  rcutils_allocator_t allocator;
  rcutils_hash_map_t map;
};

TEST_F(HashMapBaseTest, init_map_NULL_fails) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    NULL, 2, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, init_map_initial_capacity_zero_fails) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 0, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, init_map_key_size_zero_fails) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, 0, sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, init_map_data_size_zero_fails) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), 0,
    test_hash_map_uint32_hash_func, test_uint32_cmp, &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, init_map_hash_func_NULL_fails) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), sizeof(uint32_t),
    NULL, test_uint32_cmp, &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, init_map_cmp_func_NULL_fails) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, NULL, &allocator);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, init_map_allocator_NULL_fails) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, init_map_failing_allocator) {
  rcutils_allocator_t failing_allocator = get_time_bomb_allocator();
  // Check allocating hash_map->impl fails
  set_time_bomb_allocator_malloc_count(failing_allocator, 0);
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, &failing_allocator);
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, ret) << rcutils_get_error_string().str;

  // Check allocate hash_map->impl->map fails
  set_time_bomb_allocator_malloc_count(failing_allocator, 1);
  ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, &failing_allocator);
  EXPECT_EQ(RCUTILS_RET_BAD_ALLOC, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, init_map_success) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, &allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_hash_map_fini(&map);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, fini_map_success) {
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, &allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_hash_map_fini(&map);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_capacity_map_null_fails) {
  size_t cap = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_capacity(NULL, &cap);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, get_capacity_map_not_init_fails) {
  size_t cap = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_capacity(&map, &cap);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_capacity_capacity_null_fails) {
  rcutils_ret_t ret = rcutils_hash_map_get_capacity(&map, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_capacity_success_returns_ok) {
  size_t cap = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_capacity(&map, &cap);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((size_t)2, cap);
}

TEST_F(HashMapPreInitTest, get_size_map_null_fails) {
  size_t size = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_size(NULL, &size);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, get_size_map_not_init_fails) {
  size_t size = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_size(&map, &size);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_size_size_null_fails) {
  rcutils_ret_t ret = rcutils_hash_map_get_size(&map, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_size_success_returns_ok) {
  size_t size = 22;
  rcutils_ret_t ret = rcutils_hash_map_get_size(&map, &size);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((size_t)0, size);
}

TEST_F(HashMapPreInitTest, set_map_null_fails) {
  uint32_t key = 2, data = 22;
  rcutils_ret_t ret = rcutils_hash_map_set(NULL, &key, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, set_map_not_init_fails) {
  uint32_t key = 2, data = 22;
  rcutils_ret_t ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, set_key_null_fails) {
  uint32_t data = 22;
  rcutils_ret_t ret = rcutils_hash_map_set(&map, NULL, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, set_data_null_fails) {
  uint32_t key = 22;
  rcutils_ret_t ret = rcutils_hash_map_set(&map, &key, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, set_data_success_returns_ok) {
  uint32_t key = 2, data = 22;
  rcutils_ret_t ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, set_data_two_times_success_returns_ok) {
  uint32_t key = 2, data = 22, ret_data = 0;
  rcutils_ret_t ret;
  ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_hash_map_get(&map, &key, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(ret_data, 22u);

  data = 23;
  ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_hash_map_get(&map, &key, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(ret_data, 23u);
}

TEST_F(HashMapPreInitTest, unset_map_null_fails) {
  uint32_t key = 2;
  rcutils_ret_t ret = rcutils_hash_map_unset(NULL, &key);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, unset_map_not_init_fails) {
  uint32_t key = 2;
  rcutils_ret_t ret = rcutils_hash_map_unset(&map, &key);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, unset_key_null_fails) {
  rcutils_ret_t ret = rcutils_hash_map_unset(&map, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, unset_key_not_in_map_success) {
  uint32_t key = 22;
  rcutils_ret_t ret = rcutils_hash_map_unset(&map, &key);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, unset_key_in_map_success) {
  uint32_t key = 2, data = 22;

  rcutils_ret_t ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_hash_map_unset(&map, &key);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, key_exists_map_null_returns_false) {
  uint32_t key = 2;
  bool ret = rcutils_hash_map_key_exists(NULL, &key);
  EXPECT_FALSE(ret);
}

TEST_F(HashMapBaseTest, key_exists_map_not_init_returns_false) {
  uint32_t key = 2;
  bool ret = rcutils_hash_map_key_exists(&map, &key);
  EXPECT_FALSE(ret);
}

TEST_F(HashMapPreInitTest, key_exists_key_null_returns_false) {
  bool ret = rcutils_hash_map_key_exists(&map, NULL);
  EXPECT_FALSE(ret);
}

TEST_F(HashMapPreInitTest, key_exists_key_not_in_map_returns_false) {
  uint32_t key = 22;
  bool ret = rcutils_hash_map_key_exists(&map, &key);
  EXPECT_FALSE(ret);
}

TEST_F(HashMapPreInitTest, key_exists_key_in_map_returns_true) {
  uint32_t key = 2, data = 22;

  rcutils_ret_t ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  bool ret_bool = rcutils_hash_map_key_exists(&map, &key);
  EXPECT_TRUE(ret_bool);
}

TEST_F(HashMapPreInitTest, get_map_null_fails) {
  uint32_t key = 2, data = 0;
  rcutils_ret_t ret = rcutils_hash_map_get(NULL, &key, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, get_map_not_init_fails) {
  uint32_t key = 2, data = 0;
  rcutils_ret_t ret = rcutils_hash_map_get(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_key_null_fails) {
  uint32_t data = 0;
  rcutils_ret_t ret = rcutils_hash_map_get(&map, NULL, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_data_null_fails) {
  uint32_t key = 22;
  rcutils_ret_t ret = rcutils_hash_map_get(&map, &key, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_key_not_in_map_fails) {
  uint32_t key = 22, data = 0;
  rcutils_ret_t ret = rcutils_hash_map_get(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_NOT_FOUND, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_key_in_map_sets_data) {
  uint32_t key = 2, data = 22, ret_data = 0;

  rcutils_ret_t ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_hash_map_get(&map, &key, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(data, ret_data);
}

TEST_F(HashMapPreInitTest, get_next_key_and_data_map_null_fails) {
  uint32_t previous_key = 1, key = 0, data = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_next_key_and_data(NULL, &previous_key, &key, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, get_next_key_and_data_map_not_init_fails) {
  uint32_t previous_key = 1, key = 0, data = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_next_key_and_data(&map, &previous_key, &key, &data);
  EXPECT_EQ(RCUTILS_RET_NOT_INITIALIZED, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_next_key_and_data_key_null_fails) {
  uint32_t previous_key = 1, data = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_next_key_and_data(&map, &previous_key, NULL, &data);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_next_key_and_data_data_null_fails) {
  uint32_t previous_key = 1, key = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_next_key_and_data(&map, &previous_key, &key, NULL);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_next_key_and_data_key_not_in_map_fails) {
  uint32_t previous_key = 1, key = 0, data = 0;
  rcutils_ret_t ret = rcutils_hash_map_get_next_key_and_data(&map, &previous_key, &key, &data);
  EXPECT_EQ(RCUTILS_RET_NOT_FOUND, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapPreInitTest, get_next_key_and_data_working) {
  uint32_t key = 1, data = 3, ret_key = 0, ret_data = 0, last_key = 0;

  rcutils_ret_t ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  key++;
  data++;
  ret = rcutils_hash_map_set(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  // Get the first entry, but there's no guaranteed ordering
  ret = rcutils_hash_map_get_next_key_and_data(&map, NULL, &ret_key, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_TRUE(1 == ret_key || 2 == ret_key);  // we only put these two keys in the map
  if (1 == ret_key) {
    EXPECT_EQ((uint32_t)3, ret_data);
  } else {
    EXPECT_EQ((uint32_t)4, ret_data);
  }
  last_key = ret_key;

  // Get the next entry
  ret = rcutils_hash_map_get_next_key_and_data(&map, &ret_key, &ret_key, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_TRUE(1 == ret_key || 2 == ret_key);  // we only put these two keys in the map
  EXPECT_NE(last_key, ret_key);
  if (1 == ret_key) {
    EXPECT_EQ((uint32_t)3, ret_data);
  } else {
    EXPECT_EQ((uint32_t)4, ret_data);
  }

  // There should be no more keys beyond the last one
  ret = rcutils_hash_map_get_next_key_and_data(&map, &ret_key, &ret_key, &ret_data);
  EXPECT_EQ(RCUTILS_RET_HASH_MAP_NO_MORE_ENTRIES, ret) << rcutils_get_error_string().str;
}

/* Use BaseTest as the fixture here so we can control the initial capacity independent of the
 * other tests
 */
TEST_F(HashMapBaseTest, growing_the_map_beyond_initial_capacity) {
  size_t capacity = 0;
  uint32_t key = 22, data = 0;
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 2, sizeof(uint32_t), sizeof(uint32_t),
    test_hash_map_uint32_hash_func, test_uint32_cmp, &allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  for (uint32_t i = 0; i < 50; ++i) {
    ret = rcutils_hash_map_set(&map, &i, &i);
    EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  }

  ret = rcutils_hash_map_get_capacity(&map, &capacity);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  // the reserved capacity should be greater than 50 due to the Load Factor
  EXPECT_LT((size_t)50, capacity);

  // Retrieve some data to make sure it wasn't lost while growing the map
  ret = rcutils_hash_map_get(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(key, data);

  key++;
  ret = rcutils_hash_map_get(&map, &key, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ(key, data);

  ret = rcutils_hash_map_fini(&map);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}

TEST_F(HashMapBaseTest, string_keys) {
  uint32_t data = 1, ret_data = 0;
  const char * key1 = "one";
  const char * key2 = "two";
  const char * lookup_key = "one";
  rcutils_ret_t ret = rcutils_hash_map_init(
    &map, 10, sizeof(char *), sizeof(uint32_t),
    rcutils_hash_map_string_hash_func, rcutils_hash_map_string_cmp_func,
    &allocator);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_hash_map_set(&map, &key1, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  data++;
  ret = rcutils_hash_map_set(&map, &key2, &data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;

  ret = rcutils_hash_map_get(&map, &lookup_key, &ret_data);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_EQ((uint32_t)1, ret_data);

  ret = rcutils_hash_map_fini(&map);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
}
