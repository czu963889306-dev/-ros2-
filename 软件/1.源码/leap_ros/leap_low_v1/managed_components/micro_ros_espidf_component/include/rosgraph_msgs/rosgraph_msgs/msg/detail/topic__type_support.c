// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from rosgraph_msgs:msg/Topic.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "rosgraph_msgs/msg/detail/topic__rosidl_typesupport_introspection_c.h"
#include "rosgraph_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosgraph_msgs/msg/detail/topic__functions.h"
#include "rosgraph_msgs/msg/detail/topic__struct.h"


// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"
// Member `type`
#include "rosgraph_msgs/msg/interface_type.h"
// Member `type`
#include "rosgraph_msgs/msg/detail/interface_type__rosidl_typesupport_introspection_c.h"
// Member `qos`
#include "rosgraph_msgs/msg/qo_s_profile.h"
// Member `qos`
#include "rosgraph_msgs/msg/detail/qo_s_profile__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  rosgraph_msgs__msg__Topic__init(message_memory);
}

void rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_fini_function(void * message_memory)
{
  rosgraph_msgs__msg__Topic__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_member_array[3] = {
  {
    "name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Topic, name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Topic, type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "qos",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Topic, qos),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_members = {
  "rosgraph_msgs__msg",  // message namespace
  "Topic",  // message name
  3,  // number of fields
  sizeof(rosgraph_msgs__msg__Topic),
  rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_member_array,  // message members
  rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_init_function,  // function to initialize message memory (memory has to be allocated)
  rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_type_support_handle = {
  0,
  &rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_rosgraph_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Topic)() {
  rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, InterfaceType)();
  rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, QoSProfile)();
  if (!rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_type_support_handle.typesupport_identifier) {
    rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &rosgraph_msgs__msg__Topic__rosidl_typesupport_introspection_c__Topic_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
