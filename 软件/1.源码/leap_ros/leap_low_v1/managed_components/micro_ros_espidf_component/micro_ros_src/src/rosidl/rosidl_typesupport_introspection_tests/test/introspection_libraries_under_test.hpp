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

#ifndef INTROSPECTION_LIBRARIES_UNDER_TEST_HPP_
#define INTROSPECTION_LIBRARIES_UNDER_TEST_HPP_

#include <rcutils/macros.h>

#include <rosidl_typesupport_interface/macros.h>
#include <rosidl_typesupport_introspection_c/message_introspection.h>
#include <rosidl_typesupport_introspection_c/service_introspection.h>

#include <memory>

#include <rosidl_typesupport_introspection_cpp/message_introspection.hpp>
#include <rosidl_typesupport_introspection_cpp/service_introspection.hpp>

#include "rosidl_typesupport_introspection_tests/msg/arrays.h"
#include "rosidl_typesupport_introspection_tests/msg/basic_types.h"
#include "rosidl_typesupport_introspection_tests/msg/bounded_sequences.h"
#include "rosidl_typesupport_introspection_tests/msg/constants.h"
#include "rosidl_typesupport_introspection_tests/msg/defaults.h"
#include "rosidl_typesupport_introspection_tests/msg/empty.h"
#include "rosidl_typesupport_introspection_tests/msg/multi_nested.h"
#include "rosidl_typesupport_introspection_tests/msg/nested.h"
#include "rosidl_typesupport_introspection_tests/msg/strings.h"
#include "rosidl_typesupport_introspection_tests/msg/unbounded_sequences.h"
#include "rosidl_typesupport_introspection_tests/msg/w_strings.h"
#include "rosidl_typesupport_introspection_tests/srv/arrays.h"
#include "rosidl_typesupport_introspection_tests/srv/basic_types.h"
#include "rosidl_typesupport_introspection_tests/srv/empty.h"

#include "rosidl_typesupport_introspection_tests/msg/arrays.hpp"
#include "rosidl_typesupport_introspection_tests/msg/basic_types.hpp"
#include "rosidl_typesupport_introspection_tests/msg/bounded_sequences.hpp"
#include "rosidl_typesupport_introspection_tests/msg/constants.hpp"
#include "rosidl_typesupport_introspection_tests/msg/defaults.hpp"
#include "rosidl_typesupport_introspection_tests/msg/empty.hpp"
#include "rosidl_typesupport_introspection_tests/msg/multi_nested.hpp"
#include "rosidl_typesupport_introspection_tests/msg/nested.hpp"
#include "rosidl_typesupport_introspection_tests/msg/strings.hpp"
#include "rosidl_typesupport_introspection_tests/msg/w_strings.hpp"
#include "rosidl_typesupport_introspection_tests/msg/unbounded_sequences.hpp"
#include "rosidl_typesupport_introspection_tests/srv/arrays.hpp"
#include "rosidl_typesupport_introspection_tests/srv/basic_types.hpp"
#include "rosidl_typesupport_introspection_tests/srv/empty.hpp"

#include "rosidl_typesupport_introspection_tests/fixtures.hpp"
#include "rosidl_typesupport_introspection_tests/helpers.hpp"
#include "rosidl_typesupport_introspection_tests/libraries.hpp"
#include "rosidl_typesupport_introspection_tests/type_traits.hpp"

// Extra C++ APIs to homogeneize access to test interfaces in C and C++
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, Arrays)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, BasicTypes)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, BoundedSequences)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, Constants)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, Defaults)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, Empty)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, MultiNested)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, Nested)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, Strings)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, WStrings)
DEFINE_CXX_API_FOR_C_MESSAGE(rosidl_typesupport_introspection_tests, msg, UnboundedSequences)
DEFINE_CXX_API_FOR_C_SERVICE(rosidl_typesupport_introspection_tests, srv, Arrays)
DEFINE_CXX_API_FOR_C_SERVICE(rosidl_typesupport_introspection_tests, srv, BasicTypes)
DEFINE_CXX_API_FOR_C_SERVICE(rosidl_typesupport_introspection_tests, srv, Empty)

