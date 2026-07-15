// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// Copyright 2019 Open Source Robotics Foundation, Inc.
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
#include <string>

// the idl file is commented out in the test_interface_files package
// #include "rosidl_generator_cpp/idl/idl_only_types.hpp"
#include "rosidl_generator_cpp/msg/defaults.hpp"
#include "rosidl_generator_cpp/msg/empty.hpp"
#include "rosidl_generator_cpp/msg/bounded_sequences.hpp"
#include "rosidl_generator_cpp/msg/nested.hpp"
#include "rosidl_generator_cpp/msg/strings.hpp"
#include "rosidl_generator_cpp/msg/w_strings.hpp"
#include "rosidl_generator_cpp/srv/empty.hpp"

using rosidl_generator_traits::is_message;
using rosidl_generator_traits::is_service;
using rosidl_generator_traits::is_service_request;
using rosidl_generator_traits::is_service_response;
using rosidl_generator_cpp::msg::to_yaml;

TEST(Test_rosidl_generator_traits, to_yaml_default_style) {
  {
    const rosidl_generator_cpp::msg::Empty msg;
    EXPECT_STREQ("null\n", to_yaml(msg).c_str());
  }

  {
    rosidl_generator_cpp::msg::Defaults msg;
    msg.float64_value = 1.0;
    EXPECT_STREQ(
      R"(bool_value: true
byte_value: 0x32
char_value: 100
float32_value: 1.12500
float64_value: 1.00000
int8_value: -50
uint8_value: 200
int16_value: -1000
uint16_value: 2000
int32_value: -30000
uint32_value: 60000
int64_value: -40000000
uint64_value: 50000000
)",
      to_yaml(
        msg).c_str());
  }

  {
    rosidl_generator_cpp::msg::Strings msg;
    msg.string_value = "Hello\nworld";
    EXPECT_STREQ(
      R"(string_value: "Hello
world"
string_value_default1: "Hello world!"
string_value_default2: "Hello'world!"
string_value_default3: "Hello\"world!"
string_value_default4: "Hello'world!"
string_value_default5: "Hello\"world!"
bounded_string_value: ""
bounded_string_value_default1: "Hello world!"
bounded_string_value_default2: "Hello'world!"
bounded_string_value_default3: "Hello\"world!"
bounded_string_value_default4: "Hello'world!"
bounded_string_value_default5: "Hello\"world!"
)",
      to_yaml(
        msg).c_str());
  }

  {
    rosidl_generator_cpp::msg::WStrings msg;
    msg.wstring_value = u"Hello\nwörld";
    EXPECT_STREQ(
      R"(wstring_value: "Hello
w\xf6rld"
wstring_value_default1: "Hello world!"
wstring_value_default2: "Hell\xf6 w\xf6rld!"
wstring_value_default3: "\u30cf\u30ed\u30fc\u30ef\u30fc\u30eb\u30c9"
array_of_wstrings:
- ""
- ""
- ""
bounded_sequence_of_wstrings: []
unbounded_sequence_of_wstrings: []
)",
      to_yaml(
        msg).c_str());
  }

  /*{
    test_msgs::idl::IdlOnlyTypes msg;
    msg.wchar_value = u'ö';
    msg.long_double_value = 1.125;
    EXPECT_STREQ(
      R"(wchar_value: "\u00f6"
long_double_value: 1.12500
)",
      to_yaml(
        msg).c_str());

    msg.wchar_value = u'貓';
    EXPECT_STREQ(
      R"(wchar_value: "\u8c93"
long_double_value: 1.12500
)",
      to_yaml(
        msg).c_str());
  }*/

  {
    rosidl_generator_cpp::msg::Nested msg;
    std::string yaml = to_yaml(msg);
#ifdef _WIN32
    // update yaml to handle variance of floating point decimals on Windows
    size_t index = 0;
    while ((index = yaml.find("0.000000", index)) != std::string::npos) {
      yaml = yaml.replace(index, 8, "0.00000");
    }
#endif
    EXPECT_STREQ(
      R"(basic_types_value:
  bool_value: false
  byte_value: 0x00
  char_value: 0
  float32_value: 0.00000
  float64_value: 0.00000
  int8_value: 0
  uint8_value: 0
  int16_value: 0
  uint16_value: 0
  int32_value: 0
  uint32_value: 0
  int64_value: 0
  uint64_value: 0
)",
      yaml.c_str());
  }

  {
    rosidl_generator_cpp::msg::BoundedSequences msg;
    msg.defaults_values.push_back(rosidl_generator_cpp::msg::Defaults());
    std::string yaml = to_yaml(msg);
#ifdef _WIN32
    // update yaml to handle variance of floating point decimals on Windows
    size_t index = 0;
    while ((index = yaml.find("0.000000", index)) != std::string::npos) {
      yaml = yaml.replace(index, 8, "0.00000");
    }
#endif
    EXPECT_STREQ(
      R"(bool_values: []
byte_values: []
char_values: []
float32_values: []
float64_values: []
int8_values: []
uint8_values: []
int16_values: []
uint16_values: []
int32_values: []
uint32_values: []
int64_values: []
uint64_values: []
string_values: []
basic_types_values: []
constants_values: []
defaults_values:
-
  bool_value: true
  byte_value: 0x32
  char_value: 100
  float32_value: 1.12500
  float64_value: 1.12500
  int8_value: -50
  uint8_value: 200
  int16_value: -1000
  uint16_value: 2000
  int32_value: -30000
  uint32_value: 60000
  int64_value: -40000000
  uint64_value: 50000000
bool_values_default:
- false
- true
- false
byte_values_default:
- 0x00
- 0x01
- 0xff
char_values_default:
- 0
- 1
- 127
float32_values_default:
- 1.12500
- 0.00000
- -1.12500
float64_values_default:
- 3.14150
- 0.00000
- -3.14150
int8_values_default:
- 0
- 127
- -128
uint8_values_default:
- 0
- 1
- 255
int16_values_default:
- 0
- 32767
- -32768
uint16_values_default:
- 0
- 1
- 65535
int32_values_default:
- 0
- 2147483647
- -2147483648
uint32_values_default:
- 0
- 1
- 4294967295
int64_values_default:
- 0
- 9223372036854775807
- -9223372036854775808
uint64_values_default:
- 0
- 1
- 18446744073709551615
string_values_default:
- ""
- "max value"
- "min value"
alignment_check: 0
)",
      yaml.c_str());
  }
}

