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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__HELPERS_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__HELPERS_HPP_

#include <rcutils/error_handling.h>
#include <rcutils/macros.h>

#include <rosidl_runtime_c/primitives_sequence_functions.h>
#include <rosidl_runtime_c/string_functions.h>
#include <rosidl_runtime_c/u16string_functions.h>

#include <stdexcept>
#include <vector>

#include <rosidl_runtime_cpp/bounded_vector.hpp>

/// Performs a deep-copy of the given `value`.
/**
 * A best effort base definition, works for most C++ types.
 */
template<typename T>
inline T deepcopy(T value) {return value;}

/// Defines a `deepcopy()` overload for `type`, assuming
/// it is in the set of non-basic C message member types.
#define DEFINE_DEEPCOPY_OVERLOAD_FOR_C_MESSAGE_MEMBER(type) \
  inline type deepcopy(const type & input) { \
    type output; \
    if (!RCUTILS_JOIN(type, __init)(&output)) { \
      throw std::runtime_error(rcutils_get_error_string().str); \
    } \
    if (!RCUTILS_JOIN(type, __copy)(&input, &output)) { \
      RCUTILS_JOIN(type, __fini)(&output); \
      throw std::runtime_error(rcutils_get_error_string().str); \
    } \
    return output; \
  }

/// Defines a `deepcopy()` overload for `type`,
/// assuming it is C message sequence member type.
#define DEFINE_DEEPCOPY_OVERLOAD_FOR_C_MESSAGE_SEQUENCE_MEMBER(type) \
  inline type deepcopy(const type & input) { \
    type output; \
    if (!RCUTILS_JOIN(type, __init)(&output, 0u)) { \
      throw std::runtime_error(rcutils_get_error_string().str); \
    } \
    if (!RCUTILS_JOIN(type, __copy)(&input, &output)) { \
      throw std::runtime_error(rcutils_get_error_string().str); \
    } \
    return output; \
  }

/// Returns the size of a plain array.
template<typename T, size_t N>
inline constexpr size_t length(const T (&)[N]) {return N;}

/// Returns the size of an std::array.
template<typename T, size_t N>
inline constexpr size_t length(const std::array<T, N> &) {return N;}

/// Returns the size of an rosidl_runtime_cpp::BoundedVector.
template<typename T, size_t N>
inline size_t
length(const rosidl_runtime_cpp::BoundedVector<T, N> & vector)
{
  return vector.size();
}

/// Returns the size of an std::vector.
template<typename T>
inline size_t
length(const std::vector<T> & vector)
{
  return vector.size();
}

/// Gets a reference to the item at `index` in `array`.
template<typename T>
inline const T &
getitem(const T array[], const size_t index)
{
  return array[index];
}

/// Gets a reference to the item at `index` in `vector`.
template<typename T>
inline const T &
getitem(const std::vector<T> & vector, const size_t index)
{
  return vector[index];
}

// Deal with std::vector<bool> quirks.
inline bool
getitem(const std::vector<bool> & vector, const size_t index)
{
  return vector[index];
}

/// Gets a reference to the item at `index` in `vector`.
template<typename T, size_t N>
inline const T & getitem(
  const rosidl_runtime_cpp::BoundedVector<T, N> & vector,
  const size_t index)
{
  return vector[index];
}

// Deal with rosidl_runtime_cpp::BoundedVector<bool, N> quirks.
template<size_t N>
inline bool getitem(
  const rosidl_runtime_cpp::BoundedVector<bool, N> & vector,
  const size_t index)
{
  return vector[index];
}

/// Gets a reference to the item at `index` in `array`.
template<typename T, size_t N>
inline const T &
getitem(const std::array<T, N> & array, const size_t index)
{
  return array[index];
}

/// Defines a `getitem()` overload for `type`, assuming
/// it is a C message sequence member type.
#define DEFINE_GETITEM_OVERLOAD_FOR_C_MESSAGE_SEQUENCE_MEMBER(type) \
  inline auto & getitem(const type & seq, const size_t index) { \
    return seq.data[index]; \
  }

