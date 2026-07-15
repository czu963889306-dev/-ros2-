@# Included from rosidl_typesupport_microxrcedds_cpp/resource/idl__type_support.cpp.em
@{
from rosidl_parser.definition import AbstractGenericString
from rosidl_parser.definition import AbstractNestedType
from rosidl_parser.definition import AbstractSequence
from rosidl_parser.definition import AbstractString
from rosidl_parser.definition import BoundedString
from rosidl_parser.definition import AbstractWString
from rosidl_parser.definition import Array
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import BoundedSequence
from rosidl_parser.definition import UnboundedSequence
from rosidl_parser.definition import NamespacedType

header_files = [
    'limits',
    'algorithm',
    'stdexcept',
    'string',
    'cstring',
    'rosidl_typesupport_cpp/message_type_support.hpp',
    'rosidl_typesupport_microxrcedds_cpp/identifier.hpp',
    'rosidl_typesupport_microxrcedds_c/message_type_support.h',
    'rosidl_typesupport_microxrcedds_cpp/message_type_support_decl.hpp',
    'ucdr/microcdr.h',
]

def get_suffix(typename):
  if typename == 'boolean':
    return 'bool'
  elif typename == 'int8':
    return 'int8_t'
  elif typename in ('uint8', 'octet'):
    return 'uint8_t'
  elif typename == 'uint16':
    return 'uint16_t'
  elif typename == 'int16':
    return 'int16_t'
  elif typename == 'uint32':
    return 'uint32_t'
  elif typename == 'int32':
    return 'int32_t'
  elif typename == 'uint64':
    return 'uint64_t'
  elif typename == 'int64':
    return 'int64_t'
  else:
    return typename

}@
@[for header_file in header_files]@
@[    if header_file in include_directives]@
// already included above
// @
@[    else]@
@{include_directives.add(header_file)}@
@[    end if]@
@[    if '/' not in header_file]@
#include <@(header_file)>
@[    else]@
#include "@(header_file)"
@[    end if]@
@[end for]@

#define MICROXRCEDDS_PADDING sizeof(uint32_t)

// forward declaration of message dependencies and their conversion functions
@[for member in message.structure.members]@
@{
type_ = member.type
if isinstance(type_, AbstractNestedType):
    type_ = type_.value_type
}@
@[  if isinstance(type_, NamespacedType)]@
@[    for ns in type_.namespaces]@
namespace @(ns)
{
@[    end for]@
namespace typesupport_microxrcedds_cpp
{
bool cdr_serialize(
  const @('::'.join(type_.namespaced_name())) &,
  ucdrBuffer *);

bool cdr_deserialize(
  ucdrBuffer *,
  @('::'.join(type_.namespaced_name())) &);

size_t get_serialized_size(
  const @('::'.join(type_.namespaced_name())) &,
  size_t current_alignment);

size_t
max_serialized_size_@(type_.name)(
  bool * full_bounded,
  size_t current_alignment);
}  // namespace typesupport_microxrcedds_cpp
@[    for ns in reversed(type_.namespaces)]@
}  // namespace @(ns)
@[    end for]@

@[  end if]@
@[end for]@
@
@[  for ns in message.structure.namespaced_type.namespaces]@

