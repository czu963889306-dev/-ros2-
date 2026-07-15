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

template<typename ConstantsMessageT>
class ConstantsMessageIntrospectionTest
  : public MessageIntrospectionTest<ConstantsMessageT>
{
};

using ConstantsMessageTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__msg__Constants,
  rosidl_typesupport_introspection_tests::msg::Constants>;
TYPED_TEST_SUITE(ConstantsMessageIntrospectionTest, ConstantsMessageTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(ConstantsMessageIntrospectionTest, MessageDescriptorIsCorrect)
{
  using ConstantsMessageT = TypeParam;
  using TypeSupportLibraryT =
    typename introspection_traits<ConstantsMessageT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;

  const MessageDescriptorT * message_descriptor = this->GetMessageDescriptor();
  EXPECT_STREQ(
    get_message_namespace(message_descriptor),
    TypeSupportLibraryT::messages_namespace);
  EXPECT_STREQ(get_message_name(message_descriptor), "Constants");
  EXPECT_EQ(get_message_size(message_descriptor), sizeof(ConstantsMessageT));
  EXPECT_EQ(get_member_count(message_descriptor), 1u);
}

TYPED_TEST(ConstantsMessageIntrospectionTest, CanConstructTypeErasedMessage)
{
  using ConstantsMessageT = TypeParam;
  auto type_erased_message = this->MakeTypeErasedMessage();
  const ConstantsMessageT & message =
    *reinterpret_cast<ConstantsMessageT *>(type_erased_message.get());
  EXPECT_EQ(message, message);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