TEST(Test_rosidl_generator_traits, to_yaml_flow_style) {
  constexpr bool use_flow_style = true;
  {
    const rosidl_generator_cpp::msg::Empty msg;
    EXPECT_STREQ("null", to_yaml(msg, use_flow_style).c_str());
  }

  {
    rosidl_generator_cpp::msg::Defaults msg;
    msg.float64_value = 1.0;
    EXPECT_STREQ(
      "{bool_value: true, byte_value: 0x32, char_value: 100, "
      "float32_value: 1.12500, float64_value: 1.00000, int8_value: -50, "
      "uint8_value: 200, int16_value: -1000, uint16_value: 2000, "
      "int32_value: -30000, uint32_value: 60000, int64_value: -40000000, "
      "uint64_value: 50000000}",
      to_yaml(msg, use_flow_style).c_str());
  }

  {
    rosidl_generator_cpp::msg::Strings msg;
    msg.string_value = "Hello\nworld";
    EXPECT_STREQ(
      R"({string_value: "Hello
world", string_value_default1: "Hello world!", )"
      R"(string_value_default2: "Hello'world!", )"
      R"(string_value_default3: "Hello\"world!", )"
      R"(string_value_default4: "Hello'world!", )"
      R"(string_value_default5: "Hello\"world!", )"
      R"(bounded_string_value: "", )"
      R"(bounded_string_value_default1: "Hello world!", )"
      R"(bounded_string_value_default2: "Hello'world!", )"
      R"(bounded_string_value_default3: "Hello\"world!", )"
      R"(bounded_string_value_default4: "Hello'world!", )"
      R"(bounded_string_value_default5: "Hello\"world!"})",
      to_yaml(msg, use_flow_style).c_str());
  }

  {
    rosidl_generator_cpp::msg::WStrings msg;
    msg.wstring_value = u"Hello\nwörld";
    EXPECT_STREQ(
      R"({wstring_value: "Hello
w\xf6rld", wstring_value_default1: "Hello world!", )"
      R"(wstring_value_default2: "Hell\xf6 w\xf6rld!", )"
      R"(wstring_value_default3: "\u30cf\u30ed\u30fc\u30ef\u30fc\u30eb\u30c9", )"
      R"(array_of_wstrings: ["", "", ""], bounded_sequence_of_wstrings: [], )"
      R"(unbounded_sequence_of_wstrings: []})",
      to_yaml(msg, use_flow_style).c_str());
  }

  {
    rosidl_generator_cpp::msg::Nested msg;
    std::string yaml = to_yaml(msg, use_flow_style);
#ifdef _WIN32
    // update yaml to handle variance of floating point decimals on Windows
    size_t index = 0;
    while ((index = yaml.find("0.000000", index)) != std::string::npos) {
      yaml = yaml.replace(index, 8, "0.00000");
    }
#endif
    EXPECT_STREQ(
      R"({basic_types_value: {bool_value: false, byte_value: 0x00, )"
      R"(char_value: 0, float32_value: 0.00000, float64_value: 0.00000, )"
      R"(int8_value: 0, uint8_value: 0, int16_value: 0, uint16_value: 0, )"
      R"(int32_value: 0, uint32_value: 0, int64_value: 0, uint64_value: 0}})",
      yaml.c_str());
  }

  {
    rosidl_generator_cpp::msg::BoundedSequences msg;
    msg.defaults_values.push_back(rosidl_generator_cpp::msg::Defaults());
    std::string yaml = to_yaml(msg, use_flow_style);
#ifdef _WIN32
    // update yaml to handle variance of floating point decimals on Windows
    size_t index = 0;
    while ((index = yaml.find("0.000000", index)) != std::string::npos) {
      yaml = yaml.replace(index, 8, "0.00000");
    }
#endif
    EXPECT_STREQ(
      R"({bool_values: [], byte_values: [], char_values: [], )"
      R"(float32_values: [], float64_values: [], int8_values: [], )"
      R"(uint8_values: [], int16_values: [], uint16_values: [], )"
      R"(int32_values: [], uint32_values: [], int64_values: [], )"
      R"(uint64_values: [], string_values: [], basic_types_values: [], )"
      R"(constants_values: [], defaults_values: [{bool_value: true, )"
      R"(byte_value: 0x32, char_value: 100, float32_value: 1.12500, )"
      R"(float64_value: 1.12500, int8_value: -50, uint8_value: 200, )"
      R"(int16_value: -1000, uint16_value: 2000, int32_value: -30000, )"
      R"(uint32_value: 60000, int64_value: -40000000, uint64_value: 50000000}],)"
      R"( bool_values_default: [false, true, false], )"
      R"(byte_values_default: [0x00, 0x01, 0xff], )"
      R"(char_values_default: [0, 1, 127], float32_values_default: [1.12500, )"
      R"(0.00000, -1.12500], float64_values_default: [3.14150, 0.00000, )"
      R"(-3.14150], int8_values_default: [0, 127, -128], )"
      R"(uint8_values_default: [0, 1, 255], int16_values_default: )"
      R"([0, 32767, -32768], uint16_values_default: [0, 1, 65535], )"
      R"(int32_values_default: [0, 2147483647, -2147483648], )"
      R"(uint32_values_default: [0, 1, 4294967295], )"
      R"(int64_values_default: [0, 9223372036854775807, -9223372036854775808],)"
      R"( uint64_values_default: [0, 1, 18446744073709551615], )"
      R"(string_values_default: ["", "max value", "min value"], )"
      R"(alignment_check: 0})",
      yaml.c_str());
  }
}