namespace rosidl_typesupport_introspection_tests
{

// Typesupport library definition for introspection of test interfaces in C
struct IntrospectionCTypeSupportTestLibrary
{
  using MessageDescriptorT =
    rosidl_typesupport_introspection_c__MessageMembers;
  using ServiceDescriptorT =
    rosidl_typesupport_introspection_c__ServiceMembers;
  using MemberDescriptorT =
    rosidl_typesupport_introspection_c__MessageMember;

  static constexpr const char * name = RCUTILS_STRINGIFY(
    ROSIDL_TYPESUPPORT_INTERFACE__LIBRARY_NAME(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests));
  static constexpr const char * identifier =
    "rosidl_typesupport_introspection_c";

  static constexpr const char * messages_namespace =
    "rosidl_typesupport_introspection_tests__msg";
  static constexpr const char * services_namespace =
    "rosidl_typesupport_introspection_tests__srv";
  static constexpr const char * actions_namespace =
    "rosidl_typesupport_introspection_tests__action";

  static constexpr const MessageTypeSupportSymbolRecord messages[] = {
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, Arrays),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, BasicTypes),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, BoundedSequences),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, Constants),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, Defaults),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, Empty),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, Strings),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, MultiNested),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, Nested),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, UnboundedSequences),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, msg, WStrings)
  };
  static constexpr const ServiceTypeSupportSymbolRecord services[] = {
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, srv, Arrays),
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, srv, BasicTypes),
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_c,
      rosidl_typesupport_introspection_tests, srv, Empty)
  };
  // static constexpr const ActionTypeSupportSymbolRecord actions[] = {
  //   ACTION_TYPESUPPORT_SYMBOL_RECORD(
  //     rosidl_typesupport_introspection_c,
  //     rosidl_typesupport_introspection_tests, action, Fibonacci)
  // };
};

// Traits to aid introspection of tests interfaces in C
template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__Arrays>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c,
    rosidl_typesupport_introspection_tests, msg, Arrays);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__BasicTypes>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg, BasicTypes);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__BoundedSequences>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg,
    BoundedSequences);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__Constants>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg, Constants);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__Defaults>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg, Defaults);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__Empty>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg, Empty);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__MultiNested>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg, MultiNested);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__Nested>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg, Nested);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__Strings>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg, Strings);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__UnboundedSequences>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg,
    UnboundedSequences);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__msg__WStrings>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c, rosidl_typesupport_introspection_tests, msg, WStrings);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__srv__Arrays>
{
  static constexpr const ServiceTypeSupportSymbolRecord typesupport =
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c,
    rosidl_typesupport_introspection_tests, srv, Arrays);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__srv__BasicTypes>
{
  static constexpr const ServiceTypeSupportSymbolRecord typesupport =
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c,
    rosidl_typesupport_introspection_tests, srv, BasicTypes);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests__srv__Empty>
{
  static constexpr const ServiceTypeSupportSymbolRecord typesupport =
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_c,
    rosidl_typesupport_introspection_tests, srv, Empty);
  using TypeSupportLibraryT = IntrospectionCTypeSupportTestLibrary;
};

// Examples of test interfaces in C, useful in test fixtures
template<>
struct Example<rosidl_typesupport_introspection_tests__msg__Arrays>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__Arrays,
      std::function<void (rosidl_typesupport_introspection_tests__msg__Arrays *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__Arrays * message) {
        rosidl_typesupport_introspection_tests__msg__Arrays__fini(message);
        delete message;
      };
    ReturnT message{new rosidl_typesupport_introspection_tests__msg__Arrays, deleter};
    if (!rosidl_typesupport_introspection_tests__msg__Arrays__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->bool_values[2] = true;
    message->float64_values[1] = 1.234;
    message->uint16_values[0] = 1234u;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__msg__BasicTypes>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__BasicTypes,
      std::function<void (rosidl_typesupport_introspection_tests__msg__BasicTypes *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__BasicTypes * message) {
        rosidl_typesupport_introspection_tests__msg__BasicTypes__fini(message);
        delete message;
      };
    ReturnT message{new rosidl_typesupport_introspection_tests__msg__BasicTypes, deleter};
    if (!rosidl_typesupport_introspection_tests__msg__BasicTypes__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->bool_value = true;
    message->float32_value = 1.234f;
    message->uint16_value = 1234u;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__msg__BoundedSequences>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__BoundedSequences,
      std::function<void (rosidl_typesupport_introspection_tests__msg__BoundedSequences *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__BoundedSequences * message) {
        rosidl_typesupport_introspection_tests__msg__BoundedSequences__fini(message);
        delete message;
      };
    ReturnT message{new rosidl_typesupport_introspection_tests__msg__BoundedSequences, deleter};
    if (
      !rosidl_typesupport_introspection_tests__msg__BoundedSequences__init(message.get()) ||
      !rosidl_runtime_c__boolean__Sequence__init(&message->bool_values, 1u) ||
      !rosidl_runtime_c__byte__Sequence__init(&message->byte_values, 1u) ||
      !rosidl_runtime_c__uint8__Sequence__init(&message->char_values, 1u) ||
      !rosidl_runtime_c__float__Sequence__init(&message->float32_values, 1u) ||
      !rosidl_runtime_c__double__Sequence__init(&message->float64_values, 1u) ||
      !rosidl_runtime_c__int8__Sequence__init(&message->int8_values, 1u) ||
      !rosidl_runtime_c__uint8__Sequence__init(&message->uint8_values, 1u) ||
      !rosidl_runtime_c__int16__Sequence__init(&message->int16_values, 1u) ||
      !rosidl_runtime_c__uint16__Sequence__init(&message->uint16_values, 1u) ||
      !rosidl_runtime_c__int32__Sequence__init(&message->int32_values, 1u) ||
      !rosidl_runtime_c__uint32__Sequence__init(&message->uint32_values, 1u) ||
      !rosidl_runtime_c__int64__Sequence__init(&message->int64_values, 1u) ||
      !rosidl_runtime_c__uint64__Sequence__init(&message->uint64_values, 1u) ||
      !rosidl_runtime_c__String__Sequence__init(&message->string_values, 1u))
    {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->bool_values.data[0] = true;
    message->byte_values.data[0] = 0x1B;
    message->char_values.data[0] = 'z';
    message->float32_values.data[0] = 12.34f;
    message->float64_values.data[0] = 1.234;
    message->int8_values.data[0] = -64;
    message->uint8_values.data[0] = 64u;
    message->int16_values.data[0] = -512;
    message->uint16_values.data[0] = 512u;
    message->int32_values.data[0] = -262144;
    message->uint32_values.data[0] = 262144u;
    message->int64_values.data[0] = -12341234l;
    message->uint64_values.data[0] = 12341234ul;
    if (!rosidl_runtime_c__String__assign(&message->string_values.data[0], "foo")) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__msg__Defaults>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__Defaults,
      std::function<void (rosidl_typesupport_introspection_tests__msg__Defaults *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__Defaults * message) {
        rosidl_typesupport_introspection_tests__msg__Defaults__fini(message);
        delete message;
      };
    ReturnT message{new rosidl_typesupport_introspection_tests__msg__Defaults, deleter};
    if (!rosidl_typesupport_introspection_tests__msg__Defaults__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->bool_value = !message->bool_value;
    message->uint32_value = message->uint32_value / 2u;
    message->float64_value = -message->float64_value;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__msg__MultiNested>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__MultiNested,
      std::function<void (rosidl_typesupport_introspection_tests__msg__MultiNested *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__MultiNested * message) {
        rosidl_typesupport_introspection_tests__msg__MultiNested__fini(message);
        delete message;
      };
    ReturnT message{new rosidl_typesupport_introspection_tests__msg__MultiNested, deleter};
    if (!rosidl_typesupport_introspection_tests__msg__MultiNested__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->array_of_arrays[1].int32_values[0] = -1234;
    if (
      !rosidl_typesupport_introspection_tests__msg__Arrays__Sequence__init(
        &message->bounded_sequence_of_arrays, 1u) ||
      !rosidl_typesupport_introspection_tests__msg__BoundedSequences__Sequence__init(
        &message->bounded_sequence_of_bounded_sequences, 1u) ||
      !rosidl_typesupport_introspection_tests__msg__UnboundedSequences__Sequence__init(
        &message->bounded_sequence_of_unbounded_sequences, 1u) ||
      !rosidl_typesupport_introspection_tests__msg__Arrays__Sequence__init(
        &message->unbounded_sequence_of_arrays, 1u) ||
      !rosidl_typesupport_introspection_tests__msg__BoundedSequences__Sequence__init(
        &message->unbounded_sequence_of_bounded_sequences, 1u) ||
      !rosidl_typesupport_introspection_tests__msg__UnboundedSequences__Sequence__init(
        &message->unbounded_sequence_of_unbounded_sequences, 1u))
    {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__msg__Nested>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__Nested,
      std::function<void (rosidl_typesupport_introspection_tests__msg__Nested *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__Nested * message) {
        rosidl_typesupport_introspection_tests__msg__Nested__fini(message);
        delete message;
      };
    ReturnT message{new rosidl_typesupport_introspection_tests__msg__Nested, deleter};
    if (!rosidl_typesupport_introspection_tests__msg__Nested__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->basic_types_value.bool_value = true;
    message->basic_types_value.char_value = 'x';
    message->basic_types_value.int16_value = -4321;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__msg__Strings>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__Strings,
      std::function<void (rosidl_typesupport_introspection_tests__msg__Strings *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__Strings * message) {
        rosidl_typesupport_introspection_tests__msg__Strings__fini(message);
        delete message;
      };
    ReturnT message{new rosidl_typesupport_introspection_tests__msg__Strings, deleter};
    if (
      !rosidl_typesupport_introspection_tests__msg__Strings__init(message.get()) ||
      !rosidl_runtime_c__String__assign(&message->string_value, "foo") ||
      !rosidl_runtime_c__String__assign(&message->bounded_string_value, "bar"))
    {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__msg__UnboundedSequences>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__UnboundedSequences,
      std::function<void (rosidl_typesupport_introspection_tests__msg__UnboundedSequences *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__UnboundedSequences * message) {
        rosidl_typesupport_introspection_tests__msg__UnboundedSequences__fini(message);
        delete message;
      };
    ReturnT message{new rosidl_typesupport_introspection_tests__msg__UnboundedSequences, deleter};
    if (
      !rosidl_typesupport_introspection_tests__msg__UnboundedSequences__init(message.get()) ||
      !rosidl_runtime_c__boolean__Sequence__init(&message->bool_values, 1u) ||
      !rosidl_runtime_c__byte__Sequence__init(&message->byte_values, 1u) ||
      !rosidl_runtime_c__uint8__Sequence__init(&message->char_values, 1u) ||
      !rosidl_runtime_c__float__Sequence__init(&message->float32_values, 1u) ||
      !rosidl_runtime_c__double__Sequence__init(&message->float64_values, 1u) ||
      !rosidl_runtime_c__int8__Sequence__init(&message->int8_values, 1u) ||
      !rosidl_runtime_c__uint8__Sequence__init(&message->uint8_values, 1u) ||
      !rosidl_runtime_c__int16__Sequence__init(&message->int16_values, 1u) ||
      !rosidl_runtime_c__uint16__Sequence__init(&message->uint16_values, 1u) ||
      !rosidl_runtime_c__int32__Sequence__init(&message->int32_values, 1u) ||
      !rosidl_runtime_c__uint32__Sequence__init(&message->uint32_values, 1u) ||
      !rosidl_runtime_c__int64__Sequence__init(&message->int64_values, 1u) ||
      !rosidl_runtime_c__uint64__Sequence__init(&message->uint64_values, 1u) ||
      !rosidl_runtime_c__String__Sequence__init(&message->string_values, 1u))
    {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->bool_values.data[0] = true;
    message->byte_values.data[0] = 0x1B;
    message->char_values.data[0] = 'z';
    message->float32_values.data[0] = 12.34f;
    message->float64_values.data[0] = 1.234;
    message->int8_values.data[0] = -64;
    message->uint8_values.data[0] = 64u;
    message->int16_values.data[0] = -512;
    message->uint16_values.data[0] = 512u;
    message->int32_values.data[0] = -262144;
    message->uint32_values.data[0] = 262144u;
    message->int64_values.data[0] = -12341234l;
    message->uint64_values.data[0] = 12341234ul;
    if (!rosidl_runtime_c__String__assign(&message->string_values.data[0], "foo")) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__msg__WStrings>
{
  static auto Make()
  {
    using ReturnT = std::unique_ptr<
      rosidl_typesupport_introspection_tests__msg__WStrings,
      std::function<void (rosidl_typesupport_introspection_tests__msg__WStrings *)>>;
    auto deleter = [](rosidl_typesupport_introspection_tests__msg__WStrings * message) {
        rosidl_typesupport_introspection_tests__msg__WStrings__fini(message);
        delete message;
      };

    ReturnT message{new rosidl_typesupport_introspection_tests__msg__WStrings, deleter};
    const uint16_t wstring_value[] = {50097u, 117u, 0u};
    if (
      !rosidl_typesupport_introspection_tests__msg__WStrings__init(message.get()) ||
      !rosidl_runtime_c__U16String__assign(&message->wstring_value, wstring_value))
    {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__srv__Arrays>
{
  static auto MakeRequest()
  {
    using MessageT = rosidl_typesupport_introspection_tests__srv__Arrays_Request;
    auto deleter = [](MessageT * message) {
        rosidl_typesupport_introspection_tests__srv__Arrays_Request__fini(message);
        delete message;
      };
    using ReturnT = std::unique_ptr<MessageT, std::function<void (MessageT *)>>;
    ReturnT message{new MessageT, deleter};
    if (!rosidl_typesupport_introspection_tests__srv__Arrays_Request__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->bool_values[2] = true;
    message->float64_values[1] = 1.234;
    message->uint16_values[0] = 1234u;
    return message;
  }

  static auto MakeResponse()
  {
    using MessageT = rosidl_typesupport_introspection_tests__srv__Arrays_Response;
    auto deleter = [](MessageT * message) {
        rosidl_typesupport_introspection_tests__srv__Arrays_Response__fini(message);
        delete message;
      };
    using ReturnT = std::unique_ptr<MessageT, std::function<void (MessageT *)>>;
    ReturnT message{new MessageT, deleter};
    if (!rosidl_typesupport_introspection_tests__srv__Arrays_Response__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->byte_values[1] = 0xAB;
    message->char_values[0] = 'b';
    message->int8_values[2] = 123;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests__srv__BasicTypes>
{
  static auto MakeRequest()
  {
    using MessageT =
      rosidl_typesupport_introspection_tests__srv__BasicTypes_Request;
    auto deleter = [](MessageT * message) {
        rosidl_typesupport_introspection_tests__srv__BasicTypes_Request__fini(message);
        delete message;
      };
    using ReturnT = std::unique_ptr<MessageT, std::function<void (MessageT *)>>;
    ReturnT message{new MessageT, deleter};
    if (!rosidl_typesupport_introspection_tests__srv__BasicTypes_Request__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->char_value = 'c';
    message->uint32_value = 1234u;
    message->float32_value = 1.234f;
    if (!rosidl_runtime_c__String__assign(&message->string_value, "foo")) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    return message;
  }

  static auto MakeResponse()
  {
    using MessageT =
      rosidl_typesupport_introspection_tests__srv__BasicTypes_Response;
    auto deleter = [](MessageT * message) {
        rosidl_typesupport_introspection_tests__srv__BasicTypes_Response__fini(message);
        delete message;
      };
    using ReturnT = std::unique_ptr<MessageT, std::function<void (MessageT *)>>;
    ReturnT message{new MessageT, deleter};
    if (!rosidl_typesupport_introspection_tests__srv__BasicTypes_Response__init(message.get())) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    message->bool_value = true;
    message->byte_value = 0xAB;
    message->float64_value = -1.234;
    if (!rosidl_runtime_c__String__assign(&message->string_value, "bar")) {
      throw std::runtime_error(rcutils_get_error_string().str);
    }
    return message;
  }
};

// Typesupport library definition for introspection of test interfaces in C++
struct IntrospectionCppTypeSupportTestLibrary
{
  using MessageDescriptorT =
    rosidl_typesupport_introspection_cpp::MessageMembers;
  using ServiceDescriptorT =
    rosidl_typesupport_introspection_cpp::ServiceMembers;
  using MemberDescriptorT =
    rosidl_typesupport_introspection_cpp::MessageMember;

  static constexpr const char * name = RCUTILS_STRINGIFY(
    ROSIDL_TYPESUPPORT_INTERFACE__LIBRARY_NAME(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests));
  static constexpr const char * identifier =
    "rosidl_typesupport_introspection_cpp";

  static constexpr const char * messages_namespace =
    "rosidl_typesupport_introspection_tests::msg";
  static constexpr const char * services_namespace =
    "rosidl_typesupport_introspection_tests::srv";
  static constexpr const char * actions_namespace =
    "rosidl_typesupport_introspection_tests::action";

  static constexpr const MessageTypeSupportSymbolRecord messages[] = {
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, Arrays),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, BasicTypes),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, BoundedSequences),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, Constants),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, Defaults),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, Empty),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, MultiNested),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, Nested),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, Strings),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, UnboundedSequences),
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, msg, WStrings)
  };
  static constexpr const ServiceTypeSupportSymbolRecord services[] = {
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, srv, Arrays),
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, srv, BasicTypes),
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
      rosidl_typesupport_introspection_cpp,
      rosidl_typesupport_introspection_tests, srv, Empty)
  };
  // static constexpr const ActionTypeSupportSymbolRecord actions[] = {
  //   ACTION_TYPESUPPORT_SYMBOL_RECORD(
  //     rosidl_typesupport_introspection_cpp,
  //     rosidl_typesupport_introspection_tests, action, Fibonacci)
  // };
};

// Traits to aid introspection of `rosidl_typesupport_introspection_tests` package interfaces in C++
template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::Arrays>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, Arrays);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::BasicTypes>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, BasicTypes);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::BoundedSequences>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, BoundedSequences);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::Constants>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, Constants);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::Defaults>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, Defaults);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::Empty>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, Empty);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::MultiNested>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, MultiNested);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::Nested>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, Nested);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::Strings>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, Strings);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::UnboundedSequences>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, UnboundedSequences);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::msg::WStrings>
{
  static constexpr const MessageTypeSupportSymbolRecord typesupport =
    MESSAGE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, msg, WStrings);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::srv::Arrays>
{
  static constexpr const ServiceTypeSupportSymbolRecord typesupport =
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, srv, Arrays);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::srv::BasicTypes>
{
  static constexpr const ServiceTypeSupportSymbolRecord typesupport =
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, srv, BasicTypes);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

