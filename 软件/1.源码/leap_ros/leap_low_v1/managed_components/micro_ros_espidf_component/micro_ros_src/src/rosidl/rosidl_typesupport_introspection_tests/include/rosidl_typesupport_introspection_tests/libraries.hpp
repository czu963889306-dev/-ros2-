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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__LIBRARIES_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__LIBRARIES_HPP_

#include <rcutils/macros.h>

namespace rosidl_typesupport_introspection_tests
{

/// A literal type to hold message symbols.
struct MessageTypeSupportSymbolRecord
{
  const char * symbol;
};

/// A literal type to hold service symbols.
struct ServiceTypeSupportSymbolRecord
{
  const char * symbol;
  const MessageTypeSupportSymbolRecord request;
  const MessageTypeSupportSymbolRecord response;
};

/// A literal type to hold action symbols.
struct ActionTypeSupportSymbolRecord
{
  const char * symbol;
  const MessageTypeSupportSymbolRecord feedback;
  const MessageTypeSupportSymbolRecord feedback_message;
  const MessageTypeSupportSymbolRecord result;
  const MessageTypeSupportSymbolRecord goal;
  const ServiceTypeSupportSymbolRecord send_goal;
  const ServiceTypeSupportSymbolRecord get_result;
};

/// Makes a MessageTypeSupportSymbolRecord literal for a message.
#define MESSAGE_TYPESUPPORT_SYMBOL_RECORD( \
    typesupport_name, package_name, interface_type, message_name) \
  {RCUTILS_STRINGIFY( \
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME( \
        typesupport_name, package_name, interface_type, message_name))}

/// Makes a MessageTypeSupportSymbolRecord literal for a service.
#define SERVICE_TYPESUPPORT_SYMBOL_RECORD( \
    typesupport_name, package_name, interface_type, service_name) \
  {RCUTILS_STRINGIFY( \
      ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME( \
        typesupport_name, package_name, interface_type, service_name)), \
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD( \
      typesupport_name, package_name, \
      interface_type, RCUTILS_JOIN(service_name, _Request)), \
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD( \
      typesupport_name, package_name, \
      interface_type, RCUTILS_JOIN(service_name, _Response))}

/// Makes a MessageTypeSupportSymbolRecord literal for an action.
#define ACTION_TYPESUPPORT_SYMBOL_RECORD( \
    typesupport_name, package_name, interface_type, action_name) \
  {RCUTILS_STRINGIFY( \
      ROSIDL_TYPESUPPORT_INTERFACE__ACTION_SYMBOL_NAME( \
        typesupport_name, package_name, interface_type, action_name)), \
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD( \
      typesupport_name, package_name, \
      interface_type, RCUTILS_JOIN(action_name, _Feedback)), \
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD( \
      typesupport_name, package_name, \
      interface_type, RCUTILS_JOIN(action_name, _FeedbackMessage)), \
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD( \
      typesupport_name, package_name, \
      interface_type, RCUTILS_JOIN(action_name, _Result)), \
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD( \
      typesupport_name, package_name, \
      interface_type, RCUTILS_JOIN(action_name, _Goal)), \
    SERVICE_TYPESUPPORT_SYMBOL_RECORD( \
      typesupport_name, package_name, \
      interface_type, RCUTILS_JOIN(action_name, _SendGoal)), \
    SERVICE_TYPESUPPORT_SYMBOL_RECORD( \
      typesupport_name, package_name, \
      interface_type, RCUTILS_JOIN(action_name, _GetResult))}

}  // namespace rosidl_typesupport_introspection_tests

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__LIBRARIES_HPP_
