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

template<typename MultiNestedMessageT>
class MultiNestedMessageIntrospectionTest
  : public MessageIntrospectionTest<MultiNestedMessageT>
{
};

using MultiNestedMessageTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__msg__MultiNested,
  rosidl_typesupport_introspection_tests::msg::MultiNested>;
TYPED_TEST_SUITE(MultiNestedMessageIntrospectionTest, MultiNestedMessageTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(MultiNestedMessageIntrospectionTest, MessageDescriptorIsCorrect)
{
  using MultiNestedMessageT = TypeParam;

  using TypeSupportLibraryT =
    typename introspection_traits<MultiNestedMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();

  EXPECT_STREQ(
    get_message_namespace(message_descriptor),
    TypeSupportLibraryT::messages_namespace);
  EXPECT_STREQ(get_message_name(message_descriptor), "MultiNested");
  EXPECT_EQ(get_message_size(message_descriptor), sizeof(MultiNestedMessageT));
  ASSERT_EQ(get_member_count(message_descriptor), 9u);

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 0u);
    EXPECT_STREQ(get_member_name(member_descriptor), "array_of_arrays");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, array_of_arrays);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_array_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 1u);
    EXPECT_STREQ(get_member_name(member_descriptor), "array_of_bounded_sequences");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, array_of_bounded_sequences);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_array_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 2u);
    EXPECT_STREQ(get_member_name(member_descriptor), "array_of_unbounded_sequences");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, array_of_unbounded_sequences);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_array_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 3u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_sequence_of_arrays");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, bounded_sequence_of_arrays);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 4u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_sequence_of_bounded_sequences");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, bounded_sequence_of_bounded_sequences);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 5u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bounded_sequence_of_unbounded_sequences");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, bounded_sequence_of_unbounded_sequences);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 6u);
    EXPECT_STREQ(get_member_name(member_descriptor), "unbounded_sequence_of_arrays");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, unbounded_sequence_of_arrays);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_sequence_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 7u);
    EXPECT_STREQ(get_member_name(member_descriptor), "unbounded_sequence_of_bounded_sequences");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, unbounded_sequence_of_bounded_sequences);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_sequence_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 8u);
    EXPECT_STREQ(get_member_name(member_descriptor), "unbounded_sequence_of_unbounded_sequences");
    using member_base_type =
      MEMBER_ITEM_TYPE(MultiNestedMessageT, unbounded_sequence_of_unbounded_sequences);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_sequence_structure(member_descriptor));
  }
}

TYPED_TEST(MultiNestedMessageIntrospectionTest, CanReadTypeErasedMessage)
{
  using MultiNestedMessageT = TypeParam;

  const auto message_ptr = Example<MultiNestedMessageT>::Make();
  const MultiNestedMessageT & message = *message_ptr;
  const void * type_erased_message = message_ptr.get();

  using TypeSupportLibraryT =
    typename introspection_traits<MultiNestedMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 9u);

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, array_of_arrays,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, array_of_bounded_sequences,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, array_of_unbounded_sequences,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, bounded_sequence_of_arrays,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, bounded_sequence_of_bounded_sequences,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, bounded_sequence_of_unbounded_sequences,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, unbounded_sequence_of_arrays,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, unbounded_sequence_of_bounded_sequences,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, unbounded_sequence_of_unbounded_sequences,
    get_member_descriptor(message_descriptor, 8u));
}

TYPED_TEST(MultiNestedMessageIntrospectionTest, CanWriteTypeErasedMessage)
{
  using MultiNestedMessageT = TypeParam;

  const auto message_ptr = Example<MultiNestedMessageT>::Make();
  const MultiNestedMessageT & message = *message_ptr;

  auto type_erased_message_copy = this->MakeTypeErasedMessage();
  const MultiNestedMessageT & message_copy =
    *reinterpret_cast<MultiNestedMessageT *>(type_erased_message_copy.get());
  EXPECT_NE(message, message_copy);

  using TypeSupportLibraryT =
    typename introspection_traits<MultiNestedMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 9u);

  EXPECT_ARRAY_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, array_of_arrays,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_ARRAY_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message,
    array_of_bounded_sequences,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_ARRAY_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message,
    array_of_unbounded_sequences,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message,
    bounded_sequence_of_arrays,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message,
    bounded_sequence_of_bounded_sequences,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message,
    bounded_sequence_of_unbounded_sequences,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message,
    unbounded_sequence_of_arrays,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message,
    unbounded_sequence_of_bounded_sequences,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message,
    unbounded_sequence_of_unbounded_sequences,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_EQ(message, message_copy);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
