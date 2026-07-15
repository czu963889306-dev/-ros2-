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
#include <cstring>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcutils/allocator.h"

#include "rosidl_runtime_c/primitives_sequence_functions.h"

#include "rosidl_typesupport_cpp/message_type_support.hpp"

#include "rmw/rmw.h"
#include "rmw/error_handling.h"

#include "test_msgs/msg/basic_types.h"
#include "test_msgs/msg/basic_types.hpp"

#include "test_msgs/msg/bounded_plain_sequences.h"
#include "test_msgs/msg/bounded_plain_sequences.hpp"

#include "test_msgs/msg/unbounded_sequences.h"
#include "test_msgs/msg/unbounded_sequences.hpp"

#include "./allocator_testing_utils.h"

static void check_bad_cdr_sequence_cases(
  const rosidl_message_type_support_t * ts,
  void * message)
{
  // Serialized CDR buffer for a data with all sequences empty.
  constexpr size_t kBufferSize = 132;
  const uint8_t valid_data[kBufferSize] = {
    0x01, 0x00, 0x00, 0x00,  // representation header (CDR little endian)
    0x00, 0x00, 0x00, 0x00,  // bool[] bool_values
    0x00, 0x00, 0x00, 0x00,  // byte[] byte_values
    0x00, 0x00, 0x00, 0x00,  // char[] char_values
    0x00, 0x00, 0x00, 0x00,  // float32[] float32_values
    0x00, 0x00, 0x00, 0x00,  // float64[] float64_values
    0x00, 0x00, 0x00, 0x00,  // int8[] int8_values
    0x00, 0x00, 0x00, 0x00,  // uint8[] uint8_values
    0x00, 0x00, 0x00, 0x00,  // int16[] int16_values
    0x00, 0x00, 0x00, 0x00,  // uint16[] uint16_values
    0x00, 0x00, 0x00, 0x00,  // int32[] int32_values
    0x00, 0x00, 0x00, 0x00,  // uint32[] uint32_values
    0x00, 0x00, 0x00, 0x00,  // int64[] int64_values
    0x00, 0x00, 0x00, 0x00,  // uint64[] uint64_values
    0x00, 0x00, 0x00, 0x00,  // string[] string_values
    0x00, 0x00, 0x00, 0x00,  // BasicTypes[] basic_types_values
    0x00, 0x00, 0x00, 0x00,  // Constants[] constants_values
    0x00, 0x00, 0x00, 0x00,  // Defaults[] defaults_values
    0x00, 0x00, 0x00, 0x00,  // bool[] bool_values_default
    0x00, 0x00, 0x00, 0x00,  // byte[] byte_values_default
    0x00, 0x00, 0x00, 0x00,  // char[] char_values_default
    0x00, 0x00, 0x00, 0x00,  // float32[] float32_values_default
    0x00, 0x00, 0x00, 0x00,  // float64[] float64_values_default
    0x00, 0x00, 0x00, 0x00,  // int8[] int8_values_default
    0x00, 0x00, 0x00, 0x00,  // uint8[] uint8_values_default
    0x00, 0x00, 0x00, 0x00,  // int16[] int16_values_default
    0x00, 0x00, 0x00, 0x00,  // uint16[] uint16_values_default
    0x00, 0x00, 0x00, 0x00,  // int32[] int32_values_default
    0x00, 0x00, 0x00, 0x00,  // uint32[] uint32_values_default
    0x00, 0x00, 0x00, 0x00,  // int64[] int64_values_default
    0x00, 0x00, 0x00, 0x00,  // uint64[] uint64_values_default
    0x00, 0x00, 0x00, 0x00,  // string[] string_values_default
    0x00, 0x00, 0x00, 0x00   // int32 alignment_check
  };

  uint8_t buffer[kBufferSize];
  memcpy(buffer, valid_data, kBufferSize);

  // The first 4 bytes are the CDR representation header, which we don't modify.
  constexpr size_t kFirstSequenceOffset = 4;
  // The last 4 bytes are the alignment check, which we also don't modify.
  constexpr size_t kLastSequenceOffset = kBufferSize - 4;
  // Each sequence length is stored as a 4-byte unsigned integer.
  constexpr size_t kSequenceLengthSize = 4;

  for (size_t i = kFirstSequenceOffset; i < kLastSequenceOffset; i += kSequenceLengthSize) {
    // Corrupt the buffer by changing the size of a sequence to an invalid value.
    buffer[i] = 0xFF;
    buffer[i + 1] = 0xFF;
    buffer[i + 2] = 0xFF;
    buffer[i + 3] = 0xFF;

    // Expect the deserialization to fail.
    rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
    serialized_message.buffer = const_cast<uint8_t *>(buffer);
    serialized_message.buffer_length = sizeof(buffer);
    serialized_message.buffer_capacity = sizeof(buffer);
    rmw_ret_t ret = rmw_deserialize(&serialized_message, ts, message);
    EXPECT_NE(RMW_RET_OK, ret);
    rmw_reset_error();

    // Restore the buffer to a valid state.
    buffer[i] = 0x00;
    buffer[i + 1] = 0x00;
    buffer[i + 2] = 0x00;
    buffer[i + 3] = 0x00;
  }
}

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestSerializeDeserialize, RMW_IMPLEMENTATION) : public ::testing::Test
{
};