namespace @(ns)
{
@[  end for]@

namespace typesupport_microxrcedds_cpp
{

bool
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_@(package_name)
cdr_serialize(
  const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) & ros_message,
  ucdrBuffer * cdr)
{
  (void) ros_message;
  (void) cdr;
  bool rv = false;

@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    size_t size = ros_message.@(member.name).size();
    rv = ucdr_serialize_array_@(get_suffix(member.type.value_type.typename))(cdr, ros_message.@(member.name).data(), size);
@[      else]@
    // Micro CDR only support arrays of basic types.
@[      end if]@
@[    elif isinstance(member.type, AbstractSequence)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    size_t size = ros_message.@(member.name).size();
@[        if member.type.value_type.typename == 'boolean']@
@[          if isinstance(member.type, BoundedSequence)]@
    bool temp[@(member.type.maximum_size)] = {0};
    std::copy(ros_message.@(member.name).begin(), ros_message.@(member.name).end(), std::begin(temp));
    rv = ucdr_serialize_sequence_bool(cdr, temp, size);
@[          else]@
    bool * temp = new bool[size];
    std::copy(ros_message.@(member.name).begin(), ros_message.@(member.name).end(), temp);
    rv = ucdr_serialize_sequence_bool(cdr, temp, size);
    delete[] temp;
@[          end if]@
@[        else]@
    rv = ucdr_serialize_sequence_@(get_suffix(member.type.value_type.typename))(cdr, &ros_message.@(member.name)[0], size);
@[        end if]@
@[      elif isinstance(member.type.value_type, AbstractString)]@
    size_t size = ros_message.@(member.name).size();
    rv = ucdr_serialize_uint32_t(cdr, size);
    for (size_t i = 0; rv && i < size; ++i) {
      rv = ucdr_serialize_string(cdr, ros_message.@(member.name)[i].c_str());
    }
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    size_t size = ros_message.@(member.name).size();
    rv = ucdr_serialize_uint32_t(cdr, size);

    size_t i = 0;
    while (i < size && rv) {
      rv = @('::'.join(member.type.value_type.namespaces))::typesupport_microxrcedds_cpp::cdr_serialize(
        ros_message.@(member.name)[i],
        cdr);
      i++;
    }
@[      end if]@
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType)]@
  rv = ucdr_serialize_@(get_suffix(member.type.typename))(cdr, ros_message.@(member.name));
@[  elif isinstance(member.type, AbstractString)]@
  rv = ucdr_serialize_string(cdr, ros_message.@(member.name).c_str());
@[  elif isinstance(member.type, AbstractWString)]@
  // Micro CDR does not support WString type.
@[  elif isinstance(member.type, NamespacedType)]@
  rv = @('::'.join(member.type.namespaces))::typesupport_microxrcedds_cpp::cdr_serialize(
    ros_message.@(member.name),
    cdr);
@[  else]@
  // Micro CDR does not support this type.
@[  end if]@
@[end for]@

  return rv;
}

bool
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_@(package_name)
cdr_deserialize(
  ucdrBuffer * cdr,
  @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) & ros_message)
{
  (void) cdr;
  (void) ros_message;
  bool rv = false;

@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    const size_t size = ros_message.@(member.name).size();
    rv = ucdr_deserialize_array_@(get_suffix(member.type.value_type.typename))(cdr, ros_message.@(member.name).data(), size);
@[      else]@
    // Micro CDR only support arrays of basic types.
@[      end if]@
@[    elif isinstance(member.type, AbstractSequence)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    uint32_t size;
    const size_t capacity = ros_message.@(member.name).capacity();
    ros_message.@(member.name).resize(capacity);
@[        if member.type.value_type.typename == 'boolean']@
@[          if isinstance(member.type, BoundedSequence)]@
    bool temp[@(member.type.maximum_size)] = {0};
    rv = ucdr_deserialize_sequence_bool(cdr, temp, @(member.type.maximum_size), &size);
    if (rv) {
      std::copy(std::begin(temp), std::begin(temp) + size, ros_message.@(member.name).begin());
    }
@[          else]@
    bool * temp = new bool[capacity];
    rv = ucdr_deserialize_sequence_bool(cdr, temp, capacity, &size);
    if (rv) {
      std::copy(temp, temp + size, ros_message.@(member.name).begin());
    }
    delete[] temp;
@[          end if]@
@[        else]@
    rv = ucdr_deserialize_sequence_@(get_suffix(member.type.value_type.typename))(cdr, &ros_message.@(member.name)[0], capacity, &size);
@[        end if]@
    if (rv) {
      ros_message.@(member.name).resize(size);
    }
@[      elif isinstance(member.type.value_type, AbstractString)]@
    uint32_t size;
    rv = ucdr_deserialize_uint32_t(cdr, &size);

    if (size > ros_message.@(member.name).capacity()) {
      ros_message.@(member.name).resize(size);
    } else {
      ros_message.@(member.name).resize(ros_message.@(member.name).capacity());
    }

    for (size_t i = 0; rv && i < size; i++) {
      uint32_t capacity = ros_message.@(member.name)[i].capacity();
      char * temp = static_cast<char *>(malloc(capacity * sizeof(char)));
      rv = ucdr_deserialize_string(cdr, temp, capacity);
      if (rv) {
        std::string stemp(temp);
        stemp.shrink_to_fit();
        ros_message.@(member.name)[i] = std::move(stemp);
      }
      free(temp);
    }
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    uint32_t size;
    rv = ucdr_deserialize_uint32_t(cdr, &size);
    ros_message.@(member.name).resize(size);

    size_t i = 0;
    while (i < size && rv) {
      rv = @('::'.join(member.type.value_type.namespaces))::typesupport_microxrcedds_cpp::cdr_deserialize(
        cdr,
        ros_message.@(member.name)[i]);
      i++;
    }
@[      end if]@
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType)]@
  rv = ucdr_deserialize_@(get_suffix(member.type.typename))(cdr, &ros_message.@(member.name));
