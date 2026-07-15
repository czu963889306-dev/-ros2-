// Copyright 2022 Open Source Robotics Foundation, Inc.
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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__TYPES_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__TYPES_HPP_

#include <rosidl_runtime_c/message_type_support_struct.h>
#include <rosidl_runtime_c/service_type_support_struct.h>

#include <rosidl_typesupport_introspection_cpp/field_types.hpp>

namespace rosidl_typesupport_introspection_tests
{

// NOTE(hidmic): bring C++ field type IDs assuming these
// match with their C equivalents.
using rosidl_typesupport_introspection_cpp::ROS_TYPE_FLOAT;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_DOUBLE;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_LONG_DOUBLE;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_CHAR;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_WCHAR;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_BOOLEAN;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_OCTET;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT8;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_INT8;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT16;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_INT16;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT32;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_INT32;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_UINT64;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_INT64;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING;
using rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE;

using MessageTypeSupportFetchFunctionT =
  const rosidl_message_type_support_t * (*)();
using ServiceTypeSupportFetchFunctionT =
  const rosidl_service_type_support_t * (*)();

}  // namespace rosidl_typesupport_introspection_tests

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__TYPES_HPP_
