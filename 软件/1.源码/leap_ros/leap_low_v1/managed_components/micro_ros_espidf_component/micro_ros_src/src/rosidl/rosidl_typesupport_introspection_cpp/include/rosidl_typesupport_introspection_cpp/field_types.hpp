// Copyright 2014 Open Source Robotics Foundation, Inc.
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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_CPP__FIELD_TYPES_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_CPP__FIELD_TYPES_HPP_

#include <rosidl_typesupport_introspection_c/field_types.h>
#include <cstdint>

namespace rosidl_typesupport_introspection_cpp
{
/// Possible types for message fields on a ROS message
/// The equivalent OMG IDL and C++ types of the different fields can be found
/// at http://design.ros2.org/articles/idl_interface_definition.html#type-mapping

/// Equivalent to float in C++ types.
const uint8_t ROS_TYPE_FLOAT = rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT;
/// Equivalent to double in C++ types.
const uint8_t ROS_TYPE_DOUBLE = rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE;
/// Equivalent to long double in C++ types.
const uint8_t ROS_TYPE_LONG_DOUBLE = rosidl_typesupport_introspection_c__ROS_TYPE_LONG_DOUBLE;
/// Equivalent to unsigned char in C++ types.
const uint8_t ROS_TYPE_CHAR = rosidl_typesupport_introspection_c__ROS_TYPE_CHAR;
/// Equivalent to char16_t in C++ types.
const uint8_t ROS_TYPE_WCHAR = rosidl_typesupport_introspection_c__ROS_TYPE_WCHAR;
/// Equivalent to bool in C++ types.
const uint8_t ROS_TYPE_BOOLEAN = rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN;
/// Equivalent to std::byte in C++ types.
const uint8_t ROS_TYPE_OCTET = rosidl_typesupport_introspection_c__ROS_TYPE_OCTET;
/// Equivalent to uint8_t in C++ types.
const uint8_t ROS_TYPE_UINT8 = rosidl_typesupport_introspection_c__ROS_TYPE_UINT8;
/// Equivalent to int8_t in C++ types.
const uint8_t ROS_TYPE_INT8 = rosidl_typesupport_introspection_c__ROS_TYPE_INT8;
/// Equivalent to uint16_t in C++ types.
const uint8_t ROS_TYPE_UINT16 = rosidl_typesupport_introspection_c__ROS_TYPE_UINT16;
/// Equivalent to int16_t in C++ types.
const uint8_t ROS_TYPE_INT16 = rosidl_typesupport_introspection_c__ROS_TYPE_INT16;
/// Equivalent to uint32_t in C++ types.
const uint8_t ROS_TYPE_UINT32 = rosidl_typesupport_introspection_c__ROS_TYPE_UINT32;
/// Equivalent to int32_t in C++ types.
const uint8_t ROS_TYPE_INT32 = rosidl_typesupport_introspection_c__ROS_TYPE_INT32;
/// Equivalent to uint64_t in C++ types.
const uint8_t ROS_TYPE_UINT64 = rosidl_typesupport_introspection_c__ROS_TYPE_UINT64;
/// Equivalent to int64_t in C++ types.
const uint8_t ROS_TYPE_INT64 = rosidl_typesupport_introspection_c__ROS_TYPE_INT64;
/// Equivalent to std::string in C++ types.
const uint8_t ROS_TYPE_STRING = rosidl_typesupport_introspection_c__ROS_TYPE_STRING;
/// Equivalent to std::u16string in C++ types.
const uint8_t ROS_TYPE_WSTRING = rosidl_typesupport_introspection_c__ROS_TYPE_WSTRING;

/// An embedded message type.
const uint8_t ROS_TYPE_MESSAGE = rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE;

/// For backward compatibility only.
const uint8_t ROS_TYPE_BOOL = rosidl_typesupport_introspection_c__ROS_TYPE_BOOL;
/// For backward compatibility only.
const uint8_t ROS_TYPE_BYTE = rosidl_typesupport_introspection_c__ROS_TYPE_BYTE;
/// For backward compatibility only.
const uint8_t ROS_TYPE_FLOAT32 = rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT32;
/// For backward compatibility only.
const uint8_t ROS_TYPE_FLOAT64 = rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT64;

}  // namespace rosidl_typesupport_introspection_cpp

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_CPP__FIELD_TYPES_HPP_
