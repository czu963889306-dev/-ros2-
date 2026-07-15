// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from @(package_name):@(interface_path)
// generated code does not contain a copyright notice
@
@#######################################################################
@# EmPy template for generating <idl>__traits.hpp files
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
include_base = '/'.join(include_parts)
header_guard_variable = '__'.join([x.upper() for x in include_parts]) + \
    '__TRAITS_HPP_'

include_directives = set()
}@

#ifndef @(header_guard_variable)
#define @(header_guard_variable)

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "@(include_base)__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

@#######################################################################
@# Handle message
@#######################################################################
@{
from rosidl_parser.definition import Message
}@
@[for message in content.get_elements_of_type(Message)]@
@{
TEMPLATE(
    'msg__traits.hpp.em',
    package_name=package_name, interface_path=interface_path, message=message,
    include_directives=include_directives)
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
    'srv__traits.hpp.em',
    package_name=package_name, interface_path=interface_path, service=service,
    include_directives=include_directives)
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
    'action__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    action=action, include_directives=include_directives)
}@

@[end for]@
#endif  // @(header_guard_variable)
