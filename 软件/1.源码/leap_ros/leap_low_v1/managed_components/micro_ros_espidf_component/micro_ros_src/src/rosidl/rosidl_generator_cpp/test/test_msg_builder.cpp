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

#include <gtest/gtest.h>

#include "rosidl_generator_cpp/msg/detail/arrays__builder.hpp"
#include "rosidl_generator_cpp/msg/detail/basic_types__builder.hpp"
#include "rosidl_generator_cpp/msg/detail/empty__builder.hpp"
#include "rosidl_generator_cpp/msg/detail/multi_nested__builder.hpp"
#include "rosidl_generator_cpp/msg/detail/nested__builder.hpp"

TEST(Test_msg_initialization, build) {
  ::rosidl_generator_cpp::msg::BasicTypes basic =
    ::rosidl_generator_cpp::build<::rosidl_generator_cpp::msg::BasicTypes>()
    .bool_value(true)
    .byte_value(5)
    .char_value(10)
    .float32_value(0.1125f)
    .float64_value(0.01125)
    .int8_value(-5)
    .uint8_value(20)
    .int16_value(-100)
    .uint16_value(200)
    .int32_value(-3000L)
    .uint32_value(60000UL)
    .int64_value(-4000000LL)
    .uint64_value(5000000ULL);

  ASSERT_TRUE(basic.bool_value);
  ASSERT_EQ(5, basic.byte_value);
  ASSERT_EQ(10, basic.char_value);
  ASSERT_EQ(0.1125f, basic.float32_value);
  ASSERT_EQ(0.01125, basic.float64_value);
  ASSERT_EQ(-5, basic.int8_value);
  ASSERT_EQ(20, basic.uint8_value);
  ASSERT_EQ(-100, basic.int16_value);
  ASSERT_EQ(200, basic.uint16_value);
  ASSERT_EQ(-3000L, basic.int32_value);
  ASSERT_EQ(60000UL, basic.uint32_value);
  ASSERT_EQ(-4000000LL, basic.int64_value);
  ASSERT_EQ(5000000ULL, basic.uint64_value);

  rosidl_generator_cpp::msg::Nested nested =
    rosidl_generator_cpp::build<rosidl_generator_cpp::msg::Nested>()
    .basic_types_value(
  {
    rosidl_generator_cpp::build<rosidl_generator_cpp::msg::BasicTypes>()
    .bool_value(false)
    .byte_value(10)
    .char_value(20)
    .float32_value(0.225f)
    .float64_value(0.0225)
    .int8_value(-10)
    .uint8_value(40)
    .int16_value(-200)
    .uint16_value(400)
    .int32_value(-6000L)
    .uint32_value(120000UL)
    .int64_value(-8000000LL)
    .uint64_value(10000000ULL)});

  ASSERT_FALSE(nested.basic_types_value.bool_value);
  ASSERT_EQ(10, nested.basic_types_value.byte_value);
  ASSERT_EQ(20, nested.basic_types_value.char_value);
  ASSERT_EQ(0.225f, nested.basic_types_value.float32_value);
  ASSERT_EQ(0.0225, nested.basic_types_value.float64_value);
  ASSERT_EQ(-10, nested.basic_types_value.int8_value);
  ASSERT_EQ(40, nested.basic_types_value.uint8_value);
  ASSERT_EQ(-200, nested.basic_types_value.int16_value);
  ASSERT_EQ(400, nested.basic_types_value.uint16_value);
  ASSERT_EQ(-6000L, nested.basic_types_value.int32_value);
  ASSERT_EQ(120000UL, nested.basic_types_value.uint32_value);
  ASSERT_EQ(-8000000LL, nested.basic_types_value.int64_value);
  ASSERT_EQ(10000000ULL, nested.basic_types_value.uint64_value);

  rosidl_generator_cpp::msg::Constants constants;
  rosidl_generator_cpp::msg::Defaults defaults;

  rosidl_generator_cpp::msg::Arrays arrays =
    rosidl_generator_cpp::build<rosidl_generator_cpp::msg::Arrays>()
    .bool_values({{true, false, true}})
    .byte_values({{5, 10, 5}})
    .char_values({{10, 20, 10}})
    .float32_values({{0.1125f, 0.225f, 0.1125f}})
    .float64_values({{0.01125, 0.0225, 0.01125}})
    .int8_values({{-5, -10, -5}})
    .uint8_values({{20, 40, 20}})
    .int16_values({{-100, -200, -100}})
    .uint16_values({{200, 400, 200}})
    .int32_values({{-3000L, -6000L, -3000L}})
    .uint32_values({{60000UL, 120000UL, 60000UL}})
    .int64_values({{-4000000LL, -8000000LL, -4000000LL}})
    .uint64_values({{5000000ULL, 10000000ULL, 5000000ULL}})
    .string_values({{"test", "test test", "test"}})
    .basic_types_values({{basic, basic, basic}})
    .constants_values({{constants, constants, constants}})
    .defaults_values({{defaults, defaults, defaults}})
    .bool_values_default({{false, true, false}})
    .byte_values_default({{10, 5, 10}})
    .char_values_default({{20, 10, 20}})
    .float32_values_default({{0.225f, 0.1125f, 0.225f}})
    .float64_values_default({{0.0225, 0.01125, 0.0225}})
    .int8_values_default({{-10, -5, -10}})
    .uint8_values_default({{40, 20, 40}})
    .int16_values_default({{-200, -100, -200}})
    .uint16_values_default({{40, 20, 40}})
    .int32_values_default({{-6000L, -3000L, -6000L}})
    .uint32_values_default({{120000UL, 60000UL, 120000UL}})
    .int64_values_default({{-8000000LL, -4000000LL, -8000000LL}})
    .uint64_values_default({{10000000ULL, 5000000ULL, 10000000ULL}})
    .string_values_default({{"test test", "test", "test test"}})
    .alignment_check(0l);

  rosidl_generator_cpp::msg::BoundedSequences bounded_sequences;
  rosidl_generator_cpp::msg::UnboundedSequences unbounded_sequences;

  rosidl_generator_cpp::msg::MultiNested multi_nested =
    rosidl_generator_cpp::build<rosidl_generator_cpp::msg::MultiNested>()
    .array_of_arrays({{arrays, arrays, arrays}})
    .array_of_bounded_sequences(
    {{
      bounded_sequences,
      bounded_sequences,
      bounded_sequences
    }})
    .array_of_unbounded_sequences(
    {{
      unbounded_sequences,
      unbounded_sequences,
      unbounded_sequences
    }})
    .bounded_sequence_of_arrays({arrays, arrays})
    .bounded_sequence_of_bounded_sequences(
  {
    bounded_sequences,
    bounded_sequences
  })
    .bounded_sequence_of_unbounded_sequences(
  {
    unbounded_sequences,
    unbounded_sequences
  })
    .unbounded_sequence_of_arrays({arrays})
    .unbounded_sequence_of_bounded_sequences({bounded_sequences})
    .unbounded_sequence_of_unbounded_sequences({unbounded_sequences});

  for (const auto & a : multi_nested.array_of_arrays) {
    ASSERT_EQ(arrays, a);
  }

  for (const auto & b : multi_nested.array_of_bounded_sequences) {
    ASSERT_EQ(bounded_sequences, b);
  }

  for (const auto & u : multi_nested.array_of_unbounded_sequences) {
    ASSERT_EQ(unbounded_sequences, u);
  }

  ASSERT_EQ(2u, multi_nested.bounded_sequence_of_arrays.size());
  for (const auto & a : multi_nested.bounded_sequence_of_arrays) {
    ASSERT_EQ(arrays, a);
  }

  ASSERT_EQ(2u, multi_nested.bounded_sequence_of_bounded_sequences.size());
  for (const auto & b : multi_nested.bounded_sequence_of_bounded_sequences) {
    ASSERT_EQ(bounded_sequences, b);
  }

  ASSERT_EQ(2u, multi_nested.bounded_sequence_of_unbounded_sequences.size());
  for (const auto & u : multi_nested.bounded_sequence_of_unbounded_sequences) {
    ASSERT_EQ(unbounded_sequences, u);
  }

  ASSERT_EQ(1u, multi_nested.unbounded_sequence_of_arrays.size());
  for (const auto & a : multi_nested.unbounded_sequence_of_arrays) {
    ASSERT_EQ(arrays, a);
  }

  ASSERT_EQ(1u, multi_nested.unbounded_sequence_of_bounded_sequences.size());
  for (const auto & b : multi_nested.unbounded_sequence_of_bounded_sequences) {
    ASSERT_EQ(bounded_sequences, b);
  }

  ASSERT_EQ(1u, multi_nested.unbounded_sequence_of_unbounded_sequences.size());
  for (const auto & u : multi_nested.unbounded_sequence_of_unbounded_sequences) {
    ASSERT_EQ(unbounded_sequences, u);
  }

  rosidl_generator_cpp::msg::Empty empty =
    rosidl_generator_cpp::build<rosidl_generator_cpp::msg::Empty>();
  ASSERT_EQ(0, empty.structure_needs_at_least_one_member);
}
