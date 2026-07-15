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
#include "rosidl_typesupport_introspection_tests/gtest/service_introspection_test.hpp"
#include "rosidl_typesupport_introspection_tests/api.hpp"
#include "rosidl_typesupport_introspection_tests/type_traits.hpp"

#include "introspection_libraries_under_test.hpp"

namespace rosidl_typesupport_introspection_tests
{
namespace testing
{
namespace
{

template<typename EmptyServiceT>
class EmptyServiceIntrospectionTest
  : public ServiceIntrospectionTest<EmptyServiceT>
{
};

using EmptyServiceTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__srv__Empty,
  rosidl_typesupport_introspection_tests::srv::Empty>;
TYPED_TEST_SUITE(EmptyServiceIntrospectionTest, EmptyServiceTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(EmptyServiceIntrospectionTest, ServiceDescriptorIsCorrect)
{
  using EmptyServiceT = TypeParam;
  using RequestMessageT = typename EmptyServiceT::Request;
  using ResponseMessageT = typename EmptyServiceT::Response;

  using TypeSupportLibraryT =
    typename introspection_traits<EmptyServiceT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  using ServiceDescriptorT = typename TypeSupportLibraryT::ServiceDescriptorT;
  const ServiceDescriptorT * service_descriptor = this->GetServiceDescriptor();

  EXPECT_STREQ(
    get_service_namespace(service_descriptor),
    TypeSupportLibraryT::services_namespace);
  EXPECT_STREQ(get_service_name(service_descriptor), "Empty");

  const MessageDescriptorT * request_message_descriptor =
    get_service_request_descriptor(service_descriptor);
  EXPECT_STREQ(
    get_message_namespace(request_message_descriptor),
    TypeSupportLibraryT::services_namespace);
  EXPECT_STREQ(get_message_name(request_message_descriptor), "Empty_Request");
  EXPECT_EQ(
    get_message_size(request_message_descriptor),
    sizeof(RequestMessageT));
  const MessageDescriptorT * response_message_descriptor =
    get_service_response_descriptor(service_descriptor);
  EXPECT_STREQ(
    get_message_namespace(response_message_descriptor),
    TypeSupportLibraryT::services_namespace);
  EXPECT_STREQ(get_message_name(response_message_descriptor), "Empty_Response");
  EXPECT_EQ(
    get_message_size(response_message_descriptor),
    sizeof(ResponseMessageT));
}

TYPED_TEST(EmptyServiceIntrospectionTest, CanConstructTypeErasedRequestMessage)
{
  using EmptyServiceT = TypeParam;
  using RequestMessageT = typename EmptyServiceT::Request;

  auto type_erased_request_message =
    this->MakeTypeErasedRequestMessage();
  const RequestMessageT & request_message =
    *reinterpret_cast<RequestMessageT *>(
    type_erased_request_message.get());
  EXPECT_EQ(request_message, request_message);
}

TYPED_TEST(EmptyServiceIntrospectionTest, CanConstructTypeErasedResponseMessage)
{
  using EmptyServiceT = TypeParam;
  using ResponseMessageT = typename EmptyServiceT::Response;

  auto type_erased_response_message =
    this->MakeTypeErasedResponseMessage();
  const ResponseMessageT & response_message =
    *reinterpret_cast<ResponseMessageT *>(
    type_erased_response_message.get());
  EXPECT_EQ(response_message, response_message);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