@[  elif isinstance(member.type, AbstractString)]@
  ros_message.@(member.name).resize(ros_message.@(member.name).capacity());
  rv = ucdr_deserialize_string(cdr, &ros_message.@(member.name)[0], ros_message.@(member.name).capacity());
  if (rv) {
    ros_message.@(member.name).resize(std::strlen(&ros_message.@(member.name)[0]));
  }
@[  elif isinstance(member.type, AbstractWString)]@
  // Micro CDR does not support WString type.
@[  elif isinstance(member.type, NamespacedType)]@
  rv = @('::'.join(member.type.namespaces))::typesupport_microxrcedds_cpp::cdr_deserialize(
    cdr,
    ros_message.@(member.name));
@[  else]@
  // Micro CDR does not support this type.
@[  end if]@
@[end for]@

  return rv;
}

size_t
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_@(package_name)
get_serialized_size(
  const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) & ros_message,
  size_t current_alignment)
{
  (void) current_alignment;
  (void) ros_message;

  const size_t initial_alignment = current_alignment;

@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    const size_t array_size = @(member.type.size);
    const size_t item_size = sizeof(ros_message.@(member.name)[0]);
    current_alignment += ucdr_alignment(current_alignment, item_size) + (array_size * item_size);
@[      end if]@
@[    elif isinstance(member.type, AbstractSequence)]@
  // Member is abstractsequence
@[      if isinstance(member.type.value_type, BasicType)]@
    size_t sequence_size = ros_message.@(member.name).size();
    size_t item_size = sizeof(ros_message.@(member.name)[0]);
    current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;
    current_alignment += ucdr_alignment(current_alignment, item_size) + (sequence_size * item_size);
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    const size_t sequence_size = ros_message.@(member.name).size();
    current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;

    for (size_t i = 0; i < sequence_size; i++) {
      const size_t item_size = @('::'.join(member.type.value_type.namespaces))::typesupport_microxrcedds_cpp::get_serialized_size(
        ros_message.@(member.name)[i],
        current_alignment);
      current_alignment += ucdr_alignment(current_alignment, item_size) + item_size;
    }
@[      elif isinstance(member.type.value_type, AbstractString)]@
    const size_t sequence_size = ros_message.@(member.name).size();
    current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;

    for (size_t i = 0; i < sequence_size; i++) {
      const size_t item_size = ros_message.@(member.name)[i].size() + 1;
      current_alignment += ucdr_alignment(current_alignment, item_size) + item_size;
    }
@[      end if]@
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType)]@
  {
    const size_t item_size = sizeof(ros_message.@(member.name));
    current_alignment += ucdr_alignment(current_alignment, item_size) + item_size;
  }
@[  elif isinstance(member.type, AbstractString)]@
  current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;
  current_alignment += ros_message.@(member.name).size() + 1;
@[  elif isinstance(member.type, NamespacedType)]@
  current_alignment += @('::'.join(member.type.namespaces))::typesupport_microxrcedds_cpp::get_serialized_size(
    ros_message.@(member.name),
    current_alignment);
