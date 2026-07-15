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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__SERVICE_INTROSPECTION_TEST_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__SERVICE_INTROSPECTION_TEST_HPP_

#include <memory>

#include "rosidl_typesupport_introspection_tests/gtest/shared_library_test.hpp"
#include "rosidl_typesupport_introspection_tests/api.hpp"
#include "rosidl_typesupport_introspection_tests/type_traits.hpp"
#include "rosidl_typesupport_introspection_tests/types.hpp"

namespace rosidl_typesupport_introspection_tests
{
namespace testing
{

/// A GTest fixture for service introspection tests
/**
 * \tparam ServiceT type of the service to test.
 *   introspection_traits<ServiceT> must exist.
 *   See test suite below for further reference
 *   on traits' requirements.
 */
template<typename ServiceT>
class ServiceIntrospectionTest : public SharedLibraryTest
{
public:
  using TypeSupportLibraryT =
    typename introspection_traits<ServiceT>::TypeSupportLibraryT;
  using ServiceDescriptorT =
    typename TypeSupportLibraryT::ServiceDescriptorT;

  ServiceIntrospectionTest()
  : SharedLibraryTest(TypeSupportLibraryT::name)
  {
  }

  void SetUp() override
  {
    const char * typesupport_symbol =
      introspection_traits<ServiceT>::typesupport.symbol;
    auto service_typesupport_fetch =
      reinterpret_cast<ServiceTypeSupportFetchFunctionT>(
      this->GetSharedLibrary().get_symbol(typesupport_symbol));
    ASSERT_NE(service_typesupport_fetch, nullptr);
    const rosidl_service_type_support_t * service_typesupport =
      get_service_typesupport_handle(
      service_typesupport_fetch(),
      TypeSupportLibraryT::identifier);
    ASSERT_NE(service_typesupport, nullptr);
    service_descriptor_ =
      reinterpret_cast<const ServiceDescriptorT *>(
      service_typesupport->data);
    ASSERT_NE(service_descriptor_, nullptr);
  }

  const ServiceDescriptorT * GetServiceDescriptor() const
  {
    return service_descriptor_;
  }

  std::unique_ptr<void, std::function<void(void *)>>
  MakeTypeErasedRequestMessage() const
  {
    using MessageDescriptorT =
      typename TypeSupportLibraryT::MessageDescriptorT;
    const MessageDescriptorT * request_message_descriptor =
      get_service_request_descriptor(service_descriptor_);
    using RequestMessageT = typename ServiceT::Request;
    std::allocator<RequestMessageT> allocator;
    std::function<void(void *)> type_erased_message_deleter = [ = ](void * ptr) mutable {
        finalize_message(ptr, request_message_descriptor);
        allocator.deallocate(reinterpret_cast<RequestMessageT *>(ptr), 1);
      };
    std::unique_ptr<void, std::function<void(void *)>> type_erased_message(
      initialize_message(allocator.allocate(1), request_message_descriptor),
      type_erased_message_deleter);
    return type_erased_message;
  }

  std::unique_ptr<void, std::function<void(void *)>>
  MakeTypeErasedResponseMessage() const
  {
    using MessageDescriptorT =
      typename TypeSupportLibraryT::MessageDescriptorT;
    const MessageDescriptorT * response_message_descriptor =
      get_service_response_descriptor(service_descriptor_);
    using ResponseMessageT = typename ServiceT::Response;
    std::allocator<ResponseMessageT> allocator;
    std::function<void(void *)> type_erased_message_deleter = [ = ](void * ptr) mutable {
        finalize_message(ptr, response_message_descriptor);
        allocator.deallocate(reinterpret_cast<ResponseMessageT *>(ptr), 1);
      };
    std::unique_ptr<void, std::function<void(void *)>> type_erased_message(
      initialize_message(allocator.allocate(1), response_message_descriptor),
      type_erased_message_deleter);
    return type_erased_message;
  }

private:
  const ServiceDescriptorT * service_descriptor_{nullptr};
};

}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__SERVICE_INTROSPECTION_TEST_HPP_
