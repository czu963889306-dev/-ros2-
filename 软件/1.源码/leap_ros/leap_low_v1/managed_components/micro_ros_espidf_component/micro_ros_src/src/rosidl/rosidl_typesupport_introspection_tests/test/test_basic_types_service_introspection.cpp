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

template<typename BasicTypesServiceT>
class BasicTypesServiceIntrospectionTest
  : public ServiceIntrospectionTest<BasicTypesServiceT>
{
};

using BasicTypesServiceTypes = ::testing::Types<
  rosidl_typesupport_introspection_tests__srv__BasicTypes,
  rosidl_typesupport_introspection_tests::srv::BasicTypes>;
TYPED_TEST_SUITE(BasicTypesServiceIntrospectionTest, BasicTypesServiceTypes);

// NOTE(hidmic): cppcheck complains about gtest macros
// cppcheck-suppress syntaxError
TYPED_TEST(BasicTypesServiceIntrospectionTest, ServiceDescriptorIsCorrect)
{
  using BasicTypesServiceT = TypeParam;

  using TypeSupportLibraryT =
    typename introspection_traits<BasicTypesServiceT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  using ServiceDescriptorT = typename TypeSupportLibraryT::ServiceDescriptorT;
  const ServiceDescriptorT * service_descriptor = this->GetServiceDescriptor();

  EXPECT_STREQ(
    get_service_namespace(service_descriptor),
    TypeSupportLibraryT::services_namespace);
  EXPECT_STREQ(get_service_name(service_descriptor), "BasicTypes");

  const MessageDescriptorT * request_message_descriptor =
    get_service_request_descriptor(service_descriptor);
  using RequestMessageT = typename BasicTypesServiceT::Request;
  EXPECT_STREQ(
    get_message_namespace(request_message_descriptor),
    TypeSupportLibraryT::services_namespace);
  EXPECT_STREQ(get_message_name(request_message_descriptor), "BasicTypes_Request");
  EXPECT_EQ(get_message_size(request_message_descriptor), sizeof(RequestMessageT));
  ASSERT_EQ(get_member_count(request_message_descriptor), 14u);
  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 0u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bool_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_BOOLEAN));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 1u);
    EXPECT_STREQ(get_member_name(member_descriptor), "byte_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_OCTET));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 2u);
    EXPECT_STREQ(get_member_name(member_descriptor), "char_value");
    // In ROS message definitions, char is an alias for uint8.
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 3u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_FLOAT));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 4u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_DOUBLE));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 5u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int8_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 6u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint8_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 7u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int16_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT16));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 8u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint16_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT16));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 9u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT32));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 10u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT32));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 11u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT64));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 12u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT64));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(request_message_descriptor, 13u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_value");
    EXPECT_TRUE(is_string_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  const MessageDescriptorT * response_message_descriptor =
    get_service_response_descriptor(service_descriptor);
  using ResponseMessageT = typename BasicTypesServiceT::Response;
  EXPECT_STREQ(
    get_message_namespace(response_message_descriptor),
    TypeSupportLibraryT::services_namespace);
  EXPECT_STREQ(get_message_name(response_message_descriptor), "BasicTypes_Response");
  EXPECT_EQ(get_message_size(response_message_descriptor), sizeof(ResponseMessageT));
  ASSERT_EQ(get_member_count(response_message_descriptor), 14u);
  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 0u);
    EXPECT_STREQ(get_member_name(member_descriptor), "bool_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_BOOLEAN));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 1u);
    EXPECT_STREQ(get_member_name(member_descriptor), "byte_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_OCTET));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 2u);
    EXPECT_STREQ(get_member_name(member_descriptor), "char_value");
    // In ROS message definitions, char is an alias for uint8.
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 3u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_FLOAT));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 4u);
    EXPECT_STREQ(get_member_name(member_descriptor), "float64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_DOUBLE));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 5u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int8_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 6u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint8_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT8));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 7u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int16_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT16));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 8u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint16_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT16));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 9u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT32));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 10u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint32_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT32));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 11u);
    EXPECT_STREQ(get_member_name(member_descriptor), "int64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_INT64));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 12u);
    EXPECT_STREQ(get_member_name(member_descriptor), "uint64_value");
    EXPECT_TRUE(is_base_type_member(member_descriptor, ROS_TYPE_UINT64));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }

  {
    auto * member_descriptor = get_member_descriptor(response_message_descriptor, 13u);
    EXPECT_STREQ(get_member_name(member_descriptor), "string_value");
    EXPECT_TRUE(is_string_member(member_descriptor));
    EXPECT_TRUE(has_simple_structure(member_descriptor));
  }
}