// Empty testing struct
struct Message {};

// Empty testing struct, with template instantiation
struct Message2 {};

namespace rosidl_generator_traits
{

template<>
struct is_message<Message2>: std::true_type {};

}  // namespace rosidl_generator_traits

TEST(Test_rosidl_generator_traits, is_message) {
  // A message is not a service
  using Empty = rosidl_generator_cpp::msg::Empty;
  EXPECT_TRUE(is_message<Empty>());
  EXPECT_FALSE(is_service<Empty>());
  EXPECT_FALSE(is_service_request<Empty>());
  EXPECT_FALSE(is_service_response<Empty>());

  // A message is not a service
  using Strings = rosidl_generator_cpp::msg::Strings;
  EXPECT_TRUE(is_message<Strings>());
  EXPECT_FALSE(is_service<Strings>());
  EXPECT_FALSE(is_service_request<Strings>());
  EXPECT_FALSE(is_service_response<Strings>());

  // Other datatypes should have is_message == false
  EXPECT_FALSE(is_message<double>());
  EXPECT_FALSE(is_message<Message>());

  // Unless the template has been specifically instantiated for the type
  EXPECT_TRUE(is_message<Message2>());
}

TEST(Test_rosidl_generator_traits, is_service) {
  using Service = rosidl_generator_cpp::srv::Empty;
  using ServiceReq = Service::Request;
  using ServiceResp = Service::Response;

  EXPECT_TRUE(is_service<Service>());
  EXPECT_FALSE(is_message<Service>());
  EXPECT_FALSE(is_service_request<Service>());
  EXPECT_FALSE(is_service_response<Service>());

  // Requests are additionally messages
  EXPECT_FALSE(is_service<ServiceReq>());
  EXPECT_TRUE(is_message<ServiceReq>());
  EXPECT_TRUE(is_service_request<ServiceReq>());
  EXPECT_FALSE(is_service_response<ServiceReq>());

  // Responses are additionally messages
  EXPECT_FALSE(is_service<ServiceResp>());
  EXPECT_TRUE(is_message<ServiceResp>());
  EXPECT_FALSE(is_service_request<ServiceResp>());
  EXPECT_TRUE(is_service_response<ServiceResp>());
}
