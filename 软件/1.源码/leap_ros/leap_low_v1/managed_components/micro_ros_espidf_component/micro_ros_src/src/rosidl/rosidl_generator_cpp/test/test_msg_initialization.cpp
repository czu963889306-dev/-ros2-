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

#include <cstring>

#include <string>

#include "rosidl_generator_cpp/msg/defaults.hpp"
#include "rosidl_generator_cpp/msg/bounded_sequences.hpp"

template<typename Callable>
struct ScopeExit
{
  explicit ScopeExit(Callable callable)
  : callable_(callable) {}
  ~ScopeExit() {callable_();}

private:
  Callable callable_;
};

template<typename Callable>
ScopeExit<Callable>
make_scope_exit(Callable callable)
{
  return ScopeExit<Callable>(callable);
}

#define DO_STRING_JOIN(arg1, arg2) arg1 ## arg2
#define STRING_JOIN(arg1, arg2) DO_STRING_JOIN(arg1, arg2)
#define SCOPE_EXIT(code) \
  auto STRING_JOIN(scope_exit_, __LINE__) = make_scope_exit([&]() {code;})

TEST(Test_msg_initialization, no_arg_constructor) {
  rosidl_generator_cpp::msg::Defaults def;
  ASSERT_TRUE(def.bool_value);
  ASSERT_EQ(50, def.byte_value);
  ASSERT_EQ(100, def.char_value);
  ASSERT_EQ(1.125f, def.float32_value);
  ASSERT_EQ(1.125, def.float64_value);
  ASSERT_EQ(-50, def.int8_value);
  ASSERT_EQ(200, def.uint8_value);
  ASSERT_EQ(-1000, def.int16_value);
  ASSERT_EQ(2000, def.uint16_value);
  ASSERT_EQ(-30000L, def.int32_value);
  ASSERT_EQ(60000UL, def.uint32_value);
  ASSERT_EQ(-40000000LL, def.int64_value);
  ASSERT_EQ(50000000ULL, def.uint64_value);

  rosidl_generator_cpp::msg::BasicTypes basic;
  ASSERT_FALSE(basic.bool_value);
  ASSERT_EQ(0, basic.byte_value);
  ASSERT_EQ(0, basic.char_value);
  ASSERT_EQ(0.0f, basic.float32_value);
  ASSERT_EQ(0.0, basic.float64_value);
  ASSERT_EQ(0, basic.int8_value);
  ASSERT_EQ(0, basic.uint8_value);
  ASSERT_EQ(0, basic.int16_value);
  ASSERT_EQ(0, basic.uint16_value);
  ASSERT_EQ(0L, basic.int32_value);
  ASSERT_EQ(0UL, basic.uint32_value);
  ASSERT_EQ(0LL, basic.int64_value);
}

TEST(Test_msg_initialization, all_constructor) {
  rosidl_generator_cpp::msg::Defaults def(
    rosidl_runtime_cpp::MessageInitialization::ALL);
  ASSERT_TRUE(def.bool_value);
  ASSERT_EQ(50, def.byte_value);
  ASSERT_EQ(100, def.char_value);
  ASSERT_EQ(1.125f, def.float32_value);
  ASSERT_EQ(1.125, def.float64_value);
  ASSERT_EQ(-50, def.int8_value);
  ASSERT_EQ(200, def.uint8_value);
  ASSERT_EQ(-1000, def.int16_value);
  ASSERT_EQ(2000, def.uint16_value);
  ASSERT_EQ(-30000L, def.int32_value);
  ASSERT_EQ(60000UL, def.uint32_value);
  ASSERT_EQ(-40000000LL, def.int64_value);
  ASSERT_EQ(50000000ULL, def.uint64_value);

  rosidl_generator_cpp::msg::BasicTypes basic;
  ASSERT_FALSE(basic.bool_value);
  ASSERT_EQ(0, basic.byte_value);
  ASSERT_EQ(0, basic.char_value);
  ASSERT_EQ(0.0f, basic.float32_value);
  ASSERT_EQ(0.0, basic.float64_value);
  ASSERT_EQ(0, basic.int8_value);
  ASSERT_EQ(0, basic.uint8_value);
  ASSERT_EQ(0, basic.int16_value);
  ASSERT_EQ(0, basic.uint16_value);
  ASSERT_EQ(0L, basic.int32_value);
  ASSERT_EQ(0UL, basic.uint32_value);
  ASSERT_EQ(0LL, basic.int64_value);
}

