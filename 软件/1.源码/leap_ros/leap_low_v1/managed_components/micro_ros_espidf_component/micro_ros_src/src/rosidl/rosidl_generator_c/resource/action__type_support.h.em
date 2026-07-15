@# Included from rosidl_generator_c/resource/idl__type_support.h.em
@{header_file = 'rosidl_runtime_c/action_type_support_struct.h'}@
@[if header_file in include_directives]@
// already included above
// @
@[else]@
@{include_directives.add(header_file)}@
@[end if]@
#include "@(header_file)"

// Forward declare the get type support functions for this type.
ROSIDL_GENERATOR_C_PUBLIC_@(package_name)
const rosidl_action_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__ACTION_SYMBOL_NAME(
  rosidl_typesupport_c,
  @(',\n  '.join(action.namespaced_type.namespaced_name()))
)();

@{
TEMPLATE(
    'msg__type_support.h.em',
    package_name=package_name, message=action.goal,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support.h.em',
    package_name=package_name, message=action.result,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support.h.em',
    package_name=package_name, message=action.feedback,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'srv__type_support.h.em',
    package_name=package_name, service=action.send_goal_service,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'srv__type_support.h.em',
    package_name=package_name, service=action.get_result_service,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support.h.em',
    package_name=package_name, message=action.feedback_message,
    include_directives=include_directives)
}@
