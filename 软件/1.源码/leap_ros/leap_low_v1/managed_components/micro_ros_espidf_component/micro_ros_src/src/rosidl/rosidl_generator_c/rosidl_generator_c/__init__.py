# Copyright 2015 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from rosidl_cmake import convert_camel_case_to_lower_case_underscore
from rosidl_cmake import generate_files
from rosidl_parser.definition import AbstractGenericString
from rosidl_parser.definition import AbstractSequence
from rosidl_parser.definition import AbstractString
from rosidl_parser.definition import AbstractType
from rosidl_parser.definition import AbstractWString
from rosidl_parser.definition import Array
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import CHARACTER_TYPES
from rosidl_parser.definition import NamespacedType
from rosidl_parser.definition import OCTET_TYPE


def generate_c(generator_arguments_file):
    mapping = {
        'idl.h.em': '%s.h',
        'idl__functions.c.em': 'detail/%s__functions.c',
        'idl__functions.h.em': 'detail/%s__functions.h',
        'idl__struct.h.em': 'detail/%s__struct.h',
        'idl__type_support.h.em': 'detail/%s__type_support.h',
    }
    return generate_files(
        generator_arguments_file, mapping,
        post_process_callback=prefix_with_bom_if_necessary)


def prefix_with_bom_if_necessary(content):
    try:
        content.encode('ASCII')
    except UnicodeError:
        prefix = '\ufeff' + \
            '// NOLINT: This file starts with a BOM ' + \
            'since it contain non-ASCII characters\n'
        content = prefix + content
    return content


BASIC_IDL_TYPES_TO_C = {
    'float': 'float',
    'double': 'double',
    'long double': 'long double',
    'char': 'signed char',
    'wchar': 'uint16_t',
    'boolean': 'bool',
    'octet': 'uint8_t',
    'uint8': 'uint8_t',
    'int8': 'int8_t',
    'uint16': 'uint16_t',
    'int16': 'int16_t',
    'uint32': 'uint32_t',
    'int32': 'int32_t',
    'uint64': 'uint64_t',
    'int64': 'int64_t',
}


def idl_structure_type_to_c_include_prefix(namespaced_type, subdirectory=None):
    parts = [
        convert_camel_case_to_lower_case_underscore(x)
        for x in (namespaced_type.namespaced_name())]
    if subdirectory is not None:
        parts[-1:-1] = [subdirectory]
    include_prefix = '/'.join(parts)
    # Strip service or action suffixes
    if include_prefix.endswith('__request'):
        include_prefix = include_prefix[:-9]
    elif include_prefix.endswith('__response'):
        include_prefix = include_prefix[:-10]
    if include_prefix.endswith('__goal'):
        include_prefix = include_prefix[:-6]
    elif include_prefix.endswith('__result'):
        include_prefix = include_prefix[:-8]
    elif include_prefix.endswith('__feedback'):
        include_prefix = include_prefix[:-10]
    elif include_prefix.endswith('__send_goal'):
        include_prefix = include_prefix[:-11]
    elif include_prefix.endswith('__get_result'):
        include_prefix = include_prefix[:-12]
    return include_prefix


def idl_structure_type_to_c_typename(namespaced_type):
    return '__'.join(namespaced_type.namespaced_name())


def idl_structure_type_sequence_to_c_typename(namespaced_type):
    return idl_structure_type_to_c_typename(namespaced_type) + '__Sequence'


def interface_path_to_string(interface_path):
    return '/'.join(
        list(interface_path.parents[0].parts) + [interface_path.stem])


def idl_declaration_to_c(type_, name):
    """
    Convert an IDL type into the C declaration.

    Example input: uint32, std_msgs/String
    Example output: uint32_t, char *

    @param type_: The message type
    @type type_: rosidl_parser.Type
    @param type_: The member name
    @type type_: str
    """
    if isinstance(type_, AbstractGenericString):
        return basetype_to_c(type_) + ' ' + name
    if isinstance(type_, Array):
        return basetype_to_c(type_.value_type) + ' ' + name + '[' + str(type_.size) + ']'
    return idl_type_to_c(type_) + ' ' + name


def idl_type_to_c(type_):
    if isinstance(type_, Array):
        assert False, 'The array size is part of the variable'
    if isinstance(type_, AbstractSequence):
        if isinstance(type_.value_type, BasicType):
            c_type = 'rosidl_runtime_c__' + type_.value_type.typename.replace(' ', '_')
        else:
            c_type = basetype_to_c(type_.value_type)
        c_type += '__Sequence'
        return c_type
    return basetype_to_c(type_)


def basetype_to_c(basetype):
    if isinstance(basetype, BasicType):
        return BASIC_IDL_TYPES_TO_C[basetype.typename]
    if isinstance(basetype, AbstractString):
        return 'rosidl_runtime_c__String'
    if isinstance(basetype, AbstractWString):
        return 'rosidl_runtime_c__U16String'
    if isinstance(basetype, NamespacedType):
        return idl_structure_type_to_c_typename(basetype)
    assert False, str(basetype)


def value_to_c(type_, value):
    assert isinstance(type_, AbstractType)
    assert value is not None

    if isinstance(type_, AbstractString):
        return '"%s"' % escape_string(value)

    if isinstance(type_, AbstractWString):
        return 'u"%s"' % escape_wstring(value)

    return basic_value_to_c(type_, value)


def basic_value_to_c(type_, value):
    assert isinstance(type_, BasicType)
    assert value is not None

    if 'boolean' == type_.typename:
        return 'true' if value else 'false'

    if type_.typename in (
        *CHARACTER_TYPES,
        OCTET_TYPE,
        'int8',
        'uint8',
        'int16',
        'uint16',
    ):
        return str(value)

    if type_.typename == 'int32':
        # Handle edge case for INT32_MIN
        # Specifically, MSVC is not happy in this case
        if -2147483648 == value:
            return '({0}l - 1)'.format(value + 1)
        return f'{value}l'

    if type_.typename == 'uint32':
        return f'{value}ul'

    if type_.typename == 'int64':
        # Handle edge case for INT64_MIN
        # See https://en.cppreference.com/w/cpp/language/integer_literal
        if -9223372036854775808 == value:
            return '({0}ll - 1)'.format(value + 1)
        return f'{value}ll'

    if type_.typename == 'uint64':
        return f'{value}ull'

    if 'float' == type_.typename:
        return f'{value}f'

    if 'double' == type_.typename:
        return f'{value}l'

    assert False, "unknown basic type '%s'" % type_


def escape_string(s):
    s = s.replace('\\', '\\\\')
    s = s.replace('"', r'\"')
    return s


def escape_wstring(s):
    return escape_string(s)
