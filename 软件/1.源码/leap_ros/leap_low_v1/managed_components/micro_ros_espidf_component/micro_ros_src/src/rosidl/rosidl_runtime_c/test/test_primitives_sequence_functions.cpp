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
#include "rosidl_runtime_c/primitives_sequence.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

#define TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(STRUCT_NAME, TYPE_NAME) \
  TEST(primitives_sequence_functions, test_ ## STRUCT_NAME ## _init_fini) \
  { \
    EXPECT_FALSE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__init(nullptr, 0u)); \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence sequence; \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__init(&sequence, 0u)); \
    EXPECT_EQ(sequence.data, nullptr); \
    EXPECT_EQ(sequence.size, 0u); \
    EXPECT_EQ(sequence.capacity, 0u); \
 \
    /* Shouldn't do anything exciting */ \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__fini(&sequence); \
 \
    constexpr size_t seq_size = 3u; \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__init(&sequence, seq_size)); \
    EXPECT_EQ(sequence.size, seq_size); \
    EXPECT_EQ(sequence.capacity, seq_size); \
    EXPECT_NE(sequence.data, nullptr); \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__fini(&sequence); \
  } \
 \
  TEST(primitives_sequence_functions, test_ ## STRUCT_NAME ## _equality_comparison) \
  { \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence empty; \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__init(&empty, 0u)); \
    EXPECT_FALSE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal(nullptr, nullptr)); \
    EXPECT_FALSE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal(&empty, nullptr)); \
    EXPECT_FALSE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal(nullptr, &empty)); \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal(&empty, &empty)); \
 \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence zero_initialized; \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__init(&zero_initialized, 1u)); \
    zero_initialized.data[0] = (TYPE_NAME)0; \
    EXPECT_FALSE( \
      rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal( \
        &zero_initialized, \
        &empty)); \
    EXPECT_FALSE( \
      rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal( \
        &empty, \
        &zero_initialized)); \
    EXPECT_TRUE( \
      rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal( \
        &zero_initialized, \
        &zero_initialized)); \
 \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence nonzero_initialized; \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__init(&nonzero_initialized, 1u)); \
    nonzero_initialized.data[0] = (TYPE_NAME)1; \
    EXPECT_FALSE( \
      rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal( \
        &nonzero_initialized, \
        &zero_initialized)); \
    EXPECT_FALSE( \
      rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal( \
        &zero_initialized, \
        &nonzero_initialized)); \
    EXPECT_TRUE( \
      rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal( \
        &nonzero_initialized, \
        &nonzero_initialized)); \
 \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__fini(&nonzero_initialized); \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__fini(&zero_initialized); \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__fini(&empty); \
  } \
 \
  TEST(primitives_sequence_functions, test_ ## STRUCT_NAME ## _copy) \
  { \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence input, output; \
    EXPECT_FALSE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__copy(nullptr, nullptr)); \
    EXPECT_FALSE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__copy(nullptr, &output)); \
    EXPECT_FALSE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__copy(&input, nullptr)); \
 \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__init(&input, 1u)); \
    input.data[0] = (TYPE_NAME)1; \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__init(&output, 0u)); \
    EXPECT_FALSE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal(&input, &output)); \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__copy(&input, &output)); \
    EXPECT_TRUE(rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__are_equal(&input, &output)); \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__fini(&output); \
    rosidl_runtime_c__ ## STRUCT_NAME ## __Sequence__fini(&input); \
  }

TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(float, float)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(double, double)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(long_double, long double)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(char, signed char)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(wchar, uint16_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(boolean, bool)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(octet, uint8_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(uint8, uint8_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(int8, int8_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(uint16, uint16_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(int16, int16_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(uint32, uint32_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(int32, int32_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(uint64, uint64_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(int64, int64_t)

// Testing legacy API
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(bool, bool)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(byte, uint8_t)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(float32, float)
TEST_PRIMITIVE_SEQUENCE_FUNCTIONS(float64, double)