/// Defines a `getitem()` overload for `type`, assuming
/// it is a C message sequence member type.
#define DEFINE_LENGTH_OVERLOAD_FOR_C_MESSAGE_SEQUENCE_MEMBER(type) \
  inline size_t length(const type & seq) {return seq.size;}

/// Defines `operator==()` and `operator!=()` overloads for `type`,
/// assuming it is in the set of non-basic C message member types.
#define DEFINE_OPERATOR_OVERLOADS_FOR_C_MESSAGE_MEMBER(type) \
  inline bool operator==(const type & lhs, const type & rhs) { \
    return RCUTILS_JOIN(type, __are_equal)(&lhs, &rhs); \
  } \
  inline bool operator!=(const type & lhs, const type & rhs) { \
    return !RCUTILS_JOIN(type, __are_equal)(&lhs, &rhs); \
  }

/// Defines C++ helper API for `type`, assuming it is
/// in the set of of non-basic C message member types.
#define DEFINE_CXX_API_FOR_C_MESSAGE_MEMBER(type) \
  DEFINE_OPERATOR_OVERLOADS_FOR_C_MESSAGE_MEMBER(type) \
  DEFINE_DEEPCOPY_OVERLOAD_FOR_C_MESSAGE_MEMBER(type)

/// Defines C++ helper API for `type`, assuming it is
/// a C message sequence member type.
#define DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(type) \
  DEFINE_OPERATOR_OVERLOADS_FOR_C_MESSAGE_MEMBER(type) \
  DEFINE_DEEPCOPY_OVERLOAD_FOR_C_MESSAGE_SEQUENCE_MEMBER(type) \
  DEFINE_GETITEM_OVERLOAD_FOR_C_MESSAGE_SEQUENCE_MEMBER(type) \
  DEFINE_LENGTH_OVERLOAD_FOR_C_MESSAGE_SEQUENCE_MEMBER(type)

#define C_INTERFACE_NAME(package_name, interface_type, interface_name) \
  RCUTILS_JOIN( \
    RCUTILS_JOIN( \
      RCUTILS_JOIN( \
        RCUTILS_JOIN( \
          package_name, __), interface_type), __), interface_name)

/// Defines C++ helper API for a C message.
#define DEFINE_CXX_API_FOR_C_MESSAGE(package_name, interface_type, message_name) \
  DEFINE_CXX_API_FOR_C_MESSAGE_MEMBER( \
    C_INTERFACE_NAME(package_name, interface_type, message_name)) \
  DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER( \
    RCUTILS_JOIN(C_INTERFACE_NAME(package_name, interface_type, message_name), __Sequence))

/// Defines C++ helper API for a C service.
#define DEFINE_CXX_API_FOR_C_SERVICE(package_name, interface_type, service_name) \
  DEFINE_CXX_API_FOR_C_MESSAGE_MEMBER( \
    C_INTERFACE_NAME(package_name, interface_type, RCUTILS_JOIN(service_name, _Request))) \
  DEFINE_CXX_API_FOR_C_MESSAGE_MEMBER( \
    C_INTERFACE_NAME(package_name, interface_type, RCUTILS_JOIN(service_name, _Response))) \
  struct C_INTERFACE_NAME (package_name, interface_type, service_name) { \
  using Request = C_INTERFACE_NAME( \
    package_name, interface_type, RCUTILS_JOIN( \
      service_name, \
      _Request)); \
  using Response = \
    C_INTERFACE_NAME(package_name, interface_type, RCUTILS_JOIN(service_name, _Response)); \
};

// Extra C++ APIs to homogeneize access to rosidl_runtime_c primitives
DEFINE_CXX_API_FOR_C_MESSAGE_MEMBER(rosidl_runtime_c__String)
DEFINE_CXX_API_FOR_C_MESSAGE_MEMBER(rosidl_runtime_c__U16String)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__float__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__double__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__long_double__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__char__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__wchar__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__boolean__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__octet__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__uint8__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__int8__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__uint16__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__int16__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__uint32__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__int32__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__uint64__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__int64__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__String__Sequence)
DEFINE_CXX_API_FOR_C_MESSAGE_SEQUENCE_MEMBER(rosidl_runtime_c__U16String__Sequence)

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__HELPERS_HPP_
