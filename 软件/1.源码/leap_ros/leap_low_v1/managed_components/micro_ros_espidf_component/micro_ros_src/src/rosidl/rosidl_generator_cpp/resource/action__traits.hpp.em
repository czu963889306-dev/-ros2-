@# Included from rosidl_generator_cpp/resource/idl__traits.hpp.em
@{
from rosidl_parser.definition import ACTION_FEEDBACK_MESSAGE_SUFFIX
from rosidl_parser.definition import ACTION_FEEDBACK_SUFFIX
from rosidl_parser.definition import ACTION_GOAL_SERVICE_SUFFIX
from rosidl_parser.definition import ACTION_GOAL_SUFFIX
from rosidl_parser.definition import ACTION_RESULT_SERVICE_SUFFIX
from rosidl_parser.definition import ACTION_RESULT_SUFFIX
}@
@{
TEMPLATE(
    'msg__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=action.goal, include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=action.result, include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback, include_directives=include_directives)
}@

@{
TEMPLATE(
    'srv__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    service=action.send_goal_service, include_directives=include_directives)
}@

@{
TEMPLATE(
    'srv__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    service=action.get_result_service, include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback_message, include_directives=include_directives)
}@


@{
action_typename = '::'.join(action.namespaced_type.namespaced_name())
}@
@
namespace rosidl_generator_traits
{

template<>
struct is_action<@(action_typename)>
  : std::true_type
{
};

template<>
struct is_action_goal<@(action_typename)@(ACTION_GOAL_SUFFIX)>
  : std::true_type
{
};

template<>
struct is_action_result<@(action_typename)@(ACTION_RESULT_SUFFIX)>
  : std::true_type
{
};

template<>
struct is_action_feedback<@(action_typename)@(ACTION_FEEDBACK_SUFFIX)>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits

