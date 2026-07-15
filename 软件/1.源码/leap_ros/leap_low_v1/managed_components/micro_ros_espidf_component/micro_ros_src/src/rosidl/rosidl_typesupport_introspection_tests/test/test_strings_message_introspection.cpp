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

template<typename StringsMessageT>
class StringsMessageIntrospectionTest
  : public MessageIntrospectionTest<StringsMessageT>
{
};

using StringsMessageTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__msg__Strings,
  rosidl_typesupport_introspection_tests::msg::Strings>;
TYPED_TEST_SUITE(StringsMessageIntrospectionTest, StringsMessageTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(StringsMessageIntrospectionTest, MessageDescriptorIsCorrect)
{
  using StringsMessageT = TypeParam;

  using TypeSupportLibraryT =
    typename introspection_traits<StringsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();

  EXPECT_STREQ(
    get_message_namespace(message_descriptor),
    TypeSupportLibraryT::messages_namespace);
  EXPECT_STREQ(get_message_name(message_descriptor), "Strings");
  EXPECT_EQ(get_message_size(message_descriptor), sizeof(StringsMessageT));
  ASSERT_EQ(get_member_count(message_descriptor), 12u);

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 0u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_value");
    EXPECT_TRUE(is_string_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 1u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_value_default1");
    EXPECT_TRUE(is_string_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 2u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_value_default2");
    EXPECT_TRUE(is_string_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 3u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_value_default3");
    EXPECT_TRUE(is_string_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 4u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_value_default4");
    EXPECT_TRUE(is_string_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 5u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_value_default5");
    EXPECT_TRUE(is_string_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 6u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_string_value");
    EXPECT_TRUE(is_string_member(member_descriptor, 22u));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 7u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_string_value_default1");
    EXPECT_TRUE(is_string_member(member_descriptor, 22u));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 8u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_string_value_default2");
    EXPECT_TRUE(is_string_member(member_descriptor, 22u));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 9u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_string_value_default3");
    EXPECT_TRUE(is_string_member(member_descriptor, 22u));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 10u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_string_value_default4");
    EXPECT_TRUE(is_string_member(member_descriptor, 22u));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 11u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_string_value_default5");
    EXPECT_TRUE(is_string_member(member_descriptor, 22u));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }
}

TYPED_TEST(StringsMessageIntrospectionTest, CanReadTypeErasedMessage)
{
  using StringsMessageT = TypeParam;

  const auto message_ptr = Example<StringsMessageT>::Make();
  const StringsMessageT & message = *message_ptr;
  const void * type_erased_message = message_ptr.get();

  using TypeSupportLibraryT =
    typename introspection_traits<StringsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 12u);

  EXPECT_MEMBER_EQ(
    type_erased_message, message, string_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, string_value_default1,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, string_value_default2,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, string_value_default3,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, string_value_default4,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, string_value_default5,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bounded_string_value,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bounded_string_value_default1,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bounded_string_value_default2,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bounded_string_value_default3,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bounded_string_value_default4,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bounded_string_value_default5,
    get_member_descriptor(message_descriptor, 11u));
}

TYPED_TEST(StringsMessageIntrospectionTest, CanWriteTypeErasedMessage)
{
  using StringsMessageT = TypeParam;

  const auto message_ptr = Example<StringsMessageT>::Make();
  const StringsMessageT & message = *message_ptr;

  auto type_erased_message_copy = this->MakeTypeErasedMessage();
  const StringsMessageT & message_copy =
    *reinterpret_cast<StringsMessageT *>(type_erased_message_copy.get());
  EXPECT_NE(message, message_copy);

  using TypeSupportLibraryT =
    typename introspection_traits<StringsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 12u);

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_value_default1,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_value_default2,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_value_default3,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_value_default4,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_value_default5,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bounded_string_value,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bounded_string_value_default1,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bounded_string_value_default2,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bounded_string_value_default3,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bounded_string_value_default4,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bounded_string_value_default5,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_EQ(message, message_copy);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
