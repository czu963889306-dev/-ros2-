@# Included from rosidl_generator_cpp/resource/idl__builder.hpp.em
@{
TEMPLATE(
    'msg__builder.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=action.goal, include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__builder.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=action.result, include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__builder.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback, include_directives=include_directives)
}@

@{
TEMPLATE(
    'srv__builder.hpp.em',
    package_name=package_name, interface_path=interface_path,
    service=action.send_goal_service, include_directives=include_directives)
}@

@{
TEMPLATE(
    'srv__builder.hpp.em',
    package_name=package_name, interface_path=interface_path,
    service=action.get_result_service, include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__builder.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback_message, include_directives=include_directives)
}@
