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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_CPP__SERVICE_INTROSPECTION_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_CPP__SERVICE_INTROSPECTION_HPP_

#include <cstddef>
#include <cstdint>

#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_runtime_c/visibility_control.h"

#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"

namespace rosidl_typesupport_introspection_cpp
{

/// This struct provides introspection information for one service definition.
/// A service is comprised of two interfaces: the request and the response.
typedef struct ServiceMembers_s
{
  /// The namespace in which the service resides, e.g. "example_messages::srv"
  /// for example_messages/srv
  const char * service_namespace_;
  /// The name of the service, e.g. "AddTwoInts"
  const char * service_name_;
  /// A pointer to the introspection information structure for the request interface.
  const MessageMembers * request_members_;
  /// A pointer to the introspection information structure for the response interface.
  const MessageMembers * response_members_;
} ServiceMembers;

}  // namespace rosidl_typesupport_introspection_cpp

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_CPP__SERVICE_INTROSPECTION_HPP_
