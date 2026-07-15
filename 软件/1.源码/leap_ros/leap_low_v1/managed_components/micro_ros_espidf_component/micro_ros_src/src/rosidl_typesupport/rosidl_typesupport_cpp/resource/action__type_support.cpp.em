@# Included from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore
include_parts = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_base = '/'.join(include_parts)

header_files = (
    'action_msgs/msg/goal_status_array.hpp',
    'action_msgs/srv/cancel_goal.hpp',
    include_base + '__struct.hpp',
    'rosidl_typesupport_cpp/visibility_control.h',
    'rosidl_runtime_c/action_type_support_struct.h',
    'rosidl_typesupport_cpp/action_type_support.hpp',
    'rosidl_typesupport_cpp/message_type_support.hpp',
    'rosidl_typesupport_cpp/service_type_support.hpp',
)
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
@
@[  for ns in action.namespaced_type.namespaces]@

namespace @(ns)
{
@[  end for]@

namespace rosidl_typesupport_cpp
{

static rosidl_action_type_support_t @(interface_path.stem)_action_type_support_handle = {
  NULL, NULL, NULL, NULL, NULL};

}  // namespace rosidl_typesupport_cpp
@[  for ns in reversed(action.namespaced_type.namespaces)]@

}  // namespace @(ns)
@[  end for]@

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_action_type_support_t *
get_action_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts)))::@(interface_path.stem)>()
{
  using ::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::rosidl_typesupport_cpp::@(interface_path.stem)_action_type_support_handle;
  // Thread-safe by always writing the same values to the static struct
  @(interface_path.stem)_action_type_support_handle.goal_service_type_support = get_service_type_support_handle<::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::@(interface_path.stem)::Impl::SendGoalService>();
  @(interface_path.stem)_action_type_support_handle.result_service_type_support = get_service_type_support_handle<::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::@(interface_path.stem)::Impl::GetResultService>();
  @(interface_path.stem)_action_type_support_handle.cancel_service_type_support = get_service_type_support_handle<::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::@(interface_path.stem)::Impl::CancelGoalService>();
  @(interface_path.stem)_action_type_support_handle.feedback_message_type_support = get_message_type_support_handle<::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::@(interface_path.stem)::Impl::FeedbackMessage>();
  @(interface_path.stem)_action_type_support_handle.status_message_type_support = get_message_type_support_handle<::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::@(interface_path.stem)::Impl::GoalStatusMessage>();
  return &@(interface_path.stem)_action_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_action_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__ACTION_SYMBOL_NAME(rosidl_typesupport_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [interface_path.stem])))() {
  return ::rosidl_typesupport_cpp::get_action_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [interface_path.stem]))>();
}

#ifdef __cplusplus
}
#endif
