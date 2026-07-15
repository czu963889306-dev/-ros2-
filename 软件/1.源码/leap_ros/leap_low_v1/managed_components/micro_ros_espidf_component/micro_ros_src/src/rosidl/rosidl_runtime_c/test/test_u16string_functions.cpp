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

#include "gtest/gtest.h"
#include "rosidl_runtime_c/u16string.h"
#include "rosidl_runtime_c/u16string_functions.h"

#include "rcutils/testing/fault_injection.h"

TEST(u16string_functions, init_fini_empty_string) {
  rosidl_runtime_c__U16String empty_string;
  EXPECT_TRUE(rosidl_runtime_c__U16String__init(&empty_string));
  EXPECT_EQ(empty_string.size, 0u);
  EXPECT_EQ(empty_string.capacity, 1u);
  EXPECT_EQ(empty_string.data[0], 0u);

  // Should just return without changing anything.
  rosidl_runtime_c__U16String__fini(nullptr);

  rosidl_runtime_c__U16String__fini(&empty_string);
  EXPECT_EQ(empty_string.data, nullptr);
  EXPECT_EQ(empty_string.size, 0u);
  EXPECT_EQ(empty_string.capacity, 0u);
}

TEST(u16string_functions, init_fini_bad_data) {
  EXPECT_FALSE(rosidl_runtime_c__U16String__init(nullptr));

  rosidl_runtime_c__U16String bad_string;
  bad_string.size = 1u;
  bad_string.capacity = 1u;
  bad_string.data = nullptr;

  int expected_error_code = -1;
#ifndef _WIN32
  // While exit() takes an int, when a parent process waits for a child process to die, it only
  // takes the lowest 8-bits.
  // See Note: in https://pubs.opengroup.org/onlinepubs/009695399/functions/exit.html
  expected_error_code &= 255;
#endif
  EXPECT_EXIT(
    rosidl_runtime_c__U16String__fini(&bad_string),
    ::testing::ExitedWithCode(expected_error_code),
    "Unexpected condition: string size was non-zero for deallocated data! Exiting.");
  bad_string.size = 0u;

  EXPECT_EXIT(
    rosidl_runtime_c__U16String__fini(&bad_string),
    ::testing::ExitedWithCode(expected_error_code),
    "Unexpected behavior: string capacity was non-zero for deallocated data! Exiting.");

  uint16_t i = 0;
  bad_string.data = &i;
  bad_string.capacity = 0u;

  EXPECT_EXIT(
    rosidl_runtime_c__U16String__fini(&bad_string),
    ::testing::ExitedWithCode(expected_error_code),
    "Unexpected condition: string capacity was zero for allocated data! Exiting.");
}

