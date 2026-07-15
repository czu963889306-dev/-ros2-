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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__TYPESUPPORT_LIBRARY_TEST_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__TYPESUPPORT_LIBRARY_TEST_HPP_

#include <gtest/gtest.h>

#include <string>

#include "rosidl_typesupport_introspection_tests/gtest/shared_library_test.hpp"
#include "rosidl_typesupport_introspection_tests/type_traits.hpp"
#include "rosidl_typesupport_introspection_tests/types.hpp"

namespace rosidl_typesupport_introspection_tests
{
namespace testing
{

/// A GTest fixture for typesupport library tests
/**
 * \tparam TypeSupportLibraryT a compile-time library definition.
 *   See type parameterized test suite below for further reference
 *   on definition requirements.
 */
template<typename TypeSupportLibraryT>
class TypeSupportLibraryTest : public SharedLibraryTest
{
public:
  TypeSupportLibraryTest()
  : SharedLibraryTest(TypeSupportLibraryT::name)
  {
  }
};


// A type parameterized test suite to validate typesupport libraries.
TYPED_TEST_SUITE_P(TypeSupportLibraryTest);

DEFINE_HAS_MEMBER_TRAIT(messages);  // I.e has_messages<T>

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST_P(TypeSupportLibraryTest, MessageTypeSupportSymbolsAreCorrect) {
  using TypeSupportLibraryT = TypeParam;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;

  if constexpr (has_messages<TypeSupportLibraryT>::value) {
    auto & library = this->GetSharedLibrary();
    for (const auto & message : TypeSupportLibraryT::messages) {
      ASSERT_TRUE(library.has_symbol(message.symbol))
        << message.symbol << " is missing from "
        << library.get_library_path();
      auto message_typesupport_fetch =
        reinterpret_cast<MessageTypeSupportFetchFunctionT>(
        library.get_symbol(message.symbol));
      ASSERT_NE(message_typesupport_fetch, nullptr);
      const rosidl_message_type_support_t * message_typesupport =
        get_message_typesupport_handle(
        message_typesupport_fetch(),
        TypeSupportLibraryT::identifier);
      ASSERT_NE(message_typesupport, nullptr);
      auto message_descriptor =
        reinterpret_cast<const MessageDescriptorT *>(message_typesupport->data);
      ASSERT_NE(message_descriptor, nullptr);
      ASSERT_STREQ(
        message_descriptor->message_namespace_,
        TypeSupportLibraryT::messages_namespace);
    }
  } else {
    GTEST_SKIP() << "No message typesupport symbols";
  }
}

DEFINE_HAS_MEMBER_TRAIT(services);  // I.e has_services<T>

TYPED_TEST_P(TypeSupportLibraryTest, ServiceTypeSupportSymbolsAreCorrect)
{
  using TypeSupportLibraryT = TypeParam;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  using ServiceDescriptorT = typename TypeSupportLibraryT::ServiceDescriptorT;

  if constexpr (has_services<TypeSupportLibraryT>::value) {
    auto & library = this->GetSharedLibrary();
    for (const auto & service : TypeSupportLibraryT::services) {
      ASSERT_TRUE(library.has_symbol(service.symbol))
        << service.symbol << " is missing from "
        << library.get_library_path();
      auto service_typesupport_fetch =
        reinterpret_cast<ServiceTypeSupportFetchFunctionT>(
        library.get_symbol(service.symbol));
      ASSERT_NE(service_typesupport_fetch, nullptr);
      const rosidl_service_type_support_t * service_typesupport =
        get_service_typesupport_handle(
        service_typesupport_fetch(),
        TypeSupportLibraryT::identifier);
      ASSERT_NE(service_typesupport, nullptr);
      auto service_descriptor =
        reinterpret_cast<const ServiceDescriptorT *>(service_typesupport->data);
      ASSERT_NE(service_descriptor, nullptr);
      ASSERT_STREQ(
        service_descriptor->service_namespace_,
        TypeSupportLibraryT::services_namespace);

      ASSERT_TRUE(library.has_symbol(service.request.symbol))
        << service.request.symbol << " is missing from "
        << library.get_library_path();
      auto service_request_typesupport_fetch =
        reinterpret_cast<MessageTypeSupportFetchFunctionT>(
        library.get_symbol(service.request.symbol));
      ASSERT_NE(service_request_typesupport_fetch, nullptr);
      const rosidl_message_type_support_t * service_request_typesupport =
        get_message_typesupport_handle(
        service_request_typesupport_fetch(),
        TypeSupportLibraryT::identifier);
      ASSERT_NE(service_request_typesupport, nullptr);
      auto service_request_descriptor =
        reinterpret_cast<const MessageDescriptorT *>(
        service_request_typesupport->data);
      ASSERT_NE(service_request_descriptor, nullptr);
      ASSERT_EQ(
        service_descriptor->request_members_,
        service_request_descriptor);
      ASSERT_STREQ(
        service_request_descriptor->message_namespace_,
        TypeSupportLibraryT::services_namespace);

      ASSERT_TRUE(library.has_symbol(service.response.symbol))
        << service.response.symbol << " is missing from "
        << library.get_library_path();
      auto service_response_typesupport_fetch =
        reinterpret_cast<MessageTypeSupportFetchFunctionT>(
        library.get_symbol(service.response.symbol));
      ASSERT_NE(service_response_typesupport_fetch, nullptr);
      const rosidl_message_type_support_t * service_response_typesupport =
        get_message_typesupport_handle(
        service_response_typesupport_fetch(),
        TypeSupportLibraryT::identifier);
      ASSERT_NE(service_response_typesupport, nullptr);
      auto service_response_descriptor =
        reinterpret_cast<const MessageDescriptorT *>(
        service_response_typesupport->data);
      ASSERT_NE(service_response_descriptor, nullptr);
      ASSERT_EQ(
        service_descriptor->response_members_,
        service_response_descriptor);
      ASSERT_STREQ(
        service_response_descriptor->message_namespace_,
        TypeSupportLibraryT::services_namespace);
    }
  } else {
    GTEST_SKIP() << "No service typesupport symbols";
  }
}

DEFINE_HAS_MEMBER_TRAIT(actions);  // I.e has_actions<T>

TYPED_TEST_P(TypeSupportLibraryTest, ActionTypeSupportSymbolsAreCorrect) {
  using TypeSupportLibraryT = TypeParam;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  using ServiceDescriptorT = typename TypeSupportLibraryT::ServiceDescriptorT;

  if constexpr (has_actions<TypeSupportLibraryT>::value) {
    auto & library = this->GetSharedLibrary();
    for (const auto & action : TypeSupportLibraryT::actions) {
      // NOTE(hidmic): action.symbol doesn't exist, should it?
      const auto action_message_symbol_table = {
        action.feedback, action.feedback_message, action.result, action.goal};
      for (const auto & message : action_message_symbol_table) {
        ASSERT_TRUE(library.has_symbol(message.symbol))
          << message.symbol << " is missing from "
          << library.get_library_path();
        auto message_typesupport_fetch =
          reinterpret_cast<MessageTypeSupportFetchFunctionT>(
          this->GetSharedLibrary().get_symbol(message.symbol));
        ASSERT_NE(message_typesupport_fetch, nullptr);
        const rosidl_message_type_support_t * message_typesupport =
          get_message_typesupport_handle(
          message_typesupport_fetch(),
          TypeSupportLibraryT::identifier);
        ASSERT_NE(message_typesupport, nullptr);
        auto message_descriptor =
          reinterpret_cast<const MessageDescriptorT *>(
          message_typesupport->data);
        ASSERT_NE(message_descriptor, nullptr);
        ASSERT_STREQ(
          message_descriptor->message_namespace_,
          TypeSupportLibraryT::actions_namespace);
      }
      for (const auto & service : {action.get_result, action.send_goal}) {
        ASSERT_TRUE(library.has_symbol(service.symbol))
          << service.symbol << " is missing from "
          << library.get_library_path();
        auto service_typesupport_fetch =
          reinterpret_cast<ServiceTypeSupportFetchFunctionT>(
          this->GetSharedLibrary().get_symbol(service.symbol));
        ASSERT_NE(service_typesupport_fetch, nullptr);
        const rosidl_service_type_support_t * service_typesupport =
          get_service_typesupport_handle(
          service_typesupport_fetch(),
          TypeSupportLibraryT::identifier);
        ASSERT_NE(service_typesupport, nullptr);
        auto service_descriptor =
          reinterpret_cast<const ServiceDescriptorT *>(
          service_typesupport->data);
        ASSERT_NE(service_descriptor, nullptr);
        ASSERT_STREQ(
          service_descriptor->service_namespace_,
          TypeSupportLibraryT::actions_namespace);

        ASSERT_TRUE(library.has_symbol(service.request.symbol))
          << service.request.symbol << " is missing from "
          << library.get_library_path();
        auto service_request_typesupport_fetch =
          reinterpret_cast<MessageTypeSupportFetchFunctionT>(
          library.get_symbol(service.request.symbol));
        ASSERT_NE(service_request_typesupport_fetch, nullptr);
        const rosidl_message_type_support_t * service_request_typesupport =
          get_message_typesupport_handle(
          service_request_typesupport_fetch(),
          TypeSupportLibraryT::identifier);
        ASSERT_NE(service_request_typesupport, nullptr);
        auto service_request_descriptor =
          reinterpret_cast<const MessageDescriptorT *>(
          service_request_typesupport->data);
        ASSERT_NE(service_request_descriptor, nullptr);
        ASSERT_EQ(service_descriptor->request_members_, service_request_descriptor);
        ASSERT_STREQ(
          service_request_descriptor->message_namespace_,
          TypeSupportLibraryT::actions_namespace);

        ASSERT_TRUE(library.has_symbol(service.response.symbol))
          << service.response.symbol << " is missing from "
          << library.get_library_path();
        auto service_response_typesupport_fetch =
          reinterpret_cast<MessageTypeSupportFetchFunctionT>(
          library.get_symbol(service.response.symbol));
        ASSERT_NE(service_response_typesupport_fetch, nullptr);
        const rosidl_message_type_support_t * service_response_typesupport =
          get_message_typesupport_handle(
          service_response_typesupport_fetch(),
          TypeSupportLibraryT::identifier);
        ASSERT_NE(service_response_typesupport, nullptr);
        auto service_response_descriptor =
          reinterpret_cast<const MessageDescriptorT *>(
          service_response_typesupport->data);
        ASSERT_NE(service_response_descriptor, nullptr);
        ASSERT_EQ(
          service_descriptor->response_members_,
          service_response_descriptor);
        ASSERT_STREQ(
          service_response_descriptor->message_namespace_,
          TypeSupportLibraryT::actions_namespace);
      }
    }
  } else {
    GTEST_SKIP() << "No service typesupport symbols";
  }
}

REGISTER_TYPED_TEST_SUITE_P(
  TypeSupportLibraryTest,
  MessageTypeSupportSymbolsAreCorrect,
  ServiceTypeSupportSymbolsAreCorrect,
  ActionTypeSupportSymbolsAreCorrect);

}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__TYPESUPPORT_LIBRARY_TEST_HPP_
