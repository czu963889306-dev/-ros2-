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

#include <gtest/gtest.h>

#include "rosidl_typesupport_introspection_tests/fixtures.hpp"
#include "rosidl_typesupport_introspection_tests/gtest/macros.hpp"
#include "rosidl_typesupport_introspection_tests/gtest/message_introspection_test.hpp"
#include "rosidl_typesupport_introspection_tests/api.hpp"
#include "rosidl_typesupport_introspection_tests/type_traits.hpp"

#include "introspection_libraries_under_test.hpp"

namespace rosidl_typesupport_introspection_tests
{
namespace testing
{
namespace
{

template<typename DefaultsMessageT>
class DefaultsMessageIntrospectionTest
  : public MessageIntrospectionTest<DefaultsMessageT>
{
};

using DefaultsMessageTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__msg__Defaults,
  rosidl_typesupport_introspection_tests::msg::Defaults>;
TYPED_TEST_SUITE(DefaultsMessageIntrospectionTest, DefaultsMessageTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(DefaultsMessageIntrospectionTest, MessageDescriptorIsCorrect)
{
  using DefaultsMessageT = TypeParam;

  using TypeSupportLibraryT =
    typename introspection_traits<DefaultsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();

  EXPECT_STREQ(
    get_message_namespace(message_descriptor),
    TypeSupportLibraryT::messages_namespace);
  EXPECT_STREQ(get_message_name(message_descriptor), "Defaults");
  EXPECT_EQ(get_message_size(message_descriptor), sizeof(DefaultsMessageT));
  ASSERT_EQ(get_member_count(message_descriptor), 13u);

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 0u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bool_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_BOOLEAN));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 1u);
    EXPECT_STREQ(get_member_name(member_descriptor), "byte_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_OCTET));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 2u);
    EXPECT_STREQ(get_member_name(member_descriptor), "char_value");
    // In ROS message definitions, char is an alias for uint8.
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 3u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_FLOAT));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 4u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_DOUBLE));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 5u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int8_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 6u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint8_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 7u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int16_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT16));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 8u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint16_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT16));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 9u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT32));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 10u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT32));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 11u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT64));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 12u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT64));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }
}

TYPED_TEST(DefaultsMessageIntrospectionTest, CanReadTypeErasedMessage)
{
  using DefaultsMessageT = TypeParam;

  const auto message_ptr = Example<DefaultsMessageT>::Make();
  const DefaultsMessageT & message = *message_ptr;
  const void * type_erased_message = message_ptr.get();

  using TypeSupportLibraryT =
    typename introspection_traits<DefaultsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 13u);

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bool_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, byte_value,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, char_value,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, float32_value,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, float64_value,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int8_value,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint8_value,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int16_value,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint16_value,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int32_value,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint32_value,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int64_value,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint64_value,
    get_member_descriptor(message_descriptor, 12u));
}

TYPED_TEST(DefaultsMessageIntrospectionTest, CanWriteTypeErasedMessage)
{
  using DefaultsMessageT = TypeParam;

  const auto message_ptr = Example<DefaultsMessageT>::Make();
  const DefaultsMessageT & message = *message_ptr;

  auto type_erased_message_copy = this->MakeTypeErasedMessage();
  const DefaultsMessageT & message_copy =
    *reinterpret_cast<DefaultsMessageT *>(type_erased_message_copy.get());
  EXPECT_NE(message, message_copy);

  using TypeSupportLibraryT =
    typename introspection_traits<DefaultsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 13u);

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bool_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, byte_value,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, char_value,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float32_value,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float64_value,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int8_value,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint8_value,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int16_value,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint16_value,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int32_value,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint32_value,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int64_value,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint64_value,
    get_member_descriptor(message_descriptor, 12u));

  EXPECT_EQ(message, message_copy);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
