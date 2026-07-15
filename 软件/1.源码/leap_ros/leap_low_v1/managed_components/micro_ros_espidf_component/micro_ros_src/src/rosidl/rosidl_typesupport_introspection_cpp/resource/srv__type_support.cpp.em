@# Included from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
@{
TEMPLATE(
    'msg__type_support.cpp.em',
    package_name=package_name, interface_path=interface_path, message=service.request_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support.cpp.em',
    package_name=package_name, interface_path=interface_path, message=service.response_message,
    include_directives=include_directives)
}@

@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore
include_parts = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_base = '/'.join(include_parts)

header_files = [
    'rosidl_runtime_c/service_type_support_struct.h',
    'rosidl_typesupport_cpp/message_type_support.hpp',
    'rosidl_typesupport_cpp/service_type_support.hpp',
    'rosidl_typesupport_interface/macros.h',
    'rosidl_typesupport_introspection_cpp/visibility_control.h',
    include_base + '__struct.hpp',
    'rosidl_typesupport_introspection_cpp/identifier.hpp',
    'rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp',
    'rosidl_typesupport_introspection_cpp/service_introspection.hpp',
    'rosidl_typesupport_introspection_cpp/service_type_support_decl.hpp',
]
}@
@[for header_file in header_files]@
@[    if header_file in include_directives]@
// already included above
// @
@[    else]@
@{include_directives.add(header_file)}@
@[    end if]@
#include "@(header_file)"
@[end for]@
@[for ns in service.namespaced_type.namespaces]@

namespace @(ns)
{
@[end for]@

namespace rosidl_typesupport_introspection_cpp
{

// this is intentionally not const to allow initialization later to prevent an initialization race
static ::rosidl_typesupport_introspection_cpp::ServiceMembers @(service.namespaced_type.name)_service_members = {
  "@('::'.join([package_name] + list(interface_path.parents[0].parts)))",  // service namespace
  "@(service.namespaced_type.name)",  // service name
  // these two fields are initialized below on the first access
  // see get_service_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [service.namespaced_type.name]))>()
  nullptr,  // request message
  nullptr  // response message
};

static const rosidl_service_type_support_t @(service.namespaced_type.name)_service_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &@(service.namespaced_type.name)_service_members,
  get_service_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_introspection_cpp
@[  for ns in reversed(service.namespaced_type.namespaces)]@

}  // namespace @(ns)
@[  end for]@


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [service.namespaced_type.name]))>()
{
  // get a handle to the value to be returned
  auto service_type_support =
    &::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::rosidl_typesupport_introspection_cpp::@(service.namespaced_type.name)_service_type_support_handle;
  // get a non-const and properly typed version of the data void *
  auto service_members = const_cast<::rosidl_typesupport_introspection_cpp::ServiceMembers *>(
    static_cast<const ::rosidl_typesupport_introspection_cpp::ServiceMembers *>(
      service_type_support->data));
  // make sure that both the request_members_ and the response_members_ are initialized
  // if they are not, initialize them
  if (
    service_members->request_members_ == nullptr ||
    service_members->response_members_ == nullptr)
  {
    // initialize the request_members_ with the static function from the external library
    service_members->request_members_ = static_cast<
      const ::rosidl_typesupport_introspection_cpp::MessageMembers *
      >(
      ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<
        ::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::@(service.request_message.structure.namespaced_type.name)
      >()->data
      );
    // initialize the response_members_ with the static function from the external library
    service_members->response_members_ = static_cast<
      const ::rosidl_typesupport_introspection_cpp::MessageMembers *
      >(
      ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<
        ::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::@(service.response_message.structure.namespaced_type.name)
      >()->data
      );
  }
  // finally return the properly initialized service_type_support handle
  return service_type_support;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [service.namespaced_type.name])))() {
  return ::rosidl_typesupport_introspection_cpp::get_service_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [service.namespaced_type.name]))>();
}

#ifdef __cplusplus
}
#endif
