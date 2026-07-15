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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__API_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__API_HPP_

#include <rosidl_runtime_c/message_initialization.h>
#include <rosidl_typesupport_introspection_c/message_introspection.h>

#include <rcpputils/shared_library.hpp>
#include <rosidl_runtime_cpp/message_initialization.hpp>
#include <rosidl_typesupport_introspection_cpp/message_introspection.hpp>

#include "rosidl_typesupport_introspection_tests/type_traits.hpp"
#include "rosidl_typesupport_introspection_tests/types.hpp"

namespace rosidl_typesupport_introspection_tests
{


/// Initialize a type erased C++ `message` based on its `message_descriptor`.
inline void * initialize_message(
  void * message,
  const rosidl_typesupport_introspection_cpp::MessageMembers * message_descriptor)
{
  using rosidl_runtime_cpp::MessageInitialization;
  message_descriptor->init_function(message, MessageInitialization::ALL);
  return message;
}

/// Initialize a type erased C `message` based on its `message_descriptor`.
inline void * initialize_message(
  void * message,
  const rosidl_typesupport_introspection_c__MessageMembers * message_descriptor)
{
  message_descriptor->init_function(message, ROSIDL_RUNTIME_C_MSG_INIT_ALL);
  return message;
}

/// Finalize a type erased `message` based on its `message_descriptor`.
template<typename MessageDescriptorT>
void finalize_message(
  void * message,
  const MessageDescriptorT * message_descriptor)
{
  message_descriptor->fini_function(message);
}

/// Get a message's namespace from its `message_descriptor`.
template<typename MessageDescriptorT>
const char *
get_message_namespace(const MessageDescriptorT * message_descriptor)
{
  return message_descriptor->message_namespace_;
}

/// Get a message's name from its `message_descriptor`.
template<typename MessageDescriptorT>
const char * get_message_name(const MessageDescriptorT * message_descriptor)
{
  return message_descriptor->message_name_;
}

/// Get a service's namespace from its `service_descriptor`.
template<typename ServiceDescriptorT>
const char *
get_service_namespace(const ServiceDescriptorT * service_descriptor)
{
  return service_descriptor->service_namespace_;
}

/// Get a service's name from its `service_descriptor`.
template<typename ServiceDescriptorT>
const char * get_service_name(const ServiceDescriptorT * service_descriptor)
{
  return service_descriptor->service_name_;
}

/// Get a service request message descriptor from its `service_descriptor`.
template<typename ServiceDescriptorT>
auto get_service_request_descriptor(const ServiceDescriptorT * service_descriptor)
{
  return service_descriptor->request_members_;
}

/// Get a service response message descriptor from its `service_descriptor`.
template<typename ServiceDescriptorT>
auto get_service_response_descriptor(const ServiceDescriptorT * service_descriptor)
{
  return service_descriptor->response_members_;
}

/// Get a message's (base) size from its `message_descriptor`.
template<typename MessageDescriptorT>
size_t get_message_size(const MessageDescriptorT * message_descriptor)
{
  return message_descriptor->size_of_;
}

/// Get a message's member count from its `message_descriptor`.
template<typename MessageDescriptorT>
size_t get_member_count(const MessageDescriptorT * message_descriptor)
{
  return message_descriptor->member_count_;
}

/// Get a mutable type erased reference to a member
/// from a type erased `message` given its `member_descriptor`.
template<typename MemberDescriptorT>
void * get_member(void * message, const MemberDescriptorT * member_descriptor)
{
  return reinterpret_cast<uint8_t *>(message) +
         member_descriptor->offset_;
}

/// Get a mutable reference to a member from a
/// type erased `message` given its `member_descriptor`.
template<typename MemberT, typename MemberDescriptorT>
MemberT &
get_member(void * message, const MemberDescriptorT * member_descriptor)
{
  return *reinterpret_cast<MemberT *>(get_member(message, member_descriptor));
}

/// Get the member descriptor for the `i`th member
/// of a message given its `message_descriptor`.
template<typename MessageDescriptorT>
auto get_member_descriptor(
  const MessageDescriptorT * message_descriptor,
  const size_t i)
{
  return &(message_descriptor->members_[i]);
}

/// Check if a member has a simple structure
/// (i.e not an array nor a sequence) given its
/// `member_descriptor`.
template<typename MemberDescriptorT>
bool has_simple_structure(const MemberDescriptorT * member_descriptor)
{
  return !member_descriptor->is_array_ &&
         member_descriptor->size_function == nullptr &&
         member_descriptor->get_function == nullptr &&
         member_descriptor->get_const_function == nullptr &&
         member_descriptor->fetch_function == nullptr &&
         member_descriptor->assign_function == nullptr &&
         member_descriptor->resize_function == nullptr;
}

/// Check if a member has an iterable structure (i.e either an array or a
/// sequence) given its `member_descriptor`.
template<typename MemberDescriptorT>
bool has_iterable_structure(const MemberDescriptorT * member_descriptor)
{
  return member_descriptor->is_array_ &&
         member_descriptor->size_function != nullptr &&
         member_descriptor->fetch_function != nullptr &&
         member_descriptor->assign_function != nullptr;
}

/// Check if a member has an array structure given its `member_descriptor`.
template<typename MemberDescriptorT>
bool has_array_structure(const MemberDescriptorT * member_descriptor)
{
  return has_iterable_structure(member_descriptor) &&
         member_descriptor->get_function != nullptr &&
         member_descriptor->get_const_function != nullptr &&
         member_descriptor->resize_function == nullptr;
}

/// Check if a member has an array structure of `size` elements given its
/// `member_descriptor`.
template<typename MemberDescriptorT>
bool has_array_structure(
  const MemberDescriptorT * member_descriptor, const size_t size)
{
  return has_array_structure(member_descriptor) &&
         member_descriptor->array_size_ == size;
}

/// Check if a member has an unbounded sequence structure given its
/// `member_descriptor`.
template<typename MemberDescriptorT>
bool has_sequence_structure(const MemberDescriptorT * member_descriptor)
{
  return has_iterable_structure(member_descriptor) &&
         !member_descriptor->is_upper_bound_ &&
         member_descriptor->array_size_ == 0u &&
         member_descriptor->resize_function != nullptr;
}

/// Check if a member has a sequence structure bounded to `size`
/// elements given its `member_descriptor`.
template<typename MemberDescriptorT>
bool has_bounded_sequence_structure(
  const MemberDescriptorT * member_descriptor, const size_t size)
{
  return has_iterable_structure(member_descriptor) &&
         member_descriptor->is_upper_bound_ &&
         member_descriptor->array_size_ == size &&
         member_descriptor->resize_function != nullptr;
}

/// Check if a member is of `MessageT` type given its `member_descriptor`.
template<typename MessageT, typename MemberDescriptorT>
bool is_message_type_member(const MemberDescriptorT * member_descriptor)
{
  using TypeSupportLibraryT =
    typename introspection_traits<MessageT>::TypeSupportLibraryT;
  const auto & typesupport = introspection_traits<MessageT>::typesupport;
  rcpputils::SharedLibrary library{
    rcpputils::get_platform_library_name(TypeSupportLibraryT::name)};
  auto message_typesupport_fetch =
    reinterpret_cast<MessageTypeSupportFetchFunctionT>(
    library.get_symbol(typesupport.symbol));
  if (!message_typesupport_fetch) {
    return false;
  }
  const rosidl_message_type_support_t * message_typesupport =
    get_message_typesupport_handle(
    message_typesupport_fetch(),
    TypeSupportLibraryT::identifier);
  return member_descriptor->type_id_ == ROS_TYPE_MESSAGE &&
         member_descriptor->members_ == message_typesupport;
}

/// Check if a member is of `type_id` type given its `member_descriptor`.
template<typename MemberDescriptorT>
bool is_base_type_member(const MemberDescriptorT * member_descriptor, int type_id)
{
  return member_descriptor->type_id_ == type_id &&
         member_descriptor->members_ == nullptr;
}

/// Check if a member is of string type, optionally bounded to `upper_bound`
/// characters, given its `member_descriptor`.
template<typename MemberDescriptorT>
bool is_string_member(
  const MemberDescriptorT * member_descriptor,
  const size_t upper_bound = 0u)
{
  return member_descriptor->type_id_ == ROS_TYPE_STRING &&
         member_descriptor->members_ == nullptr &&
         member_descriptor->string_upper_bound_ == upper_bound;
}

/// Check if a member is of wstring type, optionally bounded to `upper_bound`
/// characters, given its `member_descriptor`.
template<typename MemberDescriptorT>
bool is_wstring_member(
  const MemberDescriptorT * member_descriptor,
  const size_t upper_bound = 0u)
{
  return member_descriptor->type_id_ == ROS_TYPE_WSTRING &&
         member_descriptor->members_ == nullptr &&
         member_descriptor->string_upper_bound_ == upper_bound;
}

/// Get an immutable type erased reference to a member
/// from a type erased message given its `member_descriptor`.
template<typename MemberDescriptorT>
const void * get_const_member(
  const void * message,
  const MemberDescriptorT * member_descriptor)
{
  return reinterpret_cast<const uint8_t *>(message) +
         member_descriptor->offset_;
}

/// Get an immutable reference to a member from a type
/// erased message given its `member_descriptor`.
template<typename MemberT, typename MemberDescriptorT>
const MemberT & get_const_member(
  const void * message,
  const MemberDescriptorT * member_descriptor)
{
  return *reinterpret_cast<const MemberT *>(
    get_const_member(message, member_descriptor));
}

/// Get a member's name from its `member_descriptor`.
template<typename MemberDescriptorT>
const char * get_member_name(const MemberDescriptorT * member_descriptor)
{
  return member_descriptor->name_;
}

/// Get a member's type id from its `member_descriptor`.
template<typename MemberDescriptorT>
int get_member_base_type(const MemberDescriptorT * member_descriptor)
{
  return member_descriptor->type_id_;
}

/// Check if a member supports direct memory access
/// (ie. get operations) given its `member_descriptor`.
template<typename MemberDescriptorT>
bool has_support_for_direct_memory_access(
  const MemberDescriptorT * member_descriptor)
{
  return member_descriptor->get_const_function != nullptr &&
         member_descriptor->get_function != nullptr;
}

/// Get a reference to the `i`th item of a type erased,
/// constant, iterable `member` given its `member_descriptor`.
template<typename ItemT, typename MemberDescriptorT>
const ItemT & get_member_item(
  const void * member,
  const MemberDescriptorT * member_descriptor,
  const size_t i)
{
  return *reinterpret_cast<const ItemT *>(
    member_descriptor->get_const_function(member, i));
}

/// Get a reference to the `i`th item of a type erased,
/// iterable `member` given its `member_descriptor`.
template<typename ItemT, typename MemberDescriptorT>
ItemT & get_member_item(
  void * member,
  const MemberDescriptorT * member_descriptor,
  const size_t i)
{
  return *reinterpret_cast<ItemT *>(
    member_descriptor->get_function(member, i));
}

/// Fetch the `i`th item value of a type erased, iterable
/// `member` given its `member_descriptor`.
template<typename ItemT, typename MemberDescriptorT>
ItemT fetch_member_item(
  const void * member,
  const MemberDescriptorT * member_descriptor,
  const size_t i)
{
  ItemT value;
  member_descriptor->fetch_function(member, i, &value);
  return value;
}

/// Assign a `value` to the `i`th item of a type erased,
/// iterable `member` given its `member_descriptor`.
template<typename ItemT, typename MemberDescriptorT>
void assign_member_item(
  void * member,
  const MemberDescriptorT * member_descriptor,
  const size_t i, ItemT && value)
{
  member_descriptor->assign_function(member, i, &value);
}

/// Get the size of an iterable, type erased `member`
/// given its `member_descriptor`.
template<typename MemberDescriptorT>
size_t get_member_size(
  const void * member,
  const MemberDescriptorT * member_descriptor)
{
  return member_descriptor->size_function(member);
}

/// Resize a sequence, type erased `member`
/// to `size` given its `member_descriptor`.
template<typename MemberDescriptorT>
void resize_member(
  void * member,
  const MemberDescriptorT * member_descriptor,
  const size_t size)
{
  member_descriptor->resize_function(member, size);
}

}  // namespace rosidl_typesupport_introspection_tests

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__API_HPP_
