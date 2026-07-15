// Copyright 2015 Open Source Robotics Foundation, Inc.
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
#include <iostream>
#include <climits>
#include <cfloat>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include "test_array_generator.hpp"

#include "rosidl_generator_cpp/msg/arrays.hpp"
#include "rosidl_generator_cpp/msg/basic_types.hpp"
#include "rosidl_generator_cpp/msg/bounded_sequences.hpp"
#include "rosidl_generator_cpp/msg/constants.hpp"
#include "rosidl_generator_cpp/msg/defaults.hpp"
#include "rosidl_generator_cpp/msg/empty.hpp"
#include "rosidl_generator_cpp/msg/multi_nested.hpp"
#include "rosidl_generator_cpp/msg/nested.hpp"
#include "rosidl_generator_cpp/msg/strings.hpp"
#include "rosidl_generator_cpp/msg/unbounded_sequences.hpp"
#include "rosidl_generator_cpp/msg/w_strings.hpp"

#define SEQUENCE_SIZE 3
#define ARRAY_SIZE 3
#define BOUNDED_STRING_LENGTH 10
#define SUBMESSAGE_SEQUENCE_SIZE 3

TEST(Test_rosidl_generator_traits, has_fixed_size) {
  static_assert(
    !rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::Arrays>::value,
    "Arrays::has_fixed_size is true");

  static_assert(
    !rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::BoundedSequences>::value,
    "BoundedSequences::has_fixed_size is true");

  static_assert(
    rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::BasicTypes>::value,
    "BasicTypes::has_fixed_size is false");

  static_assert(
    rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::Constants>::value,
    "Constants::has_fixed_size is false");

  static_assert(
    rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::Defaults>::value,
    "Defaults::has_fixed_size is false");

  static_assert(
    rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::Empty>::value,
    "Empty::has_fixed_size is false");

  static_assert(
    !rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::MultiNested>::value,
    "MultiNested::has_fixed_size is true");

  static_assert(
    rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::Nested>::value,
    "Nested::has_fixed_size is false");

  static_assert(
    !rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::Strings>::value,
    "Strings::has_fixed_size is true");

  static_assert(
    !rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::UnboundedSequences>::value,
    "UnboundedSequences::has_fixed_size is true");

  static_assert(
    !rosidl_generator_traits::has_fixed_size<rosidl_generator_cpp::msg::WStrings>::value,
    "WStrings::has_fixed_size is true");
}

TEST(Test_rosidl_generator_traits, has_bounded_size) {
  static_assert(
    !rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::Arrays>::value,
    "Arrays::has_bounded_size is true");

  static_assert(
    !rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::BoundedSequences>::value,
    "BoundedSequences::has_bounded_size is true");

  static_assert(
    rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::BasicTypes>::value,
    "BasicTypes::has_bounded_size is false");

  static_assert(
    rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::Constants>::value,
    "Constants::has_bounded_size is false");

  static_assert(
    rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::Defaults>::value,
    "Defaults::has_bounded_size is false");

  static_assert(
    rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::Empty>::value,
    "Empty::has_bounded_size is false");

  static_assert(
    !rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::MultiNested>::value,
    "MultiNested::has_bounded_size is true");

  static_assert(
    rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::Nested>::value,
    "Nested::has_bounded_size is false");

  static_assert(
    !rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::Strings>::value,
    "Strings::has_bounded_size is true");

  static_assert(
    !rosidl_generator_traits::has_bounded_size<
      rosidl_generator_cpp::msg::UnboundedSequences>::value,
    "UnboundedSequences::has_bounded_size is true");

  static_assert(
    !rosidl_generator_traits::has_bounded_size<rosidl_generator_cpp::msg::WStrings>::value,
    "WStrings::has_bounded_size is true");
}

#define TEST_BASIC_TYPE_FIELD_ASSIGNMENT(Message, FieldName, InitialValue, FinalValue) \
  Message.FieldName = InitialValue; \
  ASSERT_EQ(InitialValue, Message.FieldName); \
  Message.FieldName = FinalValue; \
  ASSERT_EQ(FinalValue, Message.FieldName);

#define TEST_STRING_FIELD_ASSIGNMENT(Message, FieldName, InitialValue, FinalValue) \
  Message.FieldName = InitialValue; \
  ASSERT_STREQ(InitialValue, Message.FieldName.c_str()); \
  Message.FieldName = FinalValue; \
  ASSERT_STREQ(FinalValue, Message.FieldName.c_str());

