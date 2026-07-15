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

#include "gmock/gmock.h"

#include "osrf_testing_tools_cpp/memory_tools/testing_helpers.hpp"

#include "rcutils/allocator.h"

#include "rmw/error_handling.h"
#include "rmw/types.h"

#include "./time_bomb_allocator_testing_utils.h"

TEST(test_serialized_message, default_initialization) {
  auto serialized_msg = rmw_get_zero_initialized_serialized_message();

  auto allocator = rcutils_get_default_allocator();
  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_init(&serialized_msg, 0, &allocator));
  EXPECT_EQ(0u, serialized_msg.buffer_capacity);
  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_fini(&serialized_msg));
  EXPECT_EQ(0u, serialized_msg.buffer_capacity);
  EXPECT_FALSE(serialized_msg.buffer);
}

TEST(test_serialized_message, bad_allocation_on_init) {
  rmw_serialized_message_t serialized_message =
    rmw_get_zero_initialized_serialized_message();
  rcutils_allocator_t failing_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(failing_allocator, 0);
  set_time_bomb_allocator_realloc_count(failing_allocator, 0);
  EXPECT_EQ(
    RMW_RET_BAD_ALLOC, rmw_serialized_message_init(
      &serialized_message, 1lu, &failing_allocator));
  rmw_reset_error();
}

TEST(test_serialized_message, init_with_bad_arguments) {
  rmw_serialized_message_t serialized_message =
    rmw_get_zero_initialized_serialized_message();
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT, rmw_serialized_message_init(
      nullptr, 0lu, &default_allocator));
  rmw_reset_error();

  rcutils_allocator_t invalid_allocator =
    rcutils_get_zero_initialized_allocator();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT, rmw_serialized_message_init(
      &serialized_message, 0lu, &invalid_allocator));
  rmw_reset_error();
}

TEST(test_serialized_message, fini_with_bad_arguments) {
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_serialized_message_fini(nullptr));
  rmw_reset_error();

  rmw_serialized_message_t serialized_message =
    rmw_get_zero_initialized_serialized_message();
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_serialized_message_fini(&serialized_message));
  rmw_reset_error();
}

TEST(test_serialized_message, resize_with_bad_arguments) {
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_serialized_message_resize(nullptr, 1lu));
  rmw_reset_error();

  rmw_serialized_message_t zero_initialized_serialized_message =
    rmw_get_zero_initialized_serialized_message();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_serialized_message_resize(&zero_initialized_serialized_message, 1lu));
  rmw_reset_error();

  rmw_serialized_message_t serialized_message =
    rmw_get_zero_initialized_serialized_message();
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 1lu, &default_allocator)) <<
    rmw_get_error_string().str;

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_serialized_message_resize(&serialized_message, 0lu));
  rmw_reset_error();

  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) <<
    rmw_get_error_string().str;
}

TEST(test_serialized_message, bad_allocation_on_resize) {
  rmw_serialized_message_t serialized_message =
    rmw_get_zero_initialized_serialized_message();
  rcutils_allocator_t failing_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(failing_allocator, 0);
  set_time_bomb_allocator_realloc_count(failing_allocator, 0);
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &failing_allocator)) << rmw_get_error_string().str;

  EXPECT_EQ(
    RMW_RET_BAD_ALLOC,
    rmw_serialized_message_resize(&serialized_message, 1lu));
  rmw_reset_error();

  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) <<
    rmw_get_error_string().str;
}

TEST(test_serialized_message, init_resize_fini) {
  rmw_serialized_message_t serialized_message =
    rmw_get_zero_initialized_serialized_message();
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  constexpr size_t serialized_message_size = 5lu;

  rmw_ret_t ret = rmw_serialized_message_init(
    &serialized_message, serialized_message_size, &default_allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(serialized_message_size, serialized_message.buffer_capacity);
  EXPECT_EQ(0lu, serialized_message.buffer_length);

  for (size_t i = 0; i < serialized_message_size; ++i) {
    uint8_t c = 1u << i;
    memcpy(serialized_message.buffer + i, &c, 1);
  }
  serialized_message.buffer_length = serialized_message_size;
  for (size_t i = 0; i < serialized_message.buffer_length; ++i) {
    EXPECT_EQ(1u << i, serialized_message.buffer[i]);
  }

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rmw_serialized_message_resize(&serialized_message, serialized_message_size);
  });
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(serialized_message_size, serialized_message.buffer_capacity);
  EXPECT_EQ(serialized_message_size, serialized_message.buffer_length);
  for (size_t i = 0; i < serialized_message.buffer_length; ++i) {
    EXPECT_EQ(1u << i, serialized_message.buffer[i]);
  }

  constexpr size_t serialized_message_new_size = 2 * serialized_message_size;
  ret = rmw_serialized_message_resize(&serialized_message, serialized_message_new_size);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(serialized_message_new_size, serialized_message.buffer_capacity);
  EXPECT_EQ(serialized_message_size, serialized_message.buffer_length);

  for (size_t i = 0; i < serialized_message_new_size; ++i) {
    uint8_t c = 0xFF - static_cast<uint8_t>(i);
    memcpy(serialized_message.buffer + i, &c, 1);
  }
  serialized_message.buffer_length = serialized_message_new_size;
  for (size_t i = 0; i < serialized_message_new_size; ++i) {
    EXPECT_EQ(0xFF - i, serialized_message.buffer[i]);
  }

  ret = rmw_serialized_message_resize(&serialized_message, 3);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(3u, serialized_message.buffer_capacity);
  EXPECT_EQ(3u, serialized_message.buffer_length);
  EXPECT_EQ(0xFF, serialized_message.buffer[0]);
  EXPECT_EQ(0xFF - 1, serialized_message.buffer[1]);
  EXPECT_EQ(0xFF - 2, serialized_message.buffer[2]);
  // the other fields are garbage.

  // cleanup only 3 fields
  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) <<
    rmw_get_error_string().str;
}
