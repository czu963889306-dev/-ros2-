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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__MACROS_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__MACROS_HPP_

#include <gtest/gtest.h>

#include <type_traits>

#include "rosidl_typesupport_introspection_tests/helpers.hpp"
#include "rosidl_typesupport_introspection_tests/type_traits.hpp"

/// Expects equality of an iterable message member (i.e. an array or
/// a sequence) between a statically typed message and a type erased
/// message via introspection APIs
#define EXPECT_ITERABLE_MEMBER_EQ( \
    type_erased_message, message, member_name, member_descriptor) \
  { \
    EXPECT_STREQ(member_descriptor->name_, #member_name); \
    using member_base_type = \
      EXPRESSION_TYPE(getitem(message.member_name, 0)); \
    const void * type_erased_member = \
      get_const_member(type_erased_message, member_descriptor); \
    const size_t size = get_member_size( \
      type_erased_member, member_descriptor); \
    for (size_t i = 0u; i < size; ++i) { \
      const auto item = fetch_member_item<member_base_type>( \
        type_erased_member, member_descriptor, i); \
      EXPECT_EQ(item, getitem(message.member_name, i)); \
      if (has_support_for_direct_memory_access(member_descriptor)) { \
        const auto & item_reference = get_member_item<member_base_type>( \
          type_erased_member, member_descriptor, i); \
        EXPECT_EQ(item_reference, getitem(message.member_name, i)); \
      } \
    } \
  }

/// Expects equality of a message member between a statically typed
/// message and a type erased message via introspection APIs
#define EXPECT_MEMBER_EQ( \
    type_erased_message, message, member_name, member_descriptor) \
  { \
    EXPECT_STREQ(member_descriptor->name_, #member_name); \
    using member_type = EXPRESSION_TYPE(message.member_name); \
    const auto & member = get_const_member<member_type>( \
      type_erased_message, member_descriptor); \
    EXPECT_EQ(member, message.member_name); \
  }

/// Expects that assignment of an array message member on a type erased
/// message via introspection APIs (may incur memory corruption, make
/// sure to validate).
#define EXPECT_ARRAY_MEMBER_ASSIGNMENT( \
    type_erased_message, message, member_name, member_descriptor) \
  { \
    EXPECT_STREQ(member_descriptor->name_, #member_name); \
    using member_base_type = EXPRESSION_TYPE(message.member_name[0]); \
    void * type_erased_member = \
      get_member(type_erased_message, member_descriptor); \
    for (size_t i = 0u; i < length(message.member_name); ++i) { \
      assign_member_item<member_base_type>( \
        type_erased_member, member_descriptor, \
        i, deepcopy(message.member_name[i])); \
      if (has_support_for_direct_memory_access(member_descriptor)) { \
        auto & item_reference = get_member_item<member_base_type>( \
          type_erased_member, member_descriptor, i); \
        EXPECT_EQ(item_reference, message.member_name[i]); \
      } \
    } \
  }

/// Expects that assignment of a sequence message member on a type erased
/// message via introspection APIs (may incur memory corruption, make
/// sure to validate).
#define EXPECT_SEQUENCE_MEMBER_ASSIGNMENT( \
    type_erased_message, message, member_name, member_descriptor) \
  { \
    EXPECT_STREQ(member_descriptor->name_, #member_name); \
    using member_base_type = \
      EXPRESSION_TYPE(getitem(message.member_name, 0)); \
    void * type_erased_member = \
      get_member(type_erased_message, member_descriptor); \
    const size_t size = length(message.member_name); \
    resize_member(type_erased_member, member_descriptor, size); \
    for (size_t i = 0u; i < size; ++i) { \
      assign_member_item<member_base_type>( \
        type_erased_member, member_descriptor, \
        i, deepcopy(getitem(message.member_name, i))); \
      if (has_support_for_direct_memory_access(member_descriptor)) { \
        auto & item_reference = get_member_item<member_base_type>( \
          type_erased_member, member_descriptor, i); \
        EXPECT_EQ(item_reference, getitem(message.member_name, i)); \
      } \
    } \
  }

/// Expects that assignment of a message member on a type erased message via
/// introspection APIs (may incur memory corruption, make sure to validate).
#define EXPECT_MEMBER_ASSIGNMENT( \
    type_erased_message, message, member_name, member_descriptor) \
  { \
    EXPECT_STREQ(member_descriptor->name_, #member_name); \
    using member_type = EXPRESSION_TYPE(message.member_name); \
    auto & member = get_member<member_type>( \
      type_erased_message, member_descriptor); \
    member = deepcopy(message.member_name); \
  }

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__MACROS_HPP_
