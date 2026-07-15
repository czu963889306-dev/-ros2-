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

#ifndef ROSIDL_RUNTIME_CPP__TRAITS_HPP_
#define ROSIDL_RUNTIME_CPP__TRAITS_HPP_

#include <codecvt>
#include <cstdint>
#include <iomanip>
#include <string>
#include <type_traits>

namespace rosidl_generator_traits
{

inline void value_to_yaml(bool value, std::ostream & out)
{
  out << (value ? "true" : "false");
}

inline void character_value_to_yaml(unsigned char value, std::ostream & out)
{
  auto flags = out.flags();
  out << "0x" << std::hex << std::setw(2) << std::setfill('0') << \
    static_cast<uint16_t>(value);
  out.flags(flags);
}

inline void character_value_to_yaml(char16_t value, std::ostream & out)
{
  auto flags = out.flags();
  out << "\"\\u" << std::hex << std::setw(4) << std::setfill('0') << \
    static_cast<uint_least16_t>(value) << "\"";
  out.flags(flags);
}

inline void value_to_yaml(float value, std::ostream & out)
{
  auto flags = out.flags();
  out << std::showpoint << value;
  out.flags(flags);
}

inline void value_to_yaml(double value, std::ostream & out)
{
  auto flags = out.flags();
  out << std::showpoint << value;
  out.flags(flags);
}

inline void value_to_yaml(long double value, std::ostream & out)
{
  auto flags = out.flags();
  out << std::showpoint << value;
  out.flags(flags);
}

inline void value_to_yaml(uint8_t value, std::ostream & out)
{
  out << +value;
}

inline void value_to_yaml(int8_t value, std::ostream & out)
{
  out << +value;
}

inline void value_to_yaml(uint16_t value, std::ostream & out)
{
  out << value;
}

inline void value_to_yaml(int16_t value, std::ostream & out)
{
  out << value;
}

inline void value_to_yaml(uint32_t value, std::ostream & out)
{
  out << value;
}

inline void value_to_yaml(int32_t value, std::ostream & out)
{
  out << value;
}

inline void value_to_yaml(uint64_t value, std::ostream & out)
{
  out << value;
}

inline void value_to_yaml(int64_t value, std::ostream & out)
{
  out << value;
}

inline void value_to_yaml(const std::string & value, std::ostream & out)
{
  out << "\"";
  size_t index = 0;
  while (index < value.size()) {
    size_t pos = value.find_first_of("\\\"", index);
    if (pos == std::string::npos) {
      pos = value.size();
    }
    out.write(&value[index], pos - index);
    if (pos >= value.size()) {
      break;
    }
    out << "\\" << value[pos];
    index = pos + 1;
  }
  out << "\"";
}

inline void value_to_yaml(const std::u16string & value, std::ostream & out)
{
  out << "\"";
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  auto flags = out.flags();
  size_t index = 0;
  while (index < value.size()) {
    uint_least16_t character = static_cast<uint_least16_t>(value[index]);
    if (!(character & 0xff80)) {  // ASCII
      std::string character_as_string = convert.to_bytes(character);
      out << std::hex << character_as_string.c_str();
    } else if (!(character & 0xff00)) {  // only 1 byte set
      out << "\\x" << std::hex << std::setw(2) << std::setfill('0') << \
        character;
    } else {
      out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << \
        character;
    }
    index += 1;
  }
  out.flags(flags);
  out << "\"";
}

template<typename T>
inline const char * data_type();

template<typename T>
inline const char * name();

template<typename T>
struct has_fixed_size : std::false_type {};

template<typename T>
struct has_bounded_size : std::false_type {};

template<typename T>
struct is_message : std::false_type {};

template<typename T>
struct is_service : std::false_type {};

template<typename T>
struct is_service_request : std::false_type {};

template<typename T>
struct is_service_response : std::false_type {};

template<typename T>
struct is_action : std::false_type {};

template<typename T>
struct is_action_goal : std::false_type {};

template<typename T>
struct is_action_result : std::false_type {};

template<typename T>
struct is_action_feedback : std::false_type {};

}  // namespace rosidl_generator_traits

#endif  // ROSIDL_RUNTIME_CPP__TRAITS_HPP_
