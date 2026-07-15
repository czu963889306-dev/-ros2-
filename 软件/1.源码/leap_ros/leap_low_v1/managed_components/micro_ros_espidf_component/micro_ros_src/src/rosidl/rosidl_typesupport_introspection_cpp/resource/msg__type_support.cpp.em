@# Included from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
@{
from rosidl_parser.definition import AbstractGenericString
from rosidl_parser.definition import AbstractNestedType
from rosidl_parser.definition import AbstractSequence
from rosidl_parser.definition import AbstractString
from rosidl_parser.definition import AbstractWString
from rosidl_parser.definition import Array
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import BoundedSequence
from rosidl_parser.definition import NamespacedType
from rosidl_cmake import convert_camel_case_to_lower_case_underscore

include_parts = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_base = '/'.join(include_parts)

header_files = [
    'array',
    'cstddef',  # providing offsetof()
    'string',
    'vector',
    'rosidl_runtime_c/message_type_support_struct.h',
    'rosidl_typesupport_cpp/message_type_support.hpp',
    'rosidl_typesupport_interface/macros.h',
    include_base + '__struct.hpp',
    'rosidl_typesupport_introspection_cpp/field_types.hpp',
    'rosidl_typesupport_introspection_cpp/identifier.hpp',
    'rosidl_typesupport_introspection_cpp/message_introspection.hpp',
    'rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp',
    'rosidl_typesupport_introspection_cpp/visibility_control.h',
]
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
@[for ns in message.structure.namespaced_type.namespaces]@

namespace @(ns)
{
@[end for]@

namespace rosidl_typesupport_introspection_cpp
{

void @(message.structure.namespaced_type.name)_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(_init);
}

void @(message.structure.namespaced_type.name)_fini_function(void * message_memory)
{
  auto typed_message = static_cast<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) *>(message_memory);
  typed_message->~@(message.structure.namespaced_type.name)();
}

@{
def is_vector_bool(member):
    from rosidl_parser.definition import BasicType
    from rosidl_parser.definition import AbstractSequence
    return isinstance(member.type, AbstractSequence) and isinstance(member.type.value_type, BasicType) and member.type.value_type.typename == 'boolean'
}@
@[for member in message.structure.members]@
@[  if isinstance(member.type, AbstractNestedType)]@
@{
from rosidl_generator_cpp import  MSG_TYPE_TO_CPP
if isinstance(member.type.value_type, BasicType):
    type_ = MSG_TYPE_TO_CPP[member.type.value_type.typename]
elif isinstance(member.type.value_type, AbstractString):
    type_ = 'std::string'
elif isinstance(member.type.value_type, AbstractWString):
    type_ = 'std::u16string'
elif isinstance(member.type.value_type, NamespacedType):
    type_ = '::'.join(member.type.value_type.namespaced_name())
}@
size_t size_function__@(message.structure.namespaced_type.name)__@(member.name)(const void * untyped_member)
{
@[    if isinstance(member.type, Array)]@
  (void)untyped_member;
  return @(member.type.size);
@[    else]@
  const auto * member = reinterpret_cast<const std::vector<@(type_)> *>(untyped_member);
  return member->size();
@[    end if]@
}

@[    if not is_vector_bool(member)]@
const void * get_const_function__@(message.structure.namespaced_type.name)__@(member.name)(const void * untyped_member, size_t index)
{
@[      if isinstance(member.type, Array)]@
  const auto & member =
    *reinterpret_cast<const std::array<@(type_), @(member.type.size)> *>(untyped_member);
@[      else]@
  const auto & member =
    *reinterpret_cast<const std::vector<@(type_)> *>(untyped_member);
@[      end if]@
  return &member[index];
}

void * get_function__@(message.structure.namespaced_type.name)__@(member.name)(void * untyped_member, size_t index)
{
@[      if isinstance(member.type, Array)]@
  auto & member =
    *reinterpret_cast<std::array<@(type_), @(member.type.size)> *>(untyped_member);
@[      else]@
  auto & member =
    *reinterpret_cast<std::vector<@(type_)> *>(untyped_member);
@[      end if]@
  return &member[index];
}

void fetch_function__@(message.structure.namespaced_type.name)__@(member.name)(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const @(type_) *>(
    get_const_function__@(message.structure.namespaced_type.name)__@(member.name)(untyped_member, index));
  auto & value = *reinterpret_cast<@(type_) *>(untyped_value);
  value = item;
}

void assign_function__@(message.structure.namespaced_type.name)__@(member.name)(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<@(type_) *>(
    get_function__@(message.structure.namespaced_type.name)__@(member.name)(untyped_member, index));
  const auto & value = *reinterpret_cast<const @(type_) *>(untyped_value);
  item = value;
}
@[    else]@
void fetch_function__@(message.structure.namespaced_type.name)__@(member.name)(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & member = *reinterpret_cast<const std::vector<@(type_)> *>(untyped_member);
  auto & value = *reinterpret_cast<@(type_) *>(untyped_value);
  value = member[index];
}

void assign_function__@(message.structure.namespaced_type.name)__@(member.name)(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & member = *reinterpret_cast<std::vector<@(type_)> *>(untyped_member);
  const auto & value = *reinterpret_cast<const @(type_) *>(untyped_value);
  member[index] = value;
}
@[    end if]@

@[    if isinstance(member.type, AbstractSequence)]@
void resize_function__@(message.structure.namespaced_type.name)__@(member.name)(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<@(type_)> *>(untyped_member);
  member->resize(size);
}

@[    end if]@
@[  end if]@
@[end for]@
static const ::rosidl_typesupport_introspection_cpp::MessageMember @(message.structure.namespaced_type.name)_message_member_array[@(len(message.structure.members))] = {
@{
for index, member in enumerate(message.structure.members):
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type

    print('  {')

    # const char * name_
    print('    "%s",  // name' % member.name)
    if isinstance(type_, BasicType):
        # uint8_t type_id_
        print('    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_%s,  // type' % type_.typename.replace(' ', '_').upper())
        # size_t string_upper_bound
        print('    0,  // upper bound of string')
        # const rosidl_generator_c::MessageTypeSupportHandle * members_
        print('    nullptr,  // members of sub message')
    elif isinstance(type_, AbstractGenericString):
        # uint8_t type_id_
        if isinstance(type_, AbstractString):
            print('    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_STRING,  // type')
        elif isinstance(type_, AbstractWString):
            print('    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_WSTRING,  // type')
        else:
            assert False, 'Unknown type: ' + str(type_)
        # size_t string_upper_bound
        print('    %u,  // upper bound of string' % (type_.maximum_size if type_.has_maximum_size() else 0))
        # const rosidl_generator_c::MessageTypeSupportHandle * members_
        print('    nullptr,  // members of sub message')
    else:
        # uint8_t type_id_
        print('    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type')
        # size_t string_upper_bound
        print('    0,  // upper bound of string')
        # const rosidl_message_type_support_t * members_
        print('    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<%s>(),  // members of sub message' % '::'.join(type_.namespaced_name()))
    # bool is_array_
    print('    %s,  // is array' % ('true' if isinstance(member.type, AbstractNestedType) else 'false'))
    # size_t array_size_
    print('    %u,  // array size' % (member.type.size if isinstance(member.type, Array) else (member.type.maximum_size if isinstance(member.type, BoundedSequence) else 0)))
    # bool is_upper_bound_
    print('    %s,  // is upper bound' % ('true' if isinstance(member.type, BoundedSequence) else 'false'))
    # unsigned long offset_
    print('    offsetof(%s::%s, %s),  // bytes offset in struct' % ('::'.join([package_name] + list(interface_path.parents[0].parts)), message.structure.namespaced_type.name, member.name))
    # void * default_value_
    print('    nullptr,  // default value')  # TODO default value to be set

    function_suffix = ('%s__%s' % (message.structure.namespaced_type.name, member.name)) if isinstance(member.type, AbstractNestedType) else None

    # size_t(const void *) size_function
    print('    %s,  // size() function pointer' % ('size_function__%s' % function_suffix if isinstance(member.type, AbstractNestedType) else 'nullptr'))
    # const void *(const void *, size_t) get_const_function
    print('    %s,  // get_const(index) function pointer' % ('get_const_function__%s' % function_suffix if isinstance(member.type, AbstractNestedType) and not is_vector_bool(member) else 'nullptr'))
    # void *(void *, size_t) get_function
    print('    %s,  // get(index) function pointer' % ('get_function__%s' % function_suffix if isinstance(member.type, AbstractNestedType) and not is_vector_bool(member) else 'nullptr'))
    # void(const void *, size_t, void *) fetch_function
    print('    %s,  // fetch(index, &value) function pointer' % ('fetch_function__%s' % function_suffix if isinstance(member.type, AbstractNestedType) else 'nullptr'))
    # void(void *, size_t, const void *) assign_function
    print('    %s,  // assign(index, value) function pointer' % ('assign_function__%s' % function_suffix if isinstance(member.type, AbstractNestedType) else 'nullptr'))
    # void(void *, size_t) resize_function
    print('    %s  // resize(index) function pointer' % ('resize_function__%s' % function_suffix if isinstance(member.type, AbstractSequence) else 'nullptr'))

    if index < len(message.structure.members) - 1:
        print('  },')
    else:
        print('  }')
}@
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers @(message.structure.namespaced_type.name)_message_members = {
  "@('::'.join([package_name] + list(interface_path.parents[0].parts)))",  // message namespace
  "@(message.structure.namespaced_type.name)",  // message name
  @(len(message.structure.members)),  // number of fields
  sizeof(@('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))),
  @(message.structure.namespaced_type.name)_message_member_array,  // message members
  @(message.structure.namespaced_type.name)_init_function,  // function to initialize message memory (memory has to be allocated)
  @(message.structure.namespaced_type.name)_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t @(message.structure.namespaced_type.name)_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &@(message.structure.namespaced_type.name)_message_members,
  get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_introspection_cpp
@[  for ns in reversed(message.structure.namespaced_type.namespaces)]@

}  // namespace @(ns)
@[  end for]@


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))>()
{
  return &::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::rosidl_typesupport_introspection_cpp::@(message.structure.namespaced_type.name)_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])))() {
  return &::@('::'.join([package_name] + list(interface_path.parents[0].parts)))::rosidl_typesupport_introspection_cpp::@(message.structure.namespaced_type.name)_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
