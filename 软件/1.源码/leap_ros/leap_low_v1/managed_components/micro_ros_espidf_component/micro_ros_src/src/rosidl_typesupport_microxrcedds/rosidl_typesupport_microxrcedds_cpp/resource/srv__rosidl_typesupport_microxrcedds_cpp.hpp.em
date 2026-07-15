@# Included from rosidl_typesupport_microxrcedds_cpp/resource/idl__rosidl_typesupport_microxrcedds_cpp.hpp.em
@{
TEMPLATE(
    'msg__rosidl_typesupport_microxrcedds_cpp.hpp.em',
    package_name=package_name,
    interface_path=interface_path,
    message=service.request_message,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__rosidl_typesupport_microxrcedds_cpp.hpp.em',
    package_name=package_name,
    interface_path=interface_path,
    message=service.response_message,
    include_directives=include_directives)
}@

@{
header_files = [
    'rmw/types.h',
    'rosidl_typesupport_cpp/service_type_support.hpp',
    'rosidl_typesupport_interface/macros.h',
    package_name + '/msg/rosidl_typesupport_microxrcedds_cpp__visibility_control.h',
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

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_@(package_name)
const rosidl_service_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [service.namespaced_type.name])))();

#ifdef __cplusplus
}
#endif
