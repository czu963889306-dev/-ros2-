@# Included from rosidl_typesupport_microxrcedds_c/resource/idl__type_support_c.cpp.em
@{
from rosidl_cmake import convert_camel_case_to_lower_case_underscore
from rosidl_parser.definition import AbstractGenericString
from rosidl_parser.definition import AbstractNestedType
from rosidl_parser.definition import AbstractSequence
from rosidl_parser.definition import AbstractString
from rosidl_parser.definition import BoundedString
from rosidl_parser.definition import AbstractWString
from rosidl_parser.definition import ACTION_FEEDBACK_SUFFIX
from rosidl_parser.definition import ACTION_GOAL_SUFFIX
from rosidl_parser.definition import ACTION_RESULT_SUFFIX
from rosidl_parser.definition import Array
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import BoundedSequence
from rosidl_parser.definition import UnboundedSequence
from rosidl_parser.definition import NamespacedType

include_parts = [package_name] + list(interface_path.parents[0].parts) + [
    'detail', convert_camel_case_to_lower_case_underscore(interface_path.stem)]
include_base = '/'.join(include_parts)

header_files = [
    'stdint.h',
    'stdio.h',
    'string.h',
    # Provides the rosidl_typesupport_microxrcedds_c__identifier symbol declaration.
    'rosidl_typesupport_microxrcedds_c/identifier.h',
    # Provides the definition of the message_type_support_callbacks_t struct.
    'rosidl_typesupport_microxrcedds_c/message_type_support.h',
    package_name + '/msg/rosidl_typesupport_microxrcedds_c__visibility_control.h',
    include_base + '__struct.h',
    include_base + '__functions.h',
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

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

#define MICROXRCEDDS_PADDING sizeof(uint32_t)

// includes and forward declarations of message dependencies and their conversion functions

@# // Include the message header for each non-primitive field.
#if defined(__cplusplus)
extern "C"
{
#endif

@{
includes = {}
for member in message.structure.members:
    keys = set([])
    if isinstance(member.type, AbstractSequence) and isinstance(member.type.value_type, BasicType):
        keys.add('rosidl_runtime_c/primitives_sequence.h')
        keys.add('rosidl_runtime_c/primitives_sequence_functions.h')
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if isinstance(type_, AbstractString):
        keys.add('rosidl_runtime_c/string.h')
        keys.add('rosidl_runtime_c/string_functions.h')
    elif isinstance(type_, AbstractWString):
        keys.add('rosidl_runtime_c/u16string.h')
        keys.add('rosidl_runtime_c/u16string_functions.h')
    elif isinstance(type_, NamespacedType):
        if (
            type_.name.endswith(ACTION_GOAL_SUFFIX) or
            type_.name.endswith(ACTION_RESULT_SUFFIX) or
            type_.name.endswith(ACTION_FEEDBACK_SUFFIX)
        ):
            typename = type_.name.rsplit('_', 1)[0]
        else:
            typename = type_.name
        keys.add('/'.join(type_.namespaces + ['detail', convert_camel_case_to_lower_case_underscore(typename)]) + '__functions.h')
    for key in keys:
        if key not in includes:
            includes[key] = set([])
        includes[key].add(member.name)
}@
@[for header_file in sorted(includes.keys())]@
@[    if header_file in include_directives]@
// already included above
// @
@[    else]@
@{include_directives.add(header_file)}@
@[    end if]@
#include "@(header_file)"  // @(', '.join(sorted(includes[header_file])))
@[end for]@

// forward declare type support functions
@{
forward_declares = {}
for member in message.structure.members:
    type_ = member.type
    if isinstance(type_, AbstractNestedType):
        type_ = type_.value_type
    if isinstance(type_, NamespacedType):
        key = (*type_.namespaces, type_.name)
        if key not in includes:
            forward_declares[key] = set([])
        forward_declares[key].add(member.name)
}@
@[for key in sorted(forward_declares.keys())]@
@[  if key[0] != package_name]@
ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_IMPORT_@(package_name)
@[  end if]@
size_t get_serialized_size_@('__'.join(key))(
  const void * untyped_ros_message,
  size_t current_alignment);

@[  if key[0] != package_name]@
ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_IMPORT_@(package_name)
@[  end if]@
size_t max_serialized_size_@('__'.join(key))(
  bool * full_bounded,
  size_t current_alignment);

@[  if key[0] != package_name]@
ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_IMPORT_@(package_name)
@[  end if]@
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(key)))();
@[end for]@

@# // Make callback functions specific to this message type.

typedef @('__'.join(message.structure.namespaced_type.namespaced_name())) _@(message.structure.namespaced_type.name)__ros_msg_type;

static bool _@(message.structure.namespaced_type.name)__cdr_serialize(
  const void * untyped_ros_message,
  ucdrBuffer * cdr)
{
  (void) untyped_ros_message;
  (void) cdr;

  bool rv = false;

  if (!untyped_ros_message) {
    return false;
  }

  _@(message.structure.namespaced_type.name)__ros_msg_type * ros_message = (_@(message.structure.namespaced_type.name)__ros_msg_type *)(untyped_ros_message);
  (void)ros_message;

@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    const size_t size = @(member.type.size);
    rv = ucdr_serialize_array_@(get_suffix(member.type.value_type.typename))(cdr, ros_message->@(member.name), size);
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    const size_t array_size = sizeof(ros_message->@(member.name))/sizeof(ros_message->@(member.name)[0]);
    for(size_t i = 0; i < array_size; i++){
        rv = ((const message_type_support_callbacks_t *)(
          ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(member.type.value_type.namespaced_name()))
          )()->data))->cdr_serialize(&ros_message->@(member.name)[i], cdr);
        if(rv == false){
          break;
        }
    }
