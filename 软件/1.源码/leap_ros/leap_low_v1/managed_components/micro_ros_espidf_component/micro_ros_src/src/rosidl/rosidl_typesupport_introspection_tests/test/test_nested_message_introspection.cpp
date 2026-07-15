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

template<typename NestedMessageT>
class NestedMessageIntrospectionTest
  : public MessageIntrospectionTest<NestedMessageT>
{
};

using NestedMessageTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__msg__Nested,
  rosidl_typesupport_introspection_tests::msg::Nested>;
TYPED_TEST_SUITE(NestedMessageIntrospectionTest, NestedMessageTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(NestedMessageIntrospectionTest, MessageDescriptorIsCorrect)
{
  using NestedMessageT = TypeParam;

  using TypeSupportLibraryT =
    typename introspection_traits<NestedMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();

  EXPECT_STREQ(
    get_message_namespace(message_descriptor),
    TypeSupportLibraryT::messages_namespace);
  EXPECT_STREQ(get_message_name(message_descriptor), "Nested");
  EXPECT_EQ(get_message_size(message_descriptor), sizeof(NestedMessageT));
  ASSERT_EQ(get_member_count(message_descriptor), 1u);

  {
    auto * member_descriptor = get_member_descriptor(message_descriptor, 0u);
    EXPECT_STREQ(get_member_name(member_descriptor), "basic_types_value");
    using member_base_type =
      MEMBER_EXPRESSION_TYPE(NestedMessageT, basic_types_value);
    EXPECT_TRUE(is_message_type_member<member_base_type>(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }
}

TYPED_TEST(NestedMessageIntrospectionTest, CanReadTypeErasedMessage)
{
  using NestedMessageT = TypeParam;

  const auto message_ptr = Example<NestedMessageT>::Make();
  const NestedMessageT & message = *message_ptr;
  const void * type_erased_message = message_ptr.get();

  using TypeSupportLibraryT =
    typename introspection_traits<NestedMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 1u);

  EXPECT_MEMBER_EQ(
    type_erased_message, message, basic_types_value,
    get_member_descriptor(message_descriptor, 0u));
}

TYPED_TEST(NestedMessageIntrospectionTest, CanWriteTypeErasedMessage)
{
  using NestedMessageT = TypeParam;

  const auto message_ptr = Example<NestedMessageT>::Make();
  const NestedMessageT & message = *message_ptr;

  auto type_erased_message_copy = this->MakeTypeErasedMessage();
  const NestedMessageT & message_copy =
    *reinterpret_cast<NestedMessageT *>(type_erased_message_copy.get());
  EXPECT_NE(message, message_copy);

  using TypeSupportLibraryT =
    typename introspection_traits<NestedMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  ASSERT_EQ(get_member_count(message_descriptor), 1u);

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, basic_types_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_EQ(message, message_copy);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
