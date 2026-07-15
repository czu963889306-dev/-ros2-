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

template<typename BoundedSequencesMessageT>
class BoundedSequencesMessageIntrospectionTest
  : public MessageIntrospectionTest<BoundedSequencesMessageT>
{
};

using BoundedSequencesMessageTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__msg__BoundedSequences,
  rosidl_typesupport_introspection_tests::msg::BoundedSequences>;
TYPED_TEST_SUITE(BoundedSequencesMessageIntrospectionTest, BoundedSequencesMessageTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(BoundedSequencesMessageIntrospectionTest, MessageDescriptorIsCorrect)
{
  using BoundedSequencesMessageT = TypeParam;

  using TypeSupportLibraryT =
    typename introspection_traits<BoundedSequencesMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();

  EXPECT_STREQ(
    get_message_namespace(message_descriptor),
    TypeSupportLibraryT::messages_namespace);
  EXPECT_STREQ(get_message_name(message_descriptor), "BoundedSequences");
  EXPECT_EQ(get_message_size(message_descriptor), sizeof(BoundedSequencesMessageT));
  ASSERT_EQ(get_member_count(message_descriptor), 32u);

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 0u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bool_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_BOOLEAN));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 1u);
    EXPECT_STREQ(get_member_name(member_descriptor), "byte_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_OCTET));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 2u);
    EXPECT_STREQ(get_member_name(member_descriptor), "char_values");
    // In ROS message definitions, char is an alias for uint8.
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 3u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float32_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_FLOAT));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 4u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float64_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_DOUBLE));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 5u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int8_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT8));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 6u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint8_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 7u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int16_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT16));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 8u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint16_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT16));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 9u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int32_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT32));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 10u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint32_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT32));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 11u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int64_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT64));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 12u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint64_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT64));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 13u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_values");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_STRING));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 14u);
    EXPECT_STREQ(get_member_name(member_descriptor), "basic_types_values");
    using basic_type =
      MEMBER_ITEM_TYPE(BoundedSequencesMessageT, basic_types_values);
    EXPECT_TRUE(is_message_type_member<basic_type>(member_descriptor));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 15u);
    EXPECT_STREQ(get_member_name(member_descriptor), "constants_values");
    using constants_type =
      MEMBER_ITEM_TYPE(BoundedSequencesMessageT, constants_values);
    EXPECT_TRUE(is_message_type_member<constants_type>(member_descriptor));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 16u);
    EXPECT_STREQ(get_member_name(member_descriptor), "defaults_values");
    using defaults_type =
      MEMBER_ITEM_TYPE(BoundedSequencesMessageT, defaults_values);
    EXPECT_TRUE(is_message_type_member<defaults_type>(member_descriptor));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 17u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bool_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_BOOLEAN));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 18u);
    EXPECT_STREQ(get_member_name(member_descriptor), "byte_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_OCTET));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 19u);
    EXPECT_STREQ(get_member_name(member_descriptor), "char_values_default");
    // In ROS message definitions, char is an alias for uint8.
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 20u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float32_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_FLOAT));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 21u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float64_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_DOUBLE));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 22u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int8_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT8));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 23u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint8_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 24u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int16_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT16));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 25u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint16_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT16));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 26u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int32_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT32));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 27u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint32_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT32));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 28u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int64_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT64));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 29u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint64_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT64));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 30u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_values_default");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_STRING));
    EXPECT_TRUE(has_bounded_sequence_structure(member_descriptor, 3u));
  }

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 31u);
    EXPECT_STREQ(get_member_name(member_descriptor), "alignment_check");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT32));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }
}

