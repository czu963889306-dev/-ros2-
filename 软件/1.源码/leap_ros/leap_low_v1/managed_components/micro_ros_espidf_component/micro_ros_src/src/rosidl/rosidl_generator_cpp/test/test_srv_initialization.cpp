// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#include "rosidl_generator_cpp/srv/basic_types.hpp"
#include "rosidl_generator_cpp/srv/empty.hpp"

TEST(Test_srv_initialization, no_arg_request_constructor) {
  rosidl_generator_cpp::srv::Empty::Response empty;

  rosidl_generator_cpp::srv::BasicTypes::Request basic_types;
  EXPECT_EQ(false, basic_types.bool_value);
  EXPECT_EQ(0, basic_types.byte_value);
  EXPECT_EQ(0, basic_types.char_value);
  EXPECT_EQ(0.0f, basic_types.float32_value);
  EXPECT_EQ(0.0, basic_types.float64_value);
  EXPECT_EQ(0, basic_types.int8_value);
  EXPECT_EQ(0u, basic_types.uint8_value);
  EXPECT_EQ(0, basic_types.int16_value);
  EXPECT_EQ(0u, basic_types.uint16_value);
  EXPECT_EQ(0, basic_types.int32_value);
  EXPECT_EQ(0u, basic_types.uint32_value);
  EXPECT_EQ(0, basic_types.int64_value);
  EXPECT_EQ(0u, basic_types.uint64_value);
  EXPECT_EQ("", basic_types.string_value);
}

TEST(Test_srv_initialization, no_arg_response_constructor) {
  rosidl_generator_cpp::srv::Empty::Request empty;

  rosidl_generator_cpp::srv::BasicTypes::Response basic_types;
  EXPECT_EQ(false, basic_types.bool_value);
  EXPECT_EQ(0, basic_types.byte_value);
  EXPECT_EQ(0, basic_types.char_value);
  EXPECT_EQ(0.0f, basic_types.float32_value);
  EXPECT_EQ(0.0, basic_types.float64_value);
  EXPECT_EQ(0, basic_types.int8_value);
  EXPECT_EQ(0u, basic_types.uint8_value);
  EXPECT_EQ(0, basic_types.int16_value);
  EXPECT_EQ(0u, basic_types.uint16_value);
  EXPECT_EQ(0, basic_types.int32_value);
  EXPECT_EQ(0u, basic_types.uint32_value);
  EXPECT_EQ(0, basic_types.int64_value);
  EXPECT_EQ(0u, basic_types.uint64_value);
  EXPECT_EQ("", basic_types.string_value);
}

// Note that we very specifically don't add tests for some of the other
// possibilities here (bounded strings, bounded / unbounded arrays, other
// initialization parameters, etc).  That's because that is all tested in
// test_msg_initialization.cpp, so we don't need to repeat it here.
