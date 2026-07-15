@# Included from rosidl_generator_cpp/resource/msg__type_support.hpp.em
@{header_file = 'rosidl_typesupport_cpp/message_type_support.hpp'}@
@[if header_file in include_directives]@
// already included above
// @
@[else]@
@{include_directives.add(header_file)}@
@[end if]@
#include "@(header_file)"

#ifdef __cplusplus
extern "C"
{
#endif
// Forward declare the get type support functions for this type.
ROSIDL_GENERATOR_CPP_PUBLIC_@(package_name)
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
  rosidl_typesupport_cpp,
  @(',\n  '.join(message.structure.namespaced_type.namespaced_name()))
)();
#ifdef __cplusplus
}
#endif