TEST(TestSerializeDeserialize, get_serialization_format) {
  const char * serialization_format = rmw_get_serialization_format();
  EXPECT_NE(nullptr, serialization_format);
  EXPECT_STREQ(serialization_format, rmw_get_serialization_format());
}

TEST_F(CLASSNAME(TestSerializeDeserialize, RMW_IMPLEMENTATION), serialize_with_bad_arguments) {
  const rosidl_message_type_support_t * ts{
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes)};
  test_msgs__msg__BasicTypes input_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&input_message));
  rcutils_allocator_t failing_allocator = get_failing_allocator();
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &failing_allocator)) << rmw_get_error_string().str;

  EXPECT_NE(RMW_RET_OK, rmw_serialize(&input_message, ts, &serialized_message));
  rmw_reset_error();

  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) <<
    rmw_get_error_string().str;

  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &default_allocator)) << rmw_get_error_string().str;

  rosidl_message_type_support_t * non_const_ts =
    const_cast<rosidl_message_type_support_t *>(ts);
  const char * typesupport_identifier = non_const_ts->typesupport_identifier;
  non_const_ts->typesupport_identifier = "not-a-typesupport-identifier";

  EXPECT_NE(RMW_RET_OK, rmw_serialize(&input_message, non_const_ts, &serialized_message));
  rmw_reset_error();

  non_const_ts->typesupport_identifier = typesupport_identifier;

  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) <<
    rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestSerializeDeserialize, RMW_IMPLEMENTATION), clean_round_trip_for_c_message) {
  const rosidl_message_type_support_t * ts{
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes)};
  test_msgs__msg__BasicTypes input_message{};
  test_msgs__msg__BasicTypes output_message{};
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&input_message));
  ASSERT_TRUE(test_msgs__msg__BasicTypes__init(&output_message));
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &default_allocator)) << rmw_get_error_string().str;

  // Make input_message not equal to output_message.
  input_message.bool_value = !output_message.bool_value;
  input_message.int16_value = output_message.int16_value - 1;
  input_message.uint32_value = output_message.uint32_value + 1000000;

  rmw_ret_t ret = rmw_serialize(&input_message, ts, &serialized_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_NE(nullptr, serialized_message.buffer);
  EXPECT_GT(serialized_message.buffer_length, 0lu);

  ret = rmw_deserialize(&serialized_message, ts, &output_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(input_message.bool_value, output_message.bool_value);
  EXPECT_EQ(input_message.int16_value, output_message.int16_value);
  EXPECT_EQ(input_message.uint32_value, output_message.uint32_value);

  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) <<
    rmw_get_error_string().str;
}

TEST_F(
  CLASSNAME(
    TestSerializeDeserialize,
    RMW_IMPLEMENTATION), clean_round_trip_for_c_bounded_message) {
  const rosidl_message_type_support_t * ts{
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BoundedPlainSequences)};
  test_msgs__msg__BoundedPlainSequences input_message{};
  test_msgs__msg__BoundedPlainSequences output_message{};
  ASSERT_TRUE(test_msgs__msg__BoundedPlainSequences__init(&input_message));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BoundedPlainSequences__fini(&input_message);
  });
  ASSERT_TRUE(test_msgs__msg__BoundedPlainSequences__init(&output_message));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__BoundedPlainSequences__fini(&output_message);
  });
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &default_allocator)) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_serialized_message_fini(
        &serialized_message)) << rmw_get_error_string().str;
  });

  // Make input_message not equal to output_message.
  ASSERT_TRUE(rosidl_runtime_c__bool__Sequence__init(&input_message.bool_values, 1));
  input_message.bool_values.data[0] = true;
  ASSERT_TRUE(rosidl_runtime_c__int16__Sequence__init(&input_message.int16_values, 1));
  input_message.int16_values.data[0] = -7;

  rmw_ret_t ret = rmw_serialize(&input_message, ts, &serialized_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_NE(nullptr, serialized_message.buffer);
  EXPECT_GT(serialized_message.buffer_length, 0lu);

  // Adding more items should increase buffer length
  auto first_message_length = serialized_message.buffer_length;
  ASSERT_TRUE(rosidl_runtime_c__int32__Sequence__init(&input_message.int32_values, 1));
  input_message.int32_values.data[0] = -1;
  ASSERT_TRUE(rosidl_runtime_c__uint16__Sequence__init(&input_message.uint16_values, 1));
  input_message.uint16_values.data[0] = 125;

  ret = rmw_serialize(&input_message, ts, &serialized_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_NE(nullptr, serialized_message.buffer);
  EXPECT_GT(serialized_message.buffer_length, 0lu);
  EXPECT_GT(serialized_message.buffer_length, first_message_length);

  ret = rmw_deserialize(&serialized_message, ts, &output_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(input_message.bool_values.size, output_message.bool_values.size);
  EXPECT_EQ(input_message.bool_values.data[0], output_message.bool_values.data[0]);
  EXPECT_EQ(input_message.int16_values.size, output_message.int16_values.size);
  EXPECT_EQ(input_message.int16_values.data[0], output_message.int16_values.data[0]);
  EXPECT_EQ(input_message.int32_values.size, output_message.int32_values.size);
  EXPECT_EQ(input_message.int32_values.data[0], output_message.int32_values.data[0]);
  EXPECT_EQ(input_message.uint16_values.size, output_message.uint16_values.size);
  EXPECT_EQ(input_message.uint16_values.data[0], output_message.uint16_values.data[0]);
}

TEST(TestSerializeDeserialize, bad_cdr_sequence_correctly_fails_for_c) {
  {
    const char * serialization_format = rmw_get_serialization_format();
    if (0 != strcmp(serialization_format, "cdr")) {
      GTEST_SKIP();
    }
  }

  const rosidl_message_type_support_t * ts{
    ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, UnboundedSequences)};
  test_msgs__msg__UnboundedSequences output_message{};
  ASSERT_TRUE(test_msgs__msg__UnboundedSequences__init(&output_message));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    test_msgs__msg__UnboundedSequences__fini(&output_message);
  });

  check_bad_cdr_sequence_cases(ts, &output_message);
}