@[  end if]@
@[end for]@

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_PUBLIC_@(package_name)
max_serialized_size_@(message.structure.namespaced_type.name)(
  bool * full_bounded,
  size_t current_alignment)
{
  (void) current_alignment;
  *full_bounded = true;

  const size_t initial_alignment = current_alignment;

@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    const size_t array_size = @(member.type.size);
    current_alignment += ucdr_alignment(current_alignment, sizeof(@(get_suffix(member.type.value_type.typename)))) + (array_size * sizeof(@(get_suffix(member.type.value_type.typename))));
@[      else]@
    *full_bounded = false;
@[      end if]@
@[    elif isinstance(member.type, BoundedSequence)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    const size_t max_sequence_size = @(member.type.maximum_size);
    current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;
    current_alignment += ucdr_alignment(current_alignment, sizeof(@(get_suffix(member.type.value_type.typename)))) + (max_sequence_size * sizeof(@(get_suffix(member.type.value_type.typename))));
@[      else]@
    *full_bounded = false;
@[      end if]@
@[    elif isinstance(member.type, UnboundedSequence)]@
    *full_bounded = false;
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType)]@
  current_alignment += ucdr_alignment(current_alignment, sizeof(@(get_suffix(member.type.typename)))) + sizeof(@(get_suffix(member.type.typename)));
@[  elif isinstance(member.type, BoundedString)]@
  current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING);
  current_alignment += @(member.type.maximum_size) + 1;
@[  elif isinstance(member.type, NamespacedType)]@
  current_alignment += @('::'.join(member.type.namespaces))::typesupport_microxrcedds_cpp::max_serialized_size_@(member.type.name)(
    full_bounded,
    current_alignment);
@[  else]@
  *full_bounded = false;
@[  end if]@
@[end for]@

  return current_alignment - initial_alignment;
}

static bool _@(message.structure.namespaced_type.name)__cdr_serialize(
  const void * untyped_ros_message,
  ucdrBuffer * cdr)
{
  auto typed_message =
    static_cast<const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _@(message.structure.namespaced_type.name)__cdr_deserialize(
  ucdrBuffer * cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _@(message.structure.namespaced_type.name)__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _@(message.structure.namespaced_type.name)__get_serialized_size_with_initial_alignment(
  const void * untyped_ros_message, size_t current_alignment)
{
  auto typed_message =
    static_cast<const @('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])) *>(
    untyped_ros_message);
  return static_cast<size_t>(get_serialized_size(*typed_message, current_alignment));
}

static size_t _@(message.structure.namespaced_type.name)__max_serialized_size()
{
  bool full_bounded;
  return max_serialized_size_@(message.structure.namespaced_type.name)(&full_bounded, 0);
}

static message_type_support_callbacks_t _@(message.structure.namespaced_type.name)__callbacks = {
  "@('::'.join([package_name] + list(interface_path.parents[0].parts)))",
  "@(message.structure.namespaced_type.name)",
  _@(message.structure.namespaced_type.name)__cdr_serialize,
  _@(message.structure.namespaced_type.name)__cdr_deserialize,
  _@(message.structure.namespaced_type.name)__get_serialized_size,
  _@(message.structure.namespaced_type.name)__get_serialized_size_with_initial_alignment,
  _@(message.structure.namespaced_type.name)__max_serialized_size
};

static rosidl_message_type_support_t _@(message.structure.namespaced_type.name)__handle = {
  rosidl_typesupport_microxrcedds_cpp::typesupport_identifier,
  &_@(message.structure.namespaced_type.name)__callbacks,
  get_message_typesupport_handle_function,
};

}  // namespace typesupport_microxrcedds_cpp
@[  for ns in reversed(message.structure.namespaced_type.namespaces)]@

}  // namespace @(ns)
@[  end for]@

namespace rosidl_typesupport_microxrcedds_cpp
{

template<>
ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_EXPORT_@(package_name)
const rosidl_message_type_support_t *
get_message_type_support_handle<@('::'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))>()
{
  return &@('::'.join([package_name] + list(interface_path.parents[0].parts)))::typesupport_microxrcedds_cpp::_@(message.structure.namespaced_type.name)__handle;
}

}  // namespace rosidl_typesupport_microxrcedds_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_cpp, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])))() {
  return &@('::'.join([package_name] + list(interface_path.parents[0].parts)))::typesupport_microxrcedds_cpp::_@(message.structure.namespaced_type.name)__handle;
}

#ifdef __cplusplus
}
#endif
