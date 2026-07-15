// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from rosgraph_msgs:msg/Graph.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "rosgraph_msgs/msg/detail/graph__rosidl_typesupport_introspection_c.h"
#include "rosgraph_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosgraph_msgs/msg/detail/graph__functions.h"
#include "rosgraph_msgs/msg/detail/graph__struct.h"


// Include directives for member types
// Member `nodes`
#include "rosgraph_msgs/msg/node.h"
// Member `nodes`
#include "rosgraph_msgs/msg/detail/node__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  rosgraph_msgs__msg__Graph__init(message_memory);
}

void rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_fini_function(void * message_memory)
{
  rosgraph_msgs__msg__Graph__fini(message_memory);
}

size_t rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__size_function__Graph__nodes(
  const void * untyped_member)
{
  const rosgraph_msgs__msg__Node__Sequence * member =
    (const rosgraph_msgs__msg__Node__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__get_const_function__Graph__nodes(
  const void * untyped_member, size_t index)
{
  const rosgraph_msgs__msg__Node__Sequence * member =
    (const rosgraph_msgs__msg__Node__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__get_function__Graph__nodes(
  void * untyped_member, size_t index)
{
  rosgraph_msgs__msg__Node__Sequence * member =
    (rosgraph_msgs__msg__Node__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__fetch_function__Graph__nodes(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosgraph_msgs__msg__Node * item =
    ((const rosgraph_msgs__msg__Node *)
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__get_const_function__Graph__nodes(untyped_member, index));
  rosgraph_msgs__msg__Node * value =
    (rosgraph_msgs__msg__Node *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__assign_function__Graph__nodes(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosgraph_msgs__msg__Node * item =
    ((rosgraph_msgs__msg__Node *)
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__get_function__Graph__nodes(untyped_member, index));
  const rosgraph_msgs__msg__Node * value =
    (const rosgraph_msgs__msg__Node *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__resize_function__Graph__nodes(
  void * untyped_member, size_t size)
{
  rosgraph_msgs__msg__Node__Sequence * member =
    (rosgraph_msgs__msg__Node__Sequence *)(untyped_member);
  rosgraph_msgs__msg__Node__Sequence__fini(member);
  return rosgraph_msgs__msg__Node__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_member_array[1] = {
  {
    "nodes",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Graph, nodes),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__size_function__Graph__nodes,  // size() function pointer
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__get_const_function__Graph__nodes,  // get_const(index) function pointer
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__get_function__Graph__nodes,  // get(index) function pointer
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__fetch_function__Graph__nodes,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__assign_function__Graph__nodes,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__resize_function__Graph__nodes  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_members = {
  "rosgraph_msgs__msg",  // message namespace
  "Graph",  // message name
  1,  // number of fields
  sizeof(rosgraph_msgs__msg__Graph),
  rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_member_array,  // message members
  rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_init_function,  // function to initialize message memory (memory has to be allocated)
  rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_type_support_handle = {
  0,
  &rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_rosgraph_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Graph)() {
  rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Node)();
  if (!rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_type_support_handle.typesupport_identifier) {
    rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &rosgraph_msgs__msg__Graph__rosidl_typesupport_introspection_c__Graph_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
