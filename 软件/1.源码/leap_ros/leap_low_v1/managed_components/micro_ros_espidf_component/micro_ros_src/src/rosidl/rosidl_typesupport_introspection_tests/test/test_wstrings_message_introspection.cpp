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

template<typename WStringsMessageT>
class WStringsMessageIntrospectionTest
  : public MessageIntrospectionTest<WStringsMessageT>
{
};

using WStringsMessageTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__msg__WStrings,
  rosidl_typesupport_introspection_tests::msg::WStrings>;
TYPED_TEST_SUITE(WStringsMessageIntrospectionTest, WStringsMessageTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(WStringsMessageIntrospectionTest, MessageDescriptorIsCorrect)
{
  using WStringsMessageT = TypeParam;

  using TypeSupportLibraryT =
    typename introspection_traits<WStringsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();

  EXPECT_STREQ(
    get_message_namespace(message_descriptor),
    TypeSupportLibraryT::messages_namespace);
  EXPECT_STREQ(get_message_name(message_descriptor), "WStrings");
  EXPECT_EQ(get_message_size(message_descriptor), sizeof(WStringsMessageT));
  ASSERT_EQ(get_member_count(message_descriptor), 7u);

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 0u);
    EXPECT_STREQ(get_member_name(member_descriptor), "wstring_value");
    EXPECT_TRUE(is_wstring_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 1u);
    EXPECT_STREQ(get_member_name(member_descriptor), "wstring_value_default1");
    EXPECT_TRUE(is_wstring_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 2u);
    EXPECT_STREQ(get_member_name(member_descriptor), "wstring_value_default2");
    EXPECT_TRUE(is_wstring_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 3u);
    EXPECT_STREQ(get_member_name(member_descriptor), "wstring_value_default3");
    EXPECT_TRUE(is_wstring_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 4u);
    EXPECT_STREQ(get_member_name(member_descriptor), "array_of_wstrings");
    EXPECT_TRUE(is_wstring_member(member_descriptor));
    EXPECT_TRUE(has_array_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 5u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_sequence_of_wstrings");
    EXPECT_TRUE(is_wstring_member(member_descriptor));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 6u);
    EXPECT_STREQ(get_member_name(member_descriptor), "unbounded_sequence_of_wstrings");
    EXPECT_TRUE(is_wstring_member(member_descriptor));
    EXPECT_TRUE(has_sequence_structure(member_descriptor));
  }
}

TYPED_TEST(WStringsMessageIntrospectionTest, CanReadTypeErasedMessage)
{
  using WStringsMessageT = TypeParam;

  const auto message_ptr = Example<WStringsMessageT>::Make();
  const WStringsMessageT & message = *message_ptr;
  const void * type_erased_message = message_ptr.get();

  using TypeSupportLibraryT =
    typename introspection_traits<WStringsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 7u);

  EXPECT_MEMBER_EQ(
    type_erased_message, message, wstring_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, wstring_value_default1,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, wstring_value_default2,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, wstring_value_default3,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, array_of_wstrings,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, bounded_sequence_of_wstrings,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, unbounded_sequence_of_wstrings,
    get_member_descriptor(message_descriptor, 6u));
}

TYPED_TEST(WStringsMessageIntrospectionTest, CanWriteTypeErasedMessage)
{
  using WStringsMessageT = TypeParam;

  const auto message_ptr = Example<WStringsMessageT>::Make();
  const WStringsMessageT & message = *message_ptr;

  auto type_erased_message_copy = this->MakeTypeErasedMessage();
  const WStringsMessageT & message_copy =
    *reinterpret_cast<WStringsMessageT *>(type_erased_message_copy.get());
  EXPECT_NE(message, message_copy);

  using TypeSupportLibraryT =
    typename introspection_traits<WStringsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 7u);

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, wstring_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, wstring_value_default1,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, wstring_value_default2,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, wstring_value_default3,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_ARRAY_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, array_of_wstrings,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bounded_sequence_of_wstrings,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, unbounded_sequence_of_wstrings,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_EQ(message, message_copy);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