TYPED_TEST(BoundedSequencesMessageIntrospectionTest, CanReadTypeErasedMessage)
{
  using BoundedSequencesMessageT = TypeParam;

  const auto message_ptr = Example<BoundedSequencesMessageT>::Make();
  const BoundedSequencesMessageT & message = *message_ptr;
  const void * type_erased_message = message_ptr.get();

  using TypeSupportLibraryT =
    typename introspection_traits<BoundedSequencesMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 32u);

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, bool_values,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, byte_values,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, char_values,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, float32_values,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, float64_values,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, int8_values,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, uint8_values,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, int16_values,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, uint16_values,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, int32_values,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, uint32_values,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, int64_values,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, uint64_values,
    get_member_descriptor(message_descriptor, 12u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, string_values,
    get_member_descriptor(message_descriptor, 13u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, basic_types_values,
    get_member_descriptor(message_descriptor, 14u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, constants_values,
    get_member_descriptor(message_descriptor, 15u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, defaults_values,
    get_member_descriptor(message_descriptor, 16u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, bool_values_default,
    get_member_descriptor(message_descriptor, 17u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, byte_values_default,
    get_member_descriptor(message_descriptor, 18u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, char_values_default,
    get_member_descriptor(message_descriptor, 19u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, float32_values_default,
    get_member_descriptor(message_descriptor, 20u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, float64_values_default,
    get_member_descriptor(message_descriptor, 21u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, int8_values_default,
    get_member_descriptor(message_descriptor, 22u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, uint8_values_default,
    get_member_descriptor(message_descriptor, 23u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, int16_values_default,
    get_member_descriptor(message_descriptor, 24u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, uint16_values_default,
    get_member_descriptor(message_descriptor, 25u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, int32_values_default,
    get_member_descriptor(message_descriptor, 26u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, uint32_values_default,
    get_member_descriptor(message_descriptor, 27u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, int64_values_default,
    get_member_descriptor(message_descriptor, 28u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, uint64_values_default,
    get_member_descriptor(message_descriptor, 29u));

  EXPECT_ITERABLE_MEMBER_EQ(
    type_erased_message, message, string_values_default,
    get_member_descriptor(message_descriptor, 30u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, alignment_check,
    get_member_descriptor(message_descriptor, 31u));
}

TYPED_TEST(BoundedSequencesMessageIntrospectionTest, CanWriteTypeErasedMessage)
{
  using BoundedSequencesMessageT = TypeParam;

  const auto message_ptr = Example<BoundedSequencesMessageT>::Make();
  const BoundedSequencesMessageT & message = *message_ptr;

  auto type_erased_message_copy = this->MakeTypeErasedMessage();
  const BoundedSequencesMessageT & message_copy =
    *reinterpret_cast<BoundedSequencesMessageT *>(type_erased_message_copy.get());
  EXPECT_NE(message, message_copy);

  using TypeSupportLibraryT =
    typename introspection_traits<BoundedSequencesMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 32u);

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bool_values,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, byte_values,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, char_values,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float32_values,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float64_values,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int8_values,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint8_values,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int16_values,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint16_values,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int32_values,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint32_values,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int64_values,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint64_values,
    get_member_descriptor(message_descriptor, 12u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_values,
    get_member_descriptor(message_descriptor, 13u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, basic_types_values,
    get_member_descriptor(message_descriptor, 14u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, constants_values,
    get_member_descriptor(message_descriptor, 15u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, defaults_values,
    get_member_descriptor(message_descriptor, 16u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bool_values_default,
    get_member_descriptor(message_descriptor, 17u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, byte_values_default,
    get_member_descriptor(message_descriptor, 18u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, char_values_default,
    get_member_descriptor(message_descriptor, 19u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float32_values_default,
    get_member_descriptor(message_descriptor, 20u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float64_values_default,
    get_member_descriptor(message_descriptor, 21u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int8_values_default,
    get_member_descriptor(message_descriptor, 22u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint8_values_default,
    get_member_descriptor(message_descriptor, 23u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int16_values_default,
    get_member_descriptor(message_descriptor, 24u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint16_values_default,
    get_member_descriptor(message_descriptor, 25u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int32_values_default,
    get_member_descriptor(message_descriptor, 26u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint32_values_default,
    get_member_descriptor(message_descriptor, 27u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int64_values_default,
    get_member_descriptor(message_descriptor, 28u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint64_values_default,
    get_member_descriptor(message_descriptor, 29u));

  EXPECT_SEQUENCE_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_values_default,
    get_member_descriptor(message_descriptor, 30u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, alignment_check,
    get_member_descriptor(message_descriptor, 31u));

  EXPECT_EQ(message, message_copy);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