TEST(TestSerializeDeserialize, clean_round_trip_for_cpp_message) {
  const rosidl_message_type_support_t * ts =
    rosidl_typesupport_cpp::get_message_type_support_handle<test_msgs::msg::BasicTypes>();
  test_msgs::msg::BasicTypes input_message{};
  test_msgs::msg::BasicTypes output_message{};
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &default_allocator)) << rmw_get_error_string().str;

  // Make input_message not equal to output_message.
  input_message.bool_value = !output_message.bool_value;
  input_message.int16_value = output_message.int16_value - 1;
  input_message.uint32_value = output_message.uint32_value + 1000000;

  rmw_ret_t ret = rmw_serialize(&input_message, ts, &serialized_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_NE(nullptr, serialized_message.buffer);
  EXPECT_GT(serialized_message.buffer_length, 0lu);

  ret = rmw_deserialize(&serialized_message, ts, &output_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(input_message, output_message);

  EXPECT_EQ(RMW_RET_OK, rmw_serialized_message_fini(&serialized_message)) <<
    rmw_get_error_string().str;
}

TEST_F(
  CLASSNAME(
    TestSerializeDeserialize,
    RMW_IMPLEMENTATION), clean_round_trip_for_cpp_bounded_message) {
  using TestMessage = test_msgs::msg::BoundedPlainSequences;
  const rosidl_message_type_support_t * ts =
    rosidl_typesupport_cpp::get_message_type_support_handle<TestMessage>();
  TestMessage input_message{};
  TestMessage output_message{};
  rcutils_allocator_t default_allocator = rcutils_get_default_allocator();
  rmw_serialized_message_t serialized_message = rmw_get_zero_initialized_serialized_message();
  ASSERT_EQ(
    RMW_RET_OK, rmw_serialized_message_init(
      &serialized_message, 0lu, &default_allocator)) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_serialized_message_fini(
        &serialized_message)) << rmw_get_error_string().str;
  });

  // Make input_message not equal to output_message.
  input_message.bool_values.push_back(true);
  input_message.int16_values.push_back(-7);

  rmw_ret_t ret = rmw_serialize(&input_message, ts, &serialized_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_NE(nullptr, serialized_message.buffer);
  EXPECT_GT(serialized_message.buffer_length, 0lu);

  // Adding more items should increase buffer length
  auto first_message_length = serialized_message.buffer_length;
  input_message.int32_values.push_back(-1);
  input_message.int32_values.push_back(583);
  input_message.uint16_values.push_back(125);

  ret = rmw_serialize(&input_message, ts, &serialized_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_NE(nullptr, serialized_message.buffer);
  EXPECT_GT(serialized_message.buffer_length, 0lu);
  EXPECT_GT(serialized_message.buffer_length, first_message_length);

  ret = rmw_deserialize(&serialized_message, ts, &output_message);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  EXPECT_EQ(input_message, output_message);
}

TEST(TestSerializeDeserialize, bad_cdr_sequence_correctly_fails_for_cpp) {
  {
    const char * serialization_format = rmw_get_serialization_format();
    if (0 != strcmp(serialization_format, "cdr")) {
      GTEST_SKIP();
    }
  }

  using TestMessage = test_msgs::msg::UnboundedSequences;
  const rosidl_message_type_support_t * ts =
    rosidl_typesupport_cpp::get_message_type_support_handle<TestMessage>();
  TestMessage output_message{};

  check_bad_cdr_sequence_cases(ts, &output_message);
}

TEST(TestSerializeDeserialize, rmw_get_serialized_message_size)
{
  if (rmw_get_serialized_message_size(nullptr, nullptr, nullptr) != RMW_RET_UNSUPPORTED) {
    // TODO(anyone): Add tests here when the implementation it's supported
    GTEST_SKIP();
  }
}
