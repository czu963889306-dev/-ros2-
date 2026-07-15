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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__MESSAGE_INTROSPECTION_TEST_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__MESSAGE_INTROSPECTION_TEST_HPP_

#include <memory>

#include "rosidl_typesupport_introspection_tests/gtest/shared_library_test.hpp"
#include "rosidl_typesupport_introspection_tests/api.hpp"
#include "rosidl_typesupport_introspection_tests/type_traits.hpp"
#include "rosidl_typesupport_introspection_tests/types.hpp"

namespace rosidl_typesupport_introspection_tests
{
namespace testing
{

/// A GTest fixture for message introspection tests
/**
 * \tparam MessageT type of the message to test.
 *   introspection_traits<MessageT> must exist.
 *   See test suite below for further reference
 *   on traits' requirements.
 */
template<typename MessageT>
class MessageIntrospectionTest : public SharedLibraryTest
{
public:
  using TypeSupportLibraryT =
    typename introspection_traits<MessageT>::TypeSupportLibraryT;
  using MessageDescriptorT =
    typename TypeSupportLibraryT::MessageDescriptorT;

  MessageIntrospectionTest()
  : SharedLibraryTest(TypeSupportLibraryT::name)
  {
  }

  void SetUp() override
  {
    const char * typesupport_symbol =
      introspection_traits<MessageT>::typesupport.symbol;
    auto message_typesupport_fetch =
      reinterpret_cast<MessageTypeSupportFetchFunctionT>(
      this->GetSharedLibrary().get_symbol(typesupport_symbol));
    ASSERT_NE(message_typesupport_fetch, nullptr);
    const rosidl_message_type_support_t * message_typesupport =
      get_message_typesupport_handle(
      message_typesupport_fetch(),
      TypeSupportLibraryT::identifier);
    ASSERT_NE(message_typesupport, nullptr);
    message_descriptor_ =
      reinterpret_cast<const MessageDescriptorT *>(
      message_typesupport->data);
    ASSERT_NE(message_descriptor_, nullptr);
  }

  const MessageDescriptorT * GetMessageDescriptor() const
  {
    return message_descriptor_;
  }

  std::unique_ptr<void, std::function<void(void *)>>
  MakeTypeErasedMessage() const
  {
    std::allocator<MessageT> allocator;
    std::function<void(void *)> type_erased_message_deleter = [ = ](void * ptr) mutable {
        finalize_message(ptr, message_descriptor_);
        allocator.deallocate(reinterpret_cast<MessageT *>(ptr), 1);
      };
    std::unique_ptr<void, std::function<void(void *)>> type_erased_message(
      initialize_message(allocator.allocate(1), message_descriptor_),
      type_erased_message_deleter);
    return type_erased_message;
  }

private:
  const MessageDescriptorT * message_descriptor_{nullptr};
};

}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__MESSAGE_INTROSPECTION_TEST_HPP_
