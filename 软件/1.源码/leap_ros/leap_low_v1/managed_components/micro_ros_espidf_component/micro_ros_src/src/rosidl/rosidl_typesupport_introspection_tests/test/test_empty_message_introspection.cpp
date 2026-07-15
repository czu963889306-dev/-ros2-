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

template<typename EmptyMessageT>
class EmptyMessageIntrospectionTest
  : public MessageIntrospectionTest<EmptyMessageT>
{
};

using EmptyMessageTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__msg__Empty,
  rosidl_typesupport_introspection_tests::msg::Empty>;
TYPED_TEST_SUITE(EmptyMessageIntrospectionTest, EmptyMessageTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(EmptyMessageIntrospectionTest, MessageDescriptorIsCorrect)
{
  using EmptyMessageT = TypeParam;
  using TypeSupportLibraryT =
    typename introspection_traits<EmptyMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;

  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  EXPECT_STREQ(
    get_message_namespace(message_descriptor),
    TypeSupportLibraryT::messages_namespace);
  EXPECT_STREQ(get_message_name(message_descriptor), "Empty");
  EXPECT_EQ(get_message_size(message_descriptor), sizeof(EmptyMessageT));
  EXPECT_EQ(get_member_count(message_descriptor), 1u);
}

TYPED_TEST(EmptyMessageIntrospectionTest, CanConstructTypeErasedMessage)
{
  using EmptyMessageT = TypeParam;
  auto type_erased_message = this->MakeTypeErasedMessage();
  const EmptyMessageT & message =
    *reinterpret_cast<EmptyMessageT *>(type_erased_message.get());
  EXPECT_EQ(message, message);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