TEST(Test_msg_initialization, zero_constructor) {
  rosidl_generator_cpp::msg::Defaults def(
    rosidl_runtime_cpp::MessageInitialization::ZERO);
  ASSERT_FALSE(def.bool_value);
  ASSERT_EQ(0, def.byte_value);
  ASSERT_EQ(0, def.char_value);
  ASSERT_EQ(0.0f, def.float32_value);
  ASSERT_EQ(0.0, def.float64_value);
  ASSERT_EQ(0, def.int8_value);
  ASSERT_EQ(0, def.uint8_value);
  ASSERT_EQ(0, def.int16_value);
  ASSERT_EQ(0, def.uint16_value);
  ASSERT_EQ(0L, def.int32_value);
  ASSERT_EQ(0UL, def.uint32_value);
  ASSERT_EQ(0LL, def.int64_value);
  ASSERT_EQ(0ULL, def.uint64_value);

  rosidl_generator_cpp::msg::BasicTypes basic;
  ASSERT_FALSE(basic.bool_value);
  ASSERT_EQ(0, basic.byte_value);
  ASSERT_EQ(0, basic.char_value);
  ASSERT_EQ(0.0f, basic.float32_value);
  ASSERT_EQ(0.0, basic.float64_value);
  ASSERT_EQ(0, basic.int8_value);
  ASSERT_EQ(0, basic.uint8_value);
  ASSERT_EQ(0, basic.int16_value);
  ASSERT_EQ(0, basic.uint16_value);
  ASSERT_EQ(0L, basic.int32_value);
  ASSERT_EQ(0UL, basic.uint32_value);
  ASSERT_EQ(0LL, basic.int64_value);
}

TEST(Test_msg_initialization, defaults_only_constructor) {
  rosidl_generator_cpp::msg::Defaults def(
    rosidl_runtime_cpp::MessageInitialization::DEFAULTS_ONLY);
  ASSERT_TRUE(def.bool_value);
  ASSERT_EQ(50, def.byte_value);
  ASSERT_EQ(100, def.char_value);
  ASSERT_EQ(1.125f, def.float32_value);
  ASSERT_EQ(1.125, def.float64_value);
  ASSERT_EQ(-50, def.int8_value);
  ASSERT_EQ(200, def.uint8_value);
  ASSERT_EQ(-1000, def.int16_value);
  ASSERT_EQ(2000, def.uint16_value);
  ASSERT_EQ(-30000L, def.int32_value);
  ASSERT_EQ(60000UL, def.uint32_value);
  ASSERT_EQ(-40000000LL, def.int64_value);
  ASSERT_EQ(50000000ULL, def.uint64_value);

  rosidl_generator_cpp::msg::BasicTypes basic;
  ASSERT_FALSE(basic.bool_value);
  ASSERT_EQ(0, basic.byte_value);
  ASSERT_EQ(0, basic.char_value);
  ASSERT_EQ(0.0f, basic.float32_value);
  ASSERT_EQ(0.0, basic.float64_value);
  ASSERT_EQ(0, basic.int8_value);
  ASSERT_EQ(0, basic.uint8_value);
  ASSERT_EQ(0, basic.int16_value);
  ASSERT_EQ(0, basic.uint16_value);
  ASSERT_EQ(0L, basic.int32_value);
  ASSERT_EQ(0UL, basic.uint32_value);
  ASSERT_EQ(0LL, basic.int64_value);
}

// This is a test to ensure that when the user passes SKIP to the constructor,
// it does no initialization.
TEST(Test_msg_initialization, skip_constructor) {
  char * memory = new char[sizeof(rosidl_generator_cpp::msg::BoundedSequences)];

  std::memset(memory, 0xfe, sizeof(rosidl_generator_cpp::msg::BoundedSequences));
  rosidl_generator_cpp::msg::BoundedSequences * bounded =
    new(memory) rosidl_generator_cpp::msg::BoundedSequences(
    rosidl_runtime_cpp::MessageInitialization::SKIP);

  // ensures that the memory gets freed even if an ASSERT is raised
  SCOPE_EXIT(bounded->~BoundedSequences_(); delete[] memory);

  #ifndef _WIN32
  #  pragma GCC diagnostic push
  #  pragma GCC diagnostic ignored "-Wstrict-aliasing"
  #endif
  ASSERT_TRUE(
    std::all_of(
      bounded->float32_values_default.begin(),
      bounded->float32_values_default.end(), [](float i) {
        uint32_t float32_bit_pattern = *reinterpret_cast<uint32_t *>(&i);
        return 0xfefefefe == float32_bit_pattern;
      }));
  ASSERT_TRUE(
    std::all_of(
      bounded->float64_values_default.begin(),
      bounded->float64_values_default.end(), [](double i) {
        uint64_t float64_bit_pattern = *reinterpret_cast<uint64_t *>(&i);
        return 0xfefefefefefefefe == float64_bit_pattern;
      }));

  #ifndef _WIN32
  #  pragma GCC diagnostic pop
  #endif
  ASSERT_EQ(0UL, bounded->float64_values_default.size());
  ASSERT_EQ(0UL, bounded->float32_values_default.size());
  ASSERT_EQ(0UL, bounded->float32_values.size());
  ASSERT_TRUE(
    std::all_of(
      bounded->string_values_default.begin(),
      bounded->string_values_default.end(), [](std::string i) {
        return "" == i;
      }));
}