#define TEST_WSTRING_FIELD_ASSIGNMENT(Message, FieldName, InitialValue, FinalValue) \
  Message.FieldName = InitialValue; \
  Message.FieldName = FinalValue;

void test_message_basic_types(rosidl_generator_cpp::msg::BasicTypes message)
{
// workaround for https://github.com/google/googletest/issues/322
#ifdef __linux__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion-null"
#endif
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, bool_value, false, true)
#ifdef __linux__
#pragma GCC diagnostic pop
#endif
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, byte_value, 0, 255)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, char_value, 0, UINT8_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, float32_value, FLT_MIN, FLT_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, float64_value, DBL_MIN, DBL_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, int8_value, INT8_MIN, INT8_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, uint8_value, 0, UINT8_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, int16_value, INT16_MIN, INT16_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, uint16_value, 0, UINT16_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, int32_value, INT32_MIN, INT32_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, uint32_value, 0ul, UINT32_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, int64_value, INT64_MIN, INT64_MAX)
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, uint64_value, 0ull, UINT64_MAX)
}

#define TEST_BOUNDED_SEQUENCE_TYPES( \
    Message, FieldName, BasicType, ArraySize, MinVal, MaxVal) \
  rosidl_runtime_cpp::BoundedVector<BasicType, ArraySize> pattern_ ## FieldName; \
  Message.FieldName.resize(ArraySize); \
  pattern_ ## FieldName.resize(ArraySize); \
  test_vector_fill<decltype(pattern_ ## FieldName)>( \
    &pattern_ ## FieldName, ArraySize, MinVal, MaxVal); \
  std::copy_n(pattern_ ## FieldName.begin(), Message.FieldName.size(), Message.FieldName.begin()); \
  ASSERT_EQ(pattern_ ## FieldName, Message.FieldName); \

#define TEST_BOUNDED_SEQUENCE_STRING( \
    Message, FieldName, BasicType, ArraySize, MinVal, MaxVal, MinLength, MaxLength) \
  rosidl_runtime_cpp::BoundedVector<BasicType, ArraySize> pattern_ ## FieldName; \
  Message.FieldName.resize(ArraySize); \
  pattern_ ## FieldName.resize(ArraySize); \
  test_vector_fill<decltype(pattern_ ## FieldName)>( \
    &pattern_ ## FieldName, ArraySize, MinVal, MaxVal, MinLength, MaxLength); \
  std::copy_n(pattern_ ## FieldName.begin(), Message.FieldName.size(), Message.FieldName.begin()); \
  ASSERT_EQ(pattern_ ## FieldName, Message.FieldName);

void test_message_bounded(rosidl_generator_cpp::msg::BoundedSequences message)
{
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, bool_values, bool, SEQUENCE_SIZE, \
    false, true)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, char_values, unsigned char, SEQUENCE_SIZE, \
    0, UINT8_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, byte_values, uint8_t, SEQUENCE_SIZE, \
    0, UINT8_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, float32_values, float, SEQUENCE_SIZE, \
    FLT_MIN, FLT_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, float64_values, double, SEQUENCE_SIZE, \
    DBL_MIN, DBL_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, int8_values, int8_t, SEQUENCE_SIZE, \
    INT8_MIN, INT8_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, uint8_values, uint8_t, SEQUENCE_SIZE, \
    0, UINT8_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, int16_values, int16_t, SEQUENCE_SIZE, \
    INT16_MIN, INT16_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, uint16_values, uint16_t, SEQUENCE_SIZE, \
    0, UINT16_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, int32_values, int32_t, SEQUENCE_SIZE, \
    INT32_MIN, INT32_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, uint32_values, uint32_t, SEQUENCE_SIZE, \
    0, UINT32_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, int64_values, int64_t, SEQUENCE_SIZE, \
    INT64_MIN, INT64_MAX)
  TEST_BOUNDED_SEQUENCE_TYPES(
    message, uint64_values, uint64_t, SEQUENCE_SIZE, \
    0, UINT64_MAX)
  TEST_BOUNDED_SEQUENCE_STRING(
    message, string_values, std::string, SEQUENCE_SIZE, \
    0, UINT32_MAX, 0, BOUNDED_STRING_LENGTH)
}

#define TEST_UNBOUNDED_SEQUENCE_TYPES( \
    Message, FieldName, PrimitiveType, ArraySize, MinVal, MaxVal) \
  std::vector<PrimitiveType> pattern_ ## FieldName(ArraySize); \
  pattern_ ## FieldName.resize(ArraySize); \
  test_vector_fill<decltype(pattern_ ## FieldName)>( \
    &pattern_ ## FieldName, ArraySize, MinVal, MaxVal); \
  Message.FieldName.resize(ArraySize); \
  std::copy_n(pattern_ ## FieldName.begin(), ArraySize, Message.FieldName.begin()); \
  ASSERT_EQ(pattern_ ## FieldName, Message.FieldName); \

#define TEST_UNBOUNDED_SEQUENCE_STRING( \
    Message, FieldName, PrimitiveType, ArraySize, MinVal, MaxVal, MinLength, MaxLength) \
  std::vector<PrimitiveType> pattern_ ## FieldName; \
  Message.FieldName.resize(ArraySize); \
  pattern_ ## FieldName.resize(ArraySize); \
  test_vector_fill<decltype(pattern_ ## FieldName)>( \
    &pattern_ ## FieldName, ArraySize, MinVal, MaxVal, MinLength, MaxLength); \
  std::copy_n(pattern_ ## FieldName.begin(), Message.FieldName.size(), Message.FieldName.begin()); \
  ASSERT_EQ(pattern_ ## FieldName, Message.FieldName); \

void test_message_unbounded(rosidl_generator_cpp::msg::UnboundedSequences message)
{
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, bool_values, bool, SEQUENCE_SIZE, \
    false, true)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, char_values, unsigned char, SEQUENCE_SIZE, \
    0, UINT8_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, byte_values, uint8_t, SEQUENCE_SIZE, \
    0, UINT8_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, float32_values, float, SEQUENCE_SIZE, \
    FLT_MIN, FLT_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, float64_values, double, SEQUENCE_SIZE, \
    DBL_MIN, DBL_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, int8_values, int8_t, SEQUENCE_SIZE, \
    INT8_MIN, INT8_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, uint8_values, uint8_t, SEQUENCE_SIZE, \
    0, UINT8_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, int16_values, int16_t, SEQUENCE_SIZE, \
    INT16_MIN, INT16_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, uint16_values, uint16_t, SEQUENCE_SIZE, \
    0, UINT16_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, int32_values, int32_t, SEQUENCE_SIZE, \
    INT32_MIN, INT32_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, uint32_values, uint32_t, SEQUENCE_SIZE, \
    0, UINT32_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, int64_values, int64_t, SEQUENCE_SIZE, \
    INT64_MIN, INT64_MAX)
  TEST_UNBOUNDED_SEQUENCE_TYPES(
    message, uint64_values, uint64_t, SEQUENCE_SIZE, \
    0, UINT64_MAX)
  TEST_UNBOUNDED_SEQUENCE_STRING(
    message, string_values, std::string, SEQUENCE_SIZE, \
    0, UINT32_MAX, 0, UINT16_MAX)
}

#define TEST_ARRAY_TYPES( \
    Message, FieldName, PrimitiveType, ArraySize, MinVal, MaxVal) \
  std::array<PrimitiveType, ArraySize> pattern_ ## FieldName; \
  test_vector_fill<decltype(pattern_ ## FieldName)>( \
    &pattern_ ## FieldName, ArraySize, MinVal, MaxVal); \
  std::copy_n(pattern_ ## FieldName.begin(), ArraySize, Message.FieldName.begin()); \
  ASSERT_EQ(pattern_ ## FieldName, Message.FieldName); \

void test_message_arrays(rosidl_generator_cpp::msg::Arrays message)
{
  TEST_ARRAY_TYPES(
    message, bool_values, bool, ARRAY_SIZE, \
    false, true)
  TEST_ARRAY_TYPES(
    message, char_values, unsigned char, ARRAY_SIZE, \
    0, UINT8_MAX)
  TEST_ARRAY_TYPES(
    message, byte_values, uint8_t, ARRAY_SIZE, \
    0, UINT8_MAX)
  TEST_ARRAY_TYPES(
    message, float32_values, float, ARRAY_SIZE, \
    FLT_MIN, FLT_MAX)
  TEST_ARRAY_TYPES(
    message, float64_values, double, ARRAY_SIZE, \
    DBL_MIN, DBL_MAX)
  TEST_ARRAY_TYPES(
    message, int8_values, int8_t, ARRAY_SIZE, \
    INT8_MIN, INT8_MAX)
  TEST_ARRAY_TYPES(
    message, uint8_values, uint8_t, ARRAY_SIZE, \
    0, UINT8_MAX)
  TEST_ARRAY_TYPES(
    message, int16_values, int16_t, ARRAY_SIZE, \
    INT16_MIN, INT16_MAX)
  TEST_ARRAY_TYPES(
    message, uint16_values, uint16_t, ARRAY_SIZE, \
    0, UINT16_MAX)
  TEST_ARRAY_TYPES(
    message, int32_values, int32_t, ARRAY_SIZE, \
    INT32_MIN, INT32_MAX)
  TEST_ARRAY_TYPES(
    message, uint32_values, uint32_t, ARRAY_SIZE, \
    0, UINT32_MAX)
  TEST_ARRAY_TYPES(
    message, int64_values, int64_t, ARRAY_SIZE, \
    INT64_MIN, INT64_MAX)
  TEST_ARRAY_TYPES(
    message, uint64_values, uint64_t, ARRAY_SIZE, \
    0, UINT64_MAX)
}

// Basic types
TEST(Test_messages, basic_types) {
  rosidl_generator_cpp::msg::BasicTypes message;
  test_message_basic_types(message);
}

// Arrays
TEST(Test_messages, arrays) {
  rosidl_generator_cpp::msg::Arrays message;
  test_message_arrays(message);
}

// Bounded sequences
TEST(Test_messages, bounded_sequences) {
  rosidl_generator_cpp::msg::BoundedSequences message;
  test_message_bounded(message);
}

// Unbounded sequences
TEST(Test_messages, unbounded_sequences) {
  rosidl_generator_cpp::msg::UnboundedSequences message;
  test_message_unbounded(message);
}

// Array of bounded sequences
TEST(Test_messages, array_bounded) {
  rosidl_generator_cpp::msg::MultiNested message;
  for (int i = 0; i < SUBMESSAGE_SEQUENCE_SIZE; i++) {
    test_message_bounded(message.array_of_bounded_sequences[i]);
  }
}

// Array of unbounded sequences
TEST(Test_messages, array_unbounded) {
  rosidl_generator_cpp::msg::MultiNested message;
  for (int i = 0; i < SUBMESSAGE_SEQUENCE_SIZE; i++) {
    test_message_unbounded(message.array_of_unbounded_sequences[i]);
  }
}

// Bounded sequence of basic types
TEST(Test_messages, bounded_sequence_basic_types) {
  rosidl_generator_cpp::msg::BoundedSequences message;
  message.basic_types_values.resize(SUBMESSAGE_SEQUENCE_SIZE);
  for (int i = 0; i < SUBMESSAGE_SEQUENCE_SIZE; i++) {
    test_message_basic_types(message.basic_types_values[i]);
  }
}

// Bounded sequence of bounded sequences
TEST(Test_messages, bounded_sequence_bounded) {
  rosidl_generator_cpp::msg::MultiNested message;
  message.bounded_sequence_of_bounded_sequences.resize(SUBMESSAGE_SEQUENCE_SIZE);
  for (int i = 0; i < SUBMESSAGE_SEQUENCE_SIZE; i++) {
    test_message_bounded(message.bounded_sequence_of_bounded_sequences[i]);
  }
}

// Bounded sequence of unbounded sequences
TEST(Test_messages, bounded_sequence_unbounded) {
  rosidl_generator_cpp::msg::MultiNested message;
  message.bounded_sequence_of_unbounded_sequences.resize(SUBMESSAGE_SEQUENCE_SIZE);
  for (int i = 0; i < SUBMESSAGE_SEQUENCE_SIZE; i++) {
    test_message_unbounded(message.bounded_sequence_of_unbounded_sequences[i]);
  }
}

// Unbounded sequence of basic types
TEST(Test_messages, unbounded_sequence_basic_types) {
  rosidl_generator_cpp::msg::UnboundedSequences message;
  message.basic_types_values.resize(SUBMESSAGE_SEQUENCE_SIZE);
  for (int i = 0; i < SUBMESSAGE_SEQUENCE_SIZE; i++) {
    test_message_basic_types(message.basic_types_values[i]);
  }
}

// Unbounded sequence of bounded sequences
TEST(Test_messages, unbounded_sequence_bounded) {
  rosidl_generator_cpp::msg::MultiNested message;
  message.unbounded_sequence_of_bounded_sequences.resize(SUBMESSAGE_SEQUENCE_SIZE);
  for (int i = 0; i < SUBMESSAGE_SEQUENCE_SIZE; i++) {
    test_message_bounded(message.unbounded_sequence_of_bounded_sequences[i]);
  }
}

// Unbounded sequence of unbounded sequences
TEST(Test_messages, unbounded_sequence_unbounded) {
  rosidl_generator_cpp::msg::MultiNested message;
  message.unbounded_sequence_of_unbounded_sequences.resize(SUBMESSAGE_SEQUENCE_SIZE);
  for (int i = 0; i < SUBMESSAGE_SEQUENCE_SIZE; i++) {
    test_message_unbounded(message.unbounded_sequence_of_unbounded_sequences[i]);
  }
}

// Constants
TEST(Test_messages, constants) {
  rosidl_generator_cpp::msg::Constants message;
  ASSERT_EQ(true, message.BOOL_CONST);
  ASSERT_EQ(50, message.BYTE_CONST);
  ASSERT_EQ(100, message.CHAR_CONST);
  ASSERT_EQ(1.125f, message.FLOAT32_CONST);
  ASSERT_EQ(1.125, message.FLOAT64_CONST);
  ASSERT_EQ(-50, message.INT8_CONST);
  ASSERT_EQ(200u, message.UINT8_CONST);
  ASSERT_EQ(-1000, message.INT16_CONST);
  ASSERT_EQ(2000u, message.UINT16_CONST);
  ASSERT_EQ(-30000, message.INT32_CONST);
  ASSERT_EQ(60000ul, message.UINT32_CONST);
  ASSERT_EQ(-40000000, message.INT64_CONST);
  ASSERT_EQ(50000000ull, message.UINT64_CONST);
}

// Defaults
TEST(Test_messages, defaults) {
  rosidl_generator_cpp::msg::Defaults message;
// workaround for https://github.com/google/googletest/issues/322
#ifdef __linux__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion-null"
#endif
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, bool_value, true, false);
#ifdef __linux__
#pragma GCC diagnostic pop
#endif
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, byte_value, 50, 255);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, char_value, 100, UINT8_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, float32_value, 1.125f, FLT_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, float64_value, 1.125, DBL_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, int8_value, -50, INT8_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, uint8_value, 200, UINT8_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, int16_value, -1000, INT16_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, uint16_value, 2000, UINT16_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, int32_value, -30000, INT32_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, uint32_value, 60000ul, UINT32_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, int64_value, -40000000, INT64_MAX);
  TEST_BASIC_TYPE_FIELD_ASSIGNMENT(message, uint64_value, 50000000ull, UINT64_MAX);
}

// String array with default values
TEST(Test_messages, string_arrays_default) {
  rosidl_generator_cpp::msg::Arrays message;
  ASSERT_STREQ("", message.string_values_default[0].c_str());
  ASSERT_STREQ("max value", message.string_values_default[1].c_str());
  ASSERT_STREQ("min value", message.string_values_default[2].c_str());
  ASSERT_EQ(3ul, message.string_values_default.size());
}

// TODO(mikaelarguedas) reenable this test when bounded strings enforce length
TEST(Test_messages, DISABLED_Test_bounded_strings) {
  rosidl_generator_cpp::msg::Strings message;
  TEST_STRING_FIELD_ASSIGNMENT(message, bounded_string_value, "", "Deep into")
  std::string tooLongString = std::string("Too long string");
  message.bounded_string_value = tooLongString;
  tooLongString.resize(BOUNDED_STRING_LENGTH);
  ASSERT_STREQ(tooLongString.c_str(), message.string_value.c_str());
}

TEST(Test_messages, Test_string) {
  rosidl_generator_cpp::msg::Strings message;
  TEST_STRING_FIELD_ASSIGNMENT(message, string_value, "", "Deep into")
}

TEST(Test_messages, Test_wstring) {
  rosidl_generator_cpp::msg::WStrings message;
  TEST_WSTRING_FIELD_ASSIGNMENT(message, wstring_value, u"", u"wstring_value_\u2122")
}

#define TEST_STATIC_ARRAY_STRING( \
    Message, FieldName, PrimitiveType, ArraySize, MinVal, MaxVal, MinLength, MaxLength) \
  std::array<PrimitiveType, ArraySize> pattern_ ## FieldName; \
  test_vector_fill<decltype(pattern_ ## FieldName)>( \
    &pattern_ ## FieldName, ArraySize, MinVal, MaxVal, MinLength, MaxLength); \
  std::copy_n(pattern_ ## FieldName.begin(), Message.FieldName.size(), Message.FieldName.begin()); \
  ASSERT_EQ(pattern_ ## FieldName, Message.FieldName); \

TEST(Test_messages, Test_string_array_static) {
  rosidl_generator_cpp::msg::Arrays message;
  TEST_STATIC_ARRAY_STRING(
    message, string_values_default, std::string, ARRAY_SIZE, \
    0, UINT32_MAX, 0, UINT16_MAX)
}