template<>
struct introspection_traits<rosidl_typesupport_introspection_tests::srv::Empty>
{
  static constexpr const ServiceTypeSupportSymbolRecord typesupport =
    SERVICE_TYPESUPPORT_SYMBOL_RECORD(
    rosidl_typesupport_introspection_cpp,
    rosidl_typesupport_introspection_tests, srv, Empty);
  using TypeSupportLibraryT = IntrospectionCppTypeSupportTestLibrary;
};

// Examples of test interfaces in C++, useful in test fixtures
template<>
struct Example<rosidl_typesupport_introspection_tests::msg::Arrays>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::Arrays> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::Arrays>();
    message->bool_values[2] = true;
    message->float64_values[1] = 1.234;
    message->uint16_values[0] = 1234u;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::msg::BasicTypes>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::BasicTypes> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::BasicTypes>();
    message->bool_value = true;
    message->float32_value = 1.234f;
    message->uint16_value = 1234u;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::msg::BoundedSequences>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::BoundedSequences> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::BoundedSequences>();
    message->bool_values.push_back(true);
    message->byte_values.push_back(0x1B);
    message->char_values.push_back('z');
    message->float32_values.push_back(12.34f);
    message->float64_values.push_back(1.234);
    message->int8_values.push_back(-64);
    message->uint8_values.push_back(64u);
    message->int16_values.push_back(-512);
    message->uint16_values.push_back(512u);
    message->int32_values.push_back(-262144);
    message->uint32_values.push_back(262144u);
    message->int64_values.push_back(-12341234l);
    message->uint64_values.push_back(12341234ul);
    message->string_values.push_back("foo");
    message->basic_types_values.emplace_back();
    message->constants_values.emplace_back();
    message->defaults_values.emplace_back();
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::msg::Defaults>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::Defaults> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::Defaults>();
    message->bool_value = !message->bool_value;
    message->uint32_value = message->uint32_value / 2u;
    message->float64_value = -message->float64_value;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::msg::MultiNested>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::MultiNested> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::MultiNested>();
    message->array_of_arrays[1].int32_values[0] = -1234;
    message->unbounded_sequence_of_arrays.emplace_back();
    message->unbounded_sequence_of_arrays[0].char_values[2] = 'a';
    message->bounded_sequence_of_arrays.emplace_back();
    message->bounded_sequence_of_bounded_sequences.emplace_back();
    message->bounded_sequence_of_unbounded_sequences.emplace_back();
    message->unbounded_sequence_of_arrays.emplace_back();
    message->unbounded_sequence_of_bounded_sequences.emplace_back();
    message->unbounded_sequence_of_unbounded_sequences.emplace_back();
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::msg::Nested>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::Nested> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::Nested>();
    message->basic_types_value.bool_value = true;
    message->basic_types_value.char_value = 'x';
    message->basic_types_value.int16_value = -4321;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::msg::Strings>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::Strings> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::Strings>();
    message->string_value = "foo";
    message->bounded_string_value = "bar";
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::msg::UnboundedSequences>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::UnboundedSequences> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::UnboundedSequences>();
    message->bool_values.push_back(true);
    message->byte_values.push_back(0x1B);
    message->char_values.push_back('z');
    message->float32_values.push_back(12.34f);
    message->float64_values.push_back(1.234);
    message->int8_values.push_back(-64);
    message->uint8_values.push_back(64u);
    message->int16_values.push_back(-512);
    message->uint16_values.push_back(512u);
    message->int32_values.push_back(-262144);
    message->uint32_values.push_back(262144u);
    message->int64_values.push_back(-12341234l);
    message->uint64_values.push_back(12341234ul);
    message->string_values.push_back("foo");
    message->basic_types_values.emplace_back();
    message->constants_values.emplace_back();
    message->defaults_values.emplace_back();
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::msg::WStrings>
{
  static
  std::unique_ptr<rosidl_typesupport_introspection_tests::msg::WStrings> Make()
  {
    auto message =
      std::make_unique<rosidl_typesupport_introspection_tests::msg::WStrings>();
    message->wstring_value = u"ñu";
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::srv::Arrays>
{
  static auto MakeRequest()
  {
    using MessageT =
      rosidl_typesupport_introspection_tests::srv::Arrays::Request;
    auto message = std::make_unique<MessageT>();
    message->bool_values[2] = true;
    message->float64_values[1] = 1.234;
    message->uint16_values[0] = 1234u;
    return message;
  }

  static auto MakeResponse()
  {
    using MessageT =
      rosidl_typesupport_introspection_tests::srv::Arrays::Response;
    auto message = std::make_unique<MessageT>();
    message->byte_values[1] = 0xAB;
    message->char_values[0] = 'b';
    message->int8_values[2] = 123;
    return message;
  }
};

template<>
struct Example<rosidl_typesupport_introspection_tests::srv::BasicTypes>
{
  static auto MakeRequest()
  {
    using MessageT =
      rosidl_typesupport_introspection_tests::srv::BasicTypes::Request;
    auto message = std::make_unique<MessageT>();
    message->char_value = 'c';
    message->uint32_value = 1234u;
    message->float32_value = 1.234f;
    message->string_value = "foo";
    return message;
  }

  static auto MakeResponse()
  {
    using MessageT =
      rosidl_typesupport_introspection_tests::srv::BasicTypes::Response;
    auto message = std::make_unique<MessageT>();
    message->bool_value = true;
    message->byte_value = 0xAB;
    message->float64_value = -1.234;
    message->string_value = "bar";
    return message;
  }
};

}  // namespace rosidl_typesupport_introspection_tests

#endif  // INTROSPECTION_LIBRARIES_UNDER_TEST_HPP_
