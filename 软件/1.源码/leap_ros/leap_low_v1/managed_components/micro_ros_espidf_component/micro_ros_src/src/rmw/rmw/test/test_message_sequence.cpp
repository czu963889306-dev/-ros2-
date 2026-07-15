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

#include "gmock/gmock.h"

#include "rcutils/allocator.h"

#include "./time_bomb_allocator_testing_utils.h"
#include "rmw/error_handling.h"
#include "rmw/message_sequence.h"

TEST(test_message_info_sequence, default_initialization) {
  auto info_sequence = rmw_get_zero_initialized_message_info_sequence();
  auto allocator = rcutils_get_default_allocator();

  // Initialize the message sequence with a zero length
  EXPECT_EQ(RMW_RET_OK, rmw_message_info_sequence_init(&info_sequence, 0u, &allocator));
  EXPECT_EQ(0u, info_sequence.size);
  EXPECT_EQ(0u, info_sequence.capacity);
  EXPECT_EQ(nullptr, info_sequence.data);

  EXPECT_EQ(RMW_RET_OK, rmw_message_info_sequence_fini(&info_sequence));
  EXPECT_EQ(0u, info_sequence.size);
  EXPECT_EQ(0u, info_sequence.capacity);
  EXPECT_EQ(nullptr, info_sequence.data);
}

TEST(test_message_info_sequence, initialization_with_size) {
  auto info_sequence = rmw_get_zero_initialized_message_info_sequence();
  auto allocator = rcutils_get_default_allocator();

  EXPECT_EQ(RMW_RET_OK, rmw_message_info_sequence_init(&info_sequence, 5u, &allocator));
  EXPECT_EQ(0u, info_sequence.size);
  EXPECT_EQ(5u, info_sequence.capacity);
  EXPECT_NE(nullptr, info_sequence.data);

  EXPECT_EQ(RMW_RET_OK, rmw_message_info_sequence_fini(&info_sequence));
  EXPECT_EQ(0u, info_sequence.size);
  EXPECT_EQ(0u, info_sequence.capacity);
  EXPECT_EQ(nullptr, info_sequence.data);
}

TEST(test_message_info_sequence, bad_arguments) {
  auto info_sequence = rmw_get_zero_initialized_message_info_sequence();
  auto allocator = rcutils_get_default_allocator();

  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_message_info_sequence_init(nullptr, 5u, &allocator));
  EXPECT_EQ(0u, info_sequence.size);
  EXPECT_EQ(0u, info_sequence.capacity);
  EXPECT_EQ(nullptr, info_sequence.data);
  rmw_reset_error();

  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_message_info_sequence_init(&info_sequence, 5u, nullptr));
  EXPECT_EQ(0u, info_sequence.size);
  EXPECT_EQ(0u, info_sequence.capacity);
  EXPECT_EQ(nullptr, info_sequence.data);
  rmw_reset_error();

  rcutils_allocator_t failing_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(failing_allocator, 0);
  EXPECT_EQ(
    RMW_RET_BAD_ALLOC, rmw_message_info_sequence_init(&info_sequence, 5u, &failing_allocator));
  EXPECT_EQ(0u, info_sequence.size);
  EXPECT_EQ(0u, info_sequence.capacity);
  EXPECT_EQ(nullptr, info_sequence.data);
  rmw_reset_error();
}

TEST(test_message_sequence, default_initialization) {
  auto message_sequence = rmw_get_zero_initialized_message_sequence();
  auto allocator = rcutils_get_default_allocator();

  // Initialize the message sequence with a zero length
  EXPECT_EQ(RMW_RET_OK, rmw_message_sequence_init(&message_sequence, 0u, &allocator));
  EXPECT_EQ(0u, message_sequence.size);
  EXPECT_EQ(0u, message_sequence.capacity);
  EXPECT_EQ(nullptr, message_sequence.data);

  EXPECT_EQ(RMW_RET_OK, rmw_message_sequence_fini(&message_sequence));
  EXPECT_EQ(0u, message_sequence.size);
  EXPECT_EQ(0u, message_sequence.capacity);
  EXPECT_EQ(nullptr, message_sequence.data);
}

TEST(test_message_sequence, initialization_with_size) {
  auto message_sequence = rmw_get_zero_initialized_message_sequence();
  auto allocator = rcutils_get_default_allocator();

  EXPECT_EQ(RMW_RET_OK, rmw_message_sequence_init(&message_sequence, 5u, &allocator));
  EXPECT_EQ(0u, message_sequence.size);
  EXPECT_EQ(5u, message_sequence.capacity);
  EXPECT_NE(nullptr, message_sequence.data);

  EXPECT_EQ(RMW_RET_OK, rmw_message_sequence_fini(&message_sequence));
  EXPECT_EQ(0u, message_sequence.size);
  EXPECT_EQ(0u, message_sequence.capacity);
  EXPECT_EQ(nullptr, message_sequence.data);
}

TEST(test_message_sequence, bad_arguments) {
  auto message_sequence = rmw_get_zero_initialized_message_sequence();
  auto allocator = rcutils_get_default_allocator();

  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_message_sequence_init(nullptr, 5u, &allocator));
  EXPECT_EQ(0u, message_sequence.size);
  EXPECT_EQ(0u, message_sequence.capacity);
  EXPECT_EQ(nullptr, message_sequence.data);
  rmw_reset_error();

  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_message_sequence_init(&message_sequence, 5u, nullptr));
  EXPECT_EQ(0u, message_sequence.size);
  EXPECT_EQ(0u, message_sequence.capacity);
  EXPECT_EQ(nullptr, message_sequence.data);
  rmw_reset_error();

  rcutils_allocator_t failing_allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(failing_allocator, 0);
  EXPECT_EQ(
    RMW_RET_BAD_ALLOC, rmw_message_sequence_init(&message_sequence, 5u, &failing_allocator));
  EXPECT_EQ(0u, message_sequence.size);
  EXPECT_EQ(0u, message_sequence.capacity);
  EXPECT_EQ(nullptr, message_sequence.data);
  rmw_reset_error();
}
