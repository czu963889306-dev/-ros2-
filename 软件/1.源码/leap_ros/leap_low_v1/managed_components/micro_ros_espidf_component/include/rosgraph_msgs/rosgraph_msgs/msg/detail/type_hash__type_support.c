// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from rosgraph_msgs:msg/TypeHash.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "rosgraph_msgs/msg/detail/type_hash__rosidl_typesupport_introspection_c.h"
#include "rosgraph_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosgraph_msgs/msg/detail/type_hash__functions.h"
#include "rosgraph_msgs/msg/detail/type_hash__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  rosgraph_msgs__msg__TypeHash__init(message_memory);
}

void rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_fini_function(void * message_memory)
{
  rosgraph_msgs__msg__TypeHash__fini(message_memory);
}

size_t rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__size_function__TypeHash__value(
  const void * untyped_member)
{
  (void)untyped_member;
  return 32;
}

const void * rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__get_const_function__TypeHash__value(
  const void * untyped_member, size_t index)
{
  const uint8_t * member =
    (const uint8_t *)(untyped_member);
  return &member[index];
}

void * rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__get_function__TypeHash__value(
  void * untyped_member, size_t index)
{
  uint8_t * member =
    (uint8_t *)(untyped_member);
  return &member[index];
}

void rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__fetch_function__TypeHash__value(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint8_t * item =
    ((const uint8_t *)
    rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__get_const_function__TypeHash__value(untyped_member, index));
  uint8_t * value =
    (uint8_t *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__assign_function__TypeHash__value(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint8_t * item =
    ((uint8_t *)
    rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__get_function__TypeHash__value(untyped_member, index));
  const uint8_t * value =
    (const uint8_t *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_message_member_array[2] = {
  {
    "version",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__TypeHash, version),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    true,  // is array
    32,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__TypeHash, value),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__size_function__TypeHash__value,  // size() function pointer
    rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__get_const_function__TypeHash__value,  // get_const(index) function pointer
    rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__get_function__TypeHash__value,  // get(index) function pointer
    rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__fetch_function__TypeHash__value,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__assign_function__TypeHash__value,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_message_members = {
  "rosgraph_msgs__msg",  // message namespace
  "TypeHash",  // message name
  2,  // number of fields
  sizeof(rosgraph_msgs__msg__TypeHash),
  rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_message_member_array,  // message members
  rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_init_function,  // function to initialize message memory (memory has to be allocated)
  rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_message_type_support_handle = {
  0,
  &rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_rosgraph_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, TypeHash)() {
  if (!rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_message_type_support_handle.typesupport_identifier) {
    rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &rosgraph_msgs__msg__TypeHash__rosidl_typesupport_introspection_c__TypeHash_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