@[      else]@
    // Micro CDR only support arrays of basic and namespaced types.
@[      end if]@
@[    elif isinstance(member.type, AbstractSequence)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    const size_t size = ros_message->@(member.name).size;
    rv = ucdr_serialize_sequence_@(get_suffix(member.type.value_type.typename))(cdr, ros_message->@(member.name).data, size);
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    const size_t size = ros_message->@(member.name).size;
    rv = ucdr_serialize_uint32_t(cdr, size);

    if(rv == true){
      for(size_t i = 0; i < size; i++){
        rv = ((const message_type_support_callbacks_t *)(
          ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(member.type.value_type.namespaced_name()))
          )()->data))->cdr_serialize(&ros_message->@(member.name).data[i], cdr);
        if(rv == false){
          break;
        }
      }
    }
@[      elif isinstance(member.type.value_type, AbstractString)]@
    const size_t size = ros_message->@(member.name).size;
    rv = ucdr_serialize_uint32_t(cdr, size);
    for (size_t i = 0; rv && i < size; ++i) {
      uint32_t string_len = (ros_message->@(member.name).data[i].data == NULL) ? 0 : (uint32_t)strlen(ros_message->@(member.name).data[i].data) + 1;
      ros_message->@(member.name).data[i].size = (ros_message->@(member.name).data[i].data == NULL) ? 0 : string_len - 1;
      rv = ucdr_serialize_sequence_char(cdr, ros_message->@(member.name).data[i].data, string_len);
    }
@[      end if]@
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType)]@
@[    if get_suffix(member.type.typename) == "bool"]@
  rv = ucdr_serialize_@(get_suffix(member.type.typename))(cdr, (ros_message->@(member.name)) ? 0x01 : 0x00);
@[    else]@
  rv = ucdr_serialize_@(get_suffix(member.type.typename))(cdr, ros_message->@(member.name));
@[    end if]@
@[  elif isinstance(member.type, AbstractString)]@
 {
    uint32_t string_len = (ros_message->@(member.name).data == NULL) ? 0 : (uint32_t)strlen(ros_message->@(member.name).data) + 1;
    ros_message->@(member.name).size = (ros_message->@(member.name).data == NULL) ? 0 : string_len - 1 ;
    rv = ucdr_serialize_sequence_char(cdr, ros_message->@(member.name).data, string_len);
  }
@[  elif isinstance(member.type, AbstractWString)]@
  // Micro CDR does not support WString type.
@[  elif isinstance(member.type, NamespacedType)]@
  rv = ((const message_type_support_callbacks_t *)(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(member.type.namespaced_name()))
      )()->data))->cdr_serialize(&ros_message->@(member.name), cdr);
@[  else]@
  // Micro CDR does not support this type.
@[  end if]@
@[end for]@

  return rv;
}

static bool _@(message.structure.namespaced_type.name)__cdr_deserialize(
  ucdrBuffer * cdr,
  void * untyped_ros_message)
{
  (void) cdr;

  bool rv = false;

  if (!untyped_ros_message) {
    return false;
  }
  _@(message.structure.namespaced_type.name)__ros_msg_type * ros_message = (_@(message.structure.namespaced_type.name)__ros_msg_type *)(untyped_ros_message);
  (void)ros_message;

@[for member in message.structure.members]@
  // Field name: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    const size_t size = @(member.type.size);
    rv = ucdr_deserialize_array_@(get_suffix(member.type.value_type.typename))(cdr, ros_message->@(member.name), size);
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    const size_t array_size = sizeof(ros_message->@(member.name))/sizeof(ros_message->@(member.name)[0]);
    for(size_t i = 0; i < array_size; i++){
      rv = ((const message_type_support_callbacks_t *)(
        ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(member.type.value_type.namespaced_name()))
        )()->data))->cdr_deserialize(cdr, &ros_message->@(member.name)[i]);
      if(rv == false){
        break;
      }
    }
@[      else]@
    // Micro CDR only support arrays of basic and namespaced types.
@[      end if]@
@[    elif isinstance(member.type, AbstractSequence)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    uint32_t size;
    const size_t capacity = ros_message->@(member.name).capacity;
    rv = ucdr_deserialize_sequence_@(get_suffix(member.type.value_type.typename))(cdr, ros_message->@(member.name).data, capacity, &size);
    if (rv) {
      ros_message->@(member.name).size = size;
    } else if(size > capacity){
      cdr->error = false;
      cdr->last_data_size = 1;
      ros_message->@(member.name).size = 0;
      ucdr_align_to(cdr, sizeof(@(get_suffix(member.type.value_type.typename))));
      ucdr_advance_buffer(cdr, size * sizeof(@(get_suffix(member.type.value_type.typename))));
    }
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    uint32_t size;
    rv = ucdr_deserialize_uint32_t(cdr, &size);

    if(size > ros_message->@(member.name).capacity){
      return 0;
    }

    ros_message->@(member.name).size = size;
    for(size_t i = 0; i < size; i++){
      rv = ((const message_type_support_callbacks_t *)(
        ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(member.type.value_type.namespaced_name()))
        )()->data))->cdr_deserialize(cdr, &ros_message->@(member.name).data[i]);
      if(rv == false){
        break;
      }
    }
@[      elif isinstance(member.type.value_type, AbstractString)]@
    uint32_t size;
    rv = ucdr_deserialize_uint32_t(cdr, &size);

    if(size > ros_message->@(member.name).capacity){
      return 0;
    }
    ros_message->@(member.name).size = size;

    for (size_t i = 0; rv && i < size; i++) {
      size_t capacity = ros_message->@(member.name).data[i].capacity;
      uint32_t string_size;
      char * data = ros_message->@(member.name).data[i].data;
      rv = ucdr_deserialize_sequence_char(cdr, data, capacity, &string_size);
      if (rv) {
        ros_message->@(member.name).data[i].size = (string_size == 0) ? 0 : string_size - 1;
      } else if(string_size > capacity){
        cdr->error = false;
        cdr->last_data_size = 1;
        ros_message->@(member.name).data[i].size = 0;
        ucdr_align_to(cdr, sizeof(char));
        ucdr_advance_buffer(cdr, string_size);
      }
    }
@[      end if]@
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType)]@
  rv = ucdr_deserialize_@(get_suffix(member.type.typename))(cdr, &ros_message->@(member.name));
@[  elif isinstance(member.type, AbstractString)]@
  {
    size_t capacity = ros_message->@(member.name).capacity;
    uint32_t string_size;
    rv = ucdr_deserialize_sequence_char(cdr, ros_message->@(member.name).data, capacity, &string_size);
    if (rv) {
      ros_message->@(member.name).size = (string_size == 0) ? 0 : string_size - 1;
    } else if(string_size > capacity){
      cdr->error = false;
      cdr->last_data_size = 1;
      ros_message->@(member.name).size = 0;
      ucdr_align_to(cdr, sizeof(char));
      ucdr_advance_buffer(cdr, string_size);
    }
  }
@[  elif isinstance(member.type, AbstractWString)]@
  // Micro CDR does not support WString type.
@[  elif isinstance(member.type, NamespacedType)]@
  rv = ((const message_type_support_callbacks_t *)(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(member.type.namespaced_name()))
      )()->data))->cdr_deserialize(cdr, &ros_message->@(member.name));
@[  else]@
  // Micro CDR does not support this type.
@[  end if]@
@[end for]@
  return rv;
}

ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_PUBLIC_@(package_name)
size_t get_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  if (!untyped_ros_message) {
    return 0;
  }

  const _@(message.structure.namespaced_type.name)__ros_msg_type * ros_message = (const _@(message.structure.namespaced_type.name)__ros_msg_type *)(untyped_ros_message);
  (void)ros_message;

  const size_t initial_alignment = current_alignment;

@[for member in message.structure.members]@
  // Member: @(member.name)
@[  if isinstance(member.type, AbstractNestedType)]@
  {
@[    if isinstance(member.type, Array)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    const size_t array_size = @(member.type.size);
    const size_t item_size = sizeof(ros_message->@(member.name)[0]);
    current_alignment += ucdr_alignment(current_alignment, item_size) + (array_size * item_size);
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    const size_t array_size = sizeof(ros_message->@(member.name))/sizeof(ros_message->@(member.name)[0]);
    for(size_t i = 0; i < array_size; i++){
      size_t element_size = ((const message_type_support_callbacks_t *)(
        ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(member.type.value_type.namespaced_name()))
        )()->data))->get_serialized_size_with_initial_alignment(&ros_message->@(member.name)[i], current_alignment);
      current_alignment += element_size;
    }
@[      end if]@
@[    elif isinstance(member.type, AbstractSequence)]@
@[      if isinstance(member.type.value_type, BasicType)]@
    size_t sequence_size = ros_message->@(member.name).size;
    current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;

    if (0 < sequence_size) {
      size_t item_size = sizeof(ros_message->@(member.name).data[0]);
      current_alignment += ucdr_alignment(current_alignment, item_size) + (sequence_size * item_size);
    }
@[      elif isinstance(member.type.value_type, NamespacedType)]@
    const size_t sequence_size = ros_message->@(member.name).size;

    current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;

    for(size_t i = 0; i < sequence_size; i++){
      size_t element_size = ((const message_type_support_callbacks_t *)(
        ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join(member.type.value_type.namespaced_name()))
        )()->data))->get_serialized_size_with_initial_alignment(&ros_message->@(member.name).data[i], current_alignment);
      uint8_t alignment_size = (element_size < MICROXRCEDDS_PADDING) ? element_size : MICROXRCEDDS_PADDING;
      current_alignment += ucdr_alignment(current_alignment, alignment_size) + element_size;
    }
@[      elif isinstance(member.type.value_type, AbstractString)]@
    const size_t sequence_size = ros_message->@(member.name).size;
    current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;

    for(size_t i = 0; i < sequence_size; i++){
      current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;
      current_alignment += ros_message->@(member.name).data[i].size + 1;
    }
@[      end if]@
@[    end if]@
  }
@[  elif isinstance(member.type, BasicType)]@
  {
    const size_t item_size = sizeof(ros_message->@(member.name));
    current_alignment += ucdr_alignment(current_alignment, item_size) + item_size;
  }
@[  elif isinstance(member.type, AbstractString)]@
  current_alignment += ucdr_alignment(current_alignment, MICROXRCEDDS_PADDING) + MICROXRCEDDS_PADDING;
  current_alignment += ros_message->@(member.name).size + 1;
@[  elif isinstance(member.type, NamespacedType)]@
  current_alignment +=
    get_serialized_size_@('__'.join(member.type.namespaced_name()))(&ros_message->@(member.name), current_alignment);
@[  end if]@
@[end for]@

  return current_alignment - initial_alignment;
}

static uint32_t _@(message.structure.namespaced_type.name)__get_serialized_size(const void * untyped_ros_message)
{
  return (uint32_t)(
    get_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_MICROXRCEDDS_C_PUBLIC_@(package_name)
size_t max_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(
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
  current_alignment +=
    max_serialized_size_@('__'.join(member.type.namespaced_name()))(full_bounded, current_alignment);
@[  else]@
  *full_bounded = false;
@[  end if]@
@[end for]@

  return current_alignment - initial_alignment;
}

static size_t _@(message.structure.namespaced_type.name)__max_serialized_size()
{
  bool full_bounded;
  return max_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name]))(&full_bounded, 0);
}
@
@# // Collect the callback functions and provide a function to get the type support struct.

static message_type_support_callbacks_t __callbacks_@(message.structure.namespaced_type.name) = {
  "@('::'.join([package_name] + list(interface_path.parents[0].parts)))",
  "@(message.structure.namespaced_type.name)",
  _@(message.structure.namespaced_type.name)__cdr_serialize,
  _@(message.structure.namespaced_type.name)__cdr_deserialize,
  _@(message.structure.namespaced_type.name)__get_serialized_size,
  get_serialized_size_@('__'.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])),
  _@(message.structure.namespaced_type.name)__max_serialized_size
};

static rosidl_message_type_support_t _@(message.structure.namespaced_type.name)__type_support = {
  ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__IDENTIFIER_VALUE,
  &__callbacks_@(message.structure.namespaced_type.name),
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_microxrcedds_c, @(', '.join([package_name] + list(interface_path.parents[0].parts) + [message.structure.namespaced_type.name])))() {
  return &_@(message.structure.namespaced_type.name)__type_support;
}

#if defined(__cplusplus)
}
#endif