TEST(u16string_functions, resize_assignn) {
  rosidl_runtime_c__U16String s, t;
  constexpr size_t s_size = 3u;
  constexpr uint16_t data[s_size + 1] = {3u, 2u, 1u, 0u};

  EXPECT_TRUE(rosidl_runtime_c__U16String__init(&s));
  EXPECT_TRUE(rosidl_runtime_c__U16String__init(&t));

  EXPECT_FALSE(rosidl_runtime_c__U16String__resize(nullptr, s_size));

  // If you're here because this test crashed your computer, it might be because it just tried
  // to allocate SIZE_MAX * 2 bytes, which means someone removed an important check.
  EXPECT_FALSE(rosidl_runtime_c__U16String__resize(&s, SIZE_MAX));

  EXPECT_TRUE(rosidl_runtime_c__U16String__resize(&s, s_size));
  EXPECT_EQ(s.size, s_size);
  EXPECT_EQ(s.capacity, s_size + 1);
  EXPECT_NE(s.data, nullptr);
  EXPECT_EQ(s.data[s_size], 0u);

  EXPECT_FALSE(rosidl_runtime_c__U16String__assign(nullptr, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__assignn(nullptr, nullptr, 0));
  EXPECT_FALSE(rosidl_runtime_c__U16String__assignn(&s, nullptr, 0));
  EXPECT_FALSE(rosidl_runtime_c__U16String__assignn(nullptr, &data[0], 0));

  EXPECT_TRUE(rosidl_runtime_c__U16String__assign(&t, &data[0]));
  EXPECT_EQ(t.size, s_size);
  EXPECT_EQ(t.capacity, s_size + 1u);
  EXPECT_EQ(t.data[0], data[0]);
  EXPECT_EQ(t.data[1], data[1]);
  EXPECT_EQ(t.data[2], data[2]);
  EXPECT_EQ(t.data[3], 0u);

  EXPECT_TRUE(rosidl_runtime_c__U16String__assignn(&s, &data[0], s_size));
  EXPECT_EQ(s.size, s_size);
  EXPECT_EQ(s.capacity, s_size + 1u);
  EXPECT_EQ(s.data[0], data[0]);
  EXPECT_EQ(s.data[1], data[1]);
  EXPECT_EQ(s.data[2], data[2]);
  EXPECT_EQ(s.data[3], 0u);

  // Can't copy strings of size max
  EXPECT_FALSE(rosidl_runtime_c__U16String__assignn(&s, &data[0], SIZE_MAX));

  // Check assigning 0-length strings
  EXPECT_TRUE(rosidl_runtime_c__U16String__assignn(&s, &data[0], 0));
  EXPECT_EQ(s.size, 0u);
  EXPECT_EQ(s.capacity, 1u);
  EXPECT_EQ(s.data[0], 0u);

  constexpr size_t s8_size = 2 * s_size;
  // To avoid endian issues, these double-wide chars are the same for big/little endian
  constexpr char data_8t[s8_size + 2u] = {3, 3, 2, 2, 1, 1, 0, 0};
  // str is nullptr
  EXPECT_FALSE(rosidl_runtime_c__U16String__assignn_from_char(nullptr, nullptr, 0));
  // value is nullptr
  EXPECT_FALSE(rosidl_runtime_c__U16String__assignn_from_char(&s, nullptr, 0));
  // n is odd
  EXPECT_FALSE(rosidl_runtime_c__U16String__assignn_from_char(&s, &data_8t[0], 1u));

  EXPECT_TRUE(rosidl_runtime_c__U16String__assignn_from_char(&s, &data_8t[0], s8_size));
  EXPECT_EQ(s.size, s_size);
  EXPECT_EQ(s.capacity, s_size + 1u);
  EXPECT_EQ(s.data[0], 771u);
  EXPECT_EQ(s.data[1], 514u);
  EXPECT_EQ(s.data[2], 257u);
  EXPECT_EQ(s.data[3], 0u);

  rosidl_runtime_c__U16String__fini(&s);
  rosidl_runtime_c__U16String__fini(&t);

  // Check assign after fini without init
  EXPECT_TRUE(rosidl_runtime_c__U16String__assign(&s, &data[0]));
  rosidl_runtime_c__U16String__fini(&s);
}

TEST(u16string_functions, equality_comparison) {
  rosidl_runtime_c__U16String empty_string, foo_string, bar_string;
  EXPECT_TRUE(rosidl_runtime_c__U16String__init(&empty_string));
  EXPECT_TRUE(rosidl_runtime_c__U16String__init(&foo_string));
  const uint16_t encoded_foo_string[] = {102u, 111u, 111u, 0u};
  EXPECT_TRUE(rosidl_runtime_c__U16String__assign(&foo_string, encoded_foo_string));
  EXPECT_TRUE(rosidl_runtime_c__U16String__init(&bar_string));
  const uint16_t encoded_bar_string[] = {98u, 97u, 114u, 0u};
  EXPECT_TRUE(rosidl_runtime_c__U16String__assign(&bar_string, encoded_bar_string));

  EXPECT_FALSE(rosidl_runtime_c__U16String__are_equal(nullptr, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__are_equal(&empty_string, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__are_equal(nullptr, &empty_string));
  EXPECT_TRUE(rosidl_runtime_c__U16String__are_equal(&empty_string, &empty_string));

  EXPECT_FALSE(rosidl_runtime_c__U16String__are_equal(&empty_string, &foo_string));
  EXPECT_FALSE(rosidl_runtime_c__U16String__are_equal(&foo_string, &empty_string));
  EXPECT_TRUE(rosidl_runtime_c__U16String__are_equal(&foo_string, &foo_string));

  EXPECT_FALSE(rosidl_runtime_c__U16String__are_equal(&bar_string, &foo_string));
  EXPECT_FALSE(rosidl_runtime_c__U16String__are_equal(&foo_string, &bar_string));
  EXPECT_TRUE(rosidl_runtime_c__U16String__are_equal(&bar_string, &bar_string));

  rosidl_runtime_c__U16String__fini(&bar_string);
  rosidl_runtime_c__U16String__fini(&foo_string);
  rosidl_runtime_c__U16String__fini(&empty_string);
}

TEST(u16string_functions, copy) {
  rosidl_runtime_c__U16String input, output;

  EXPECT_FALSE(rosidl_runtime_c__U16String__copy(nullptr, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__copy(&input, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__copy(nullptr, &output));

  EXPECT_TRUE(rosidl_runtime_c__U16String__init(&input));
  const uint16_t encoded_foo_string[] = {102u, 111u, 111u, 0u};
  EXPECT_TRUE(rosidl_runtime_c__U16String__assign(&input, encoded_foo_string));
  EXPECT_TRUE(rosidl_runtime_c__U16String__init(&output));

  EXPECT_FALSE(rosidl_runtime_c__U16String__are_equal(&input, &output));
  EXPECT_TRUE(rosidl_runtime_c__U16String__copy(&input, &output));
  EXPECT_TRUE(rosidl_runtime_c__U16String__are_equal(&input, &output));

  rosidl_runtime_c__U16String__fini(&output);
  rosidl_runtime_c__U16String__fini(&input);
}

TEST(u16string_functions, init_fini_sequence) {
  rosidl_runtime_c__U16String__Sequence sequence;
  EXPECT_FALSE(rosidl_runtime_c__U16String__Sequence__init(nullptr, 0u));
  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__init(&sequence, 0u));

  EXPECT_EQ(sequence.size, 0u);
  EXPECT_EQ(sequence.capacity, 0u);
  EXPECT_EQ(sequence.data, nullptr);

  // Checking these return without issue
  rosidl_runtime_c__U16String__Sequence__fini(nullptr);
  rosidl_runtime_c__U16String__Sequence__fini(&sequence);

  constexpr size_t seq_size = 3u;
  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__init(&sequence, seq_size));
  EXPECT_EQ(sequence.size, seq_size);
  EXPECT_EQ(sequence.capacity, seq_size);
  rosidl_runtime_c__U16String__Sequence__fini(&sequence);
}

TEST(u16string_functions, create_destroy_sequence) {
  rosidl_runtime_c__U16String__Sequence * sequence =
    rosidl_runtime_c__U16String__Sequence__create(0u);
  EXPECT_NE(sequence, nullptr);
  EXPECT_EQ(sequence->data, nullptr);
  EXPECT_EQ(sequence->size, 0u);
  EXPECT_EQ(sequence->capacity, 0u);

  // Shouldn't do anything exciting
  rosidl_runtime_c__U16String__Sequence__destroy(sequence);

  constexpr size_t seq_size = 3u;
  sequence = rosidl_runtime_c__U16String__Sequence__create(seq_size);
  EXPECT_NE(sequence, nullptr);
  EXPECT_EQ(sequence->size, seq_size);
  EXPECT_EQ(sequence->capacity, seq_size);
  rosidl_runtime_c__U16String__Sequence__destroy(sequence);
}

TEST(string_functions, create_destroy_sequence_maybe_fail) {
  rosidl_runtime_c__U16String__Sequence * sequence = nullptr;
  constexpr size_t seq_size = 10u;
  RCUTILS_FAULT_INJECTION_TEST(
  {
    sequence = rosidl_runtime_c__U16String__Sequence__create(seq_size);
    if (nullptr != sequence) {
      rosidl_runtime_c__U16String__Sequence__destroy(sequence);
      sequence = nullptr;
    }
  });
}


TEST(u16string_functions, sequence_equality_comparison) {
  rosidl_runtime_c__U16String__Sequence empty_sequence;
  rosidl_runtime_c__U16String__Sequence nonempty_sequence;
  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__init(&empty_sequence, 0u));
  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__init(&nonempty_sequence, 1u));

  EXPECT_FALSE(rosidl_runtime_c__U16String__Sequence__are_equal(nullptr, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__Sequence__are_equal(&empty_sequence, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__Sequence__are_equal(nullptr, &empty_sequence));
  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__are_equal(&empty_sequence, &empty_sequence));
  EXPECT_FALSE(
    rosidl_runtime_c__U16String__Sequence__are_equal(
      &empty_sequence,
      &nonempty_sequence));
  EXPECT_FALSE(
    rosidl_runtime_c__U16String__Sequence__are_equal(
      &nonempty_sequence,
      &empty_sequence));
  EXPECT_TRUE(
    rosidl_runtime_c__U16String__Sequence__are_equal(
      &nonempty_sequence,
      &nonempty_sequence));

  rosidl_runtime_c__U16String__Sequence__fini(&empty_sequence);
  rosidl_runtime_c__U16String__Sequence__fini(&nonempty_sequence);
}

TEST(u16string_functions, copy_sequence) {
  rosidl_runtime_c__U16String__Sequence input, output;

  EXPECT_FALSE(rosidl_runtime_c__U16String__Sequence__copy(nullptr, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__Sequence__copy(&input, nullptr));
  EXPECT_FALSE(rosidl_runtime_c__U16String__Sequence__copy(nullptr, &output));

  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__init(&input, 1u));
  const uint16_t encoded_foo_string[] = {102u, 111u, 111u, 0u};
  EXPECT_TRUE(rosidl_runtime_c__U16String__assign(&input.data[0], encoded_foo_string));
  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__init(&output, 0u));

  EXPECT_FALSE(rosidl_runtime_c__U16String__Sequence__are_equal(&input, &output));
  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__copy(&input, &output));
  EXPECT_TRUE(rosidl_runtime_c__U16String__Sequence__are_equal(&input, &output));

  rosidl_runtime_c__U16String__Sequence__fini(&output);
  rosidl_runtime_c__U16String__Sequence__fini(&input);
}
