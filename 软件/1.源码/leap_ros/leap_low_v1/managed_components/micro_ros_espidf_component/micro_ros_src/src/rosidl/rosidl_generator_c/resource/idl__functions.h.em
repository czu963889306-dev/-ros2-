// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from @(package_name):@(interface_path)
// generated code does not contain a copyright notice
@
@#######################################################################
@# EmPy template for generating <idl>__struct.h files
@#
@# Context:
@#  - package_name (string)
@#  - interface_path (Path relative to the directory named after the package)
@#  - content (IdlContent, list of elements, e.g. Messages or Services)
@#######################################################################
@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore
include_parts = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
header_guard_variable = '__'.join([x.upper() for x in include_parts]) + \
    '__FUNCTIONS_H_'
include_base = '/'.join(include_parts)
}@

#ifndef @(header_guard_variable)
#define @(header_guard_variable)

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "@(package_name)/msg/rosidl_generator_c__visibility_control.h"

#include "@(include_base)__struct.h"

@#######################################################################
@# Handle message
@#######################################################################
@{
from rosidl_parser.definition import Message
}@
@[for message in content.get_elements_of_type(Message)]@
@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=message)
}@

@[end for]@
@
@#######################################################################
@# Handle service
@#######################################################################
@{
from rosidl_parser.definition import Service
}@
@[for service in content.get_elements_of_type(Service)]@
@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=service.request_message)
}@

@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=service.response_message)
}@

@[end for]@
@
@#######################################################################
@# Handle action
@#######################################################################
@{
from rosidl_parser.definition import Action
}@
@[for action in content.get_elements_of_type(Action)]@
@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=action.goal)
}@

@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=action.result)
}@

@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback)
}@

@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=action.send_goal_service.request_message)
}@

@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=action.send_goal_service.response_message)
}@

@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=action.get_result_service.request_message)
}@

@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=action.get_result_service.response_message)
}@

@{
TEMPLATE(
    'msg__functions.h.em',
    package_name=package_name, interface_path=interface_path,
    message=action.feedback_message)
}@

@[end for]@
#ifdef __cplusplus
}
#endif

#endif  // @(header_guard_variable)