TYPED_TEST(BasicTypesServiceIntrospectionTest, CanReadTypeErasedRequestMessage)
{
  using BasicTypesServiceT = TypeParam;
  using RequestMessageT = typename BasicTypesServiceT::Request;

  const auto message_ptr = Example<BasicTypesServiceT>::MakeRequest();
  const RequestMessageT & message = *message_ptr;
  const void * type_erased_message = message_ptr.get();

  using TypeSupportLibraryT =
    typename introspection_traits<BasicTypesServiceT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor =
    get_service_request_descriptor(this->GetServiceDescriptor());
  ASSERT_EQ(get_member_count(message_descriptor), 14u);

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bool_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, byte_value,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, char_value,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, float32_value,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, float64_value,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int8_value,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint8_value,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int16_value,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint16_value,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int32_value,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint32_value,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int64_value,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint64_value,
    get_member_descriptor(message_descriptor, 12u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, string_value,
    get_member_descriptor(message_descriptor, 13u));
}

TYPED_TEST(BasicTypesServiceIntrospectionTest, CanReadTypeErasedResponseMessage)
{
  using BasicTypesServiceT = TypeParam;
  using ResponseMessageT = typename BasicTypesServiceT::Response;

  const auto message_ptr = Example<BasicTypesServiceT>::MakeResponse();
  const ResponseMessageT & message = *message_ptr;
  const void * type_erased_message = message_ptr.get();

  using TypeSupportLibraryT =
    typename introspection_traits<BasicTypesServiceT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor =
    get_service_response_descriptor(this->GetServiceDescriptor());
  ASSERT_EQ(get_member_count(message_descriptor), 14u);

  EXPECT_MEMBER_EQ(
    type_erased_message, message, bool_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, byte_value,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, char_value,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, float32_value,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, float64_value,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int8_value,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint8_value,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int16_value,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint16_value,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int32_value,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint32_value,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, int64_value,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, uint64_value,
    get_member_descriptor(message_descriptor, 12u));

  EXPECT_MEMBER_EQ(
    type_erased_message, message, string_value,
    get_member_descriptor(message_descriptor, 13u));
}

TYPED_TEST(BasicTypesServiceIntrospectionTest, CanWriteTypeErasedRequestMessage)
{
  using BasicTypesServiceT = TypeParam;
  using RequestMessageT = typename BasicTypesServiceT::Request;

  const auto message_ptr = Example<BasicTypesServiceT>::MakeRequest();
  const RequestMessageT & message = *message_ptr;

  auto type_erased_message_copy = this->MakeTypeErasedRequestMessage();
  const auto & message_copy =
    *reinterpret_cast<RequestMessageT *>(type_erased_message_copy.get());
  EXPECT_NE(message, message_copy);

  using TypeSupportLibraryT =
    typename introspection_traits<BasicTypesServiceT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor =
    get_service_request_descriptor(this->GetServiceDescriptor());
  ASSERT_EQ(get_member_count(message_descriptor), 14u);

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bool_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, byte_value,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, char_value,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float32_value,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float64_value,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int8_value,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint8_value,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int16_value,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint16_value,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int32_value,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint32_value,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int64_value,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint64_value,
    get_member_descriptor(message_descriptor, 12u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_value,
    get_member_descriptor(message_descriptor, 13u));

  EXPECT_EQ(message, message_copy);
}

TYPED_TEST(BasicTypesServiceIntrospectionTest, CanWriteTypeErasedResponseMessage)
{
  using BasicTypesServiceT = TypeParam;
  using ResponseMessageT = typename BasicTypesServiceT::Response;

  const auto message_ptr = Example<BasicTypesServiceT>::MakeResponse();
  const ResponseMessageT & message = *message_ptr;

  auto type_erased_message_copy = this->MakeTypeErasedResponseMessage();
  const auto & message_copy =
    *reinterpret_cast<ResponseMessageT *>(type_erased_message_copy.get());
  EXPECT_NE(message, message_copy);

  using TypeSupportLibraryT =
    typename introspection_traits<BasicTypesServiceT>::TypeSupportLibraryT;
  using MessageDescriptorT = typename TypeSupportLibraryT::MessageDescriptorT;
  const MessageDescriptorT * message_descriptor =
    get_service_response_descriptor(this->GetServiceDescriptor());
  ASSERT_EQ(get_member_count(message_descriptor), 14u);

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, bool_value,
    get_member_descriptor(message_descriptor, 0u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, byte_value,
    get_member_descriptor(message_descriptor, 1u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, char_value,
    get_member_descriptor(message_descriptor, 2u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float32_value,
    get_member_descriptor(message_descriptor, 3u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, float64_value,
    get_member_descriptor(message_descriptor, 4u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int8_value,
    get_member_descriptor(message_descriptor, 5u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint8_value,
    get_member_descriptor(message_descriptor, 6u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int16_value,
    get_member_descriptor(message_descriptor, 7u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint16_value,
    get_member_descriptor(message_descriptor, 8u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int32_value,
    get_member_descriptor(message_descriptor, 9u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint32_value,
    get_member_descriptor(message_descriptor, 10u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, int64_value,
    get_member_descriptor(message_descriptor, 11u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, uint64_value,
    get_member_descriptor(message_descriptor, 12u));

  EXPECT_MEMBER_ASSIGNMENT(
    type_erased_message_copy.get(), message, string_value,
    get_member_descriptor(message_descriptor, 13u));

  EXPECT_EQ(message, message_copy);
}

}  // namespace
}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests
