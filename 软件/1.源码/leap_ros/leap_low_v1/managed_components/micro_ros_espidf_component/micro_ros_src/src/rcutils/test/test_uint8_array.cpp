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

#include "rcutils/allocator.h"

#include "rcutils/types/uint8_array.h"

TEST(test_uint8_array, default_initialization) {
  auto uint8_array = rcutils_get_zero_initialized_uint8_array();

  auto allocator = rcutils_get_default_allocator();
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_uint8_array_init(&uint8_array, 0, &allocator));
  EXPECT_EQ(0lu, uint8_array.buffer_capacity);
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_uint8_array_fini(&uint8_array));
  EXPECT_EQ(0lu, uint8_array.buffer_capacity);
  EXPECT_FALSE(uint8_array.buffer);
}

TEST(test_uint8_array, resize) {
  auto uint8_array = rcutils_get_zero_initialized_uint8_array();
  auto allocator = rcutils_get_default_allocator();
  auto ret = rcutils_uint8_array_init(&uint8_array, 5, &allocator);
  ASSERT_EQ(RCUTILS_RET_OK, ret);

  for (size_t i = 0; i < 5; ++i) {
    uint8_t c = 0xFF;
    memcpy(uint8_array.buffer + i, &c, 1);
  }
  uint8_array.buffer_length = 5;
  for (size_t i = 0; i < uint8_array.buffer_length; ++i) {
    EXPECT_EQ(0xFF, uint8_array.buffer[i]);
  }

  ret = rcutils_uint8_array_resize(&uint8_array, 0);
  ASSERT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  EXPECT_EQ(5lu, uint8_array.buffer_capacity);
  EXPECT_EQ(5lu, uint8_array.buffer_length);

  ret = rcutils_uint8_array_resize(&uint8_array, 10);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  EXPECT_EQ(10u, uint8_array.buffer_capacity);
  EXPECT_EQ(5u, uint8_array.buffer_length);

  for (uint8_t i = 0; i < 10; ++i) {
    uint8_t u = static_cast<uint8_t>(0xFF - i);
    memcpy(uint8_array.buffer + i, &u, 1);
  }
  uint8_array.buffer_length = 10lu;
  for (size_t i = 0; i < uint8_array.buffer_length; ++i) {
    uint8_t u = static_cast<uint8_t>(0xFF - i);
    EXPECT_EQ(u, uint8_array.buffer[i]);
  }

  ret = rcutils_uint8_array_resize(&uint8_array, 3);
  ASSERT_EQ(RCUTILS_RET_OK, ret);
  EXPECT_EQ(3lu, uint8_array.buffer_capacity);
  EXPECT_EQ(3lu, uint8_array.buffer_length);
  EXPECT_EQ(0xFF, uint8_array.buffer[0]);
  EXPECT_EQ(0xFF - 1, uint8_array.buffer[1]);
  EXPECT_EQ(0xFF - 2, uint8_array.buffer[2]);
  // the other fields are garbage.

  ASSERT_EQ(RCUTILS_RET_OK, rcutils_uint8_array_resize(&uint8_array, 3));

  // cleanup only 3 fields
  EXPECT_EQ(RCUTILS_RET_OK, rcutils_uint8_array_fini(&uint8_array));
}
