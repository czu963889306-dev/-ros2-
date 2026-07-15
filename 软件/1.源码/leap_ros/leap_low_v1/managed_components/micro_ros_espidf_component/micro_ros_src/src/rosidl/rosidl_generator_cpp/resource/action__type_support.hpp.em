@# Included from rosidl_generator_cpp/resource/action__type_support.hpp.em
@{header_file = 'rosidl_typesupport_cpp/action_type_support.hpp'}@
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
const rosidl_action_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__ACTION_SYMBOL_NAME(
  rosidl_typesupport_cpp,
  @(',\n  '.join(action.namespaced_type.namespaced_name()))
)();
#ifdef __cplusplus
}
#endif

@{
TEMPLATE(
    'msg__type_support.hpp.em',
    package_name=package_name, message=action.goal,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support.hpp.em',
    package_name=package_name, message=action.result,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support.hpp.em',
    package_name=package_name, message=action.feedback,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'srv__type_support.hpp.em',
    package_name=package_name, service=action.send_goal_service,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'srv__type_support.hpp.em',
    package_name=package_name, service=action.get_result_service,
    include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__type_support.hpp.em',
    package_name=package_name, message=action.feedback_message,
    include_directives=include_directives)
}@
