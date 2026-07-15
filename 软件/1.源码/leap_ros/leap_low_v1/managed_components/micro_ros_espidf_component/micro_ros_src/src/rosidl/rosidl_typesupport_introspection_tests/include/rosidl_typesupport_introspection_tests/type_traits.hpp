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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__TYPE_TRAITS_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__TYPE_TRAITS_HPP_

#include <type_traits>

/// Yields base type for any `expression`.
#define EXPRESSION_TYPE(expression) \
  typename std::remove_cv< \
    typename std::remove_reference< \
      decltype(expression)>::type>::type

/// Yields an expression for use in unevaluated context
#define MEMBER_EXPRESSION(type, expression) \
  std::declval<type>().expression

/// Yields base type for any `type{}.expression`
#define MEMBER_EXPRESSION_TYPE(type, expression) \
  EXPRESSION_TYPE(MEMBER_EXPRESSION(type, expression))

/// Yields base type for a `getitem(type{}.member, 0)` expression
#define MEMBER_ITEM_TYPE(type, member) \
  EXPRESSION_TYPE(getitem(MEMBER_EXPRESSION(type, member), 0));

/// Defines has_`member_name`<T> trait to check
/// for member existence in any type T.
#define DEFINE_HAS_MEMBER_TRAIT(member_name) \
  template<class T, typename = int> \
  struct has_ ## member_name : public std::false_type {}; \
  template<class T> \
  struct has_ ## member_name< \
    T, decltype((void) T::member_name, 0) \
  >: public std::true_type {}

namespace rosidl_typesupport_introspection_tests
{

// Base definition, to be specialized
template<typename InterfaceT>
struct interface_traits;

// Base definition, to be specialized
template<typename InterfaceT>
struct introspection_traits;

}  // namespace rosidl_typesupport_introspection_tests

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__TYPE_TRAITS_HPP_
