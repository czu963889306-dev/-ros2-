// generated from rosidl_generator_c/resource/idl.h.em
// with input from @(package_name):@(interface_path)
// generated code does not contain a copyright notice

@#######################################################################
@# EmPy template for generating <idl>.h files
@#
@# Context:
@#  - package_name (string)
@#  - interface_path (Path relative to the directory named after the package)
@#  - content (IdlContent, list of elements, e.g. Messages or Services)
@#######################################################################
@
@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore
include_parts = [package_name] + list(interface_path.parents[0].parts) + \
    [convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_parts_detail = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
header_guard_variable = '__'.join([x.upper() for x in include_parts]) + '_H_'
include_base = '/'.join(include_parts_detail)
}@
#ifndef @(header_guard_variable)
#define @(header_guard_variable)

#include "@(include_base)__struct.h"
#include "@(include_base)__functions.h"
#include "@(include_base)__type_support.h"

#endif  // @(header_guard_variable)
