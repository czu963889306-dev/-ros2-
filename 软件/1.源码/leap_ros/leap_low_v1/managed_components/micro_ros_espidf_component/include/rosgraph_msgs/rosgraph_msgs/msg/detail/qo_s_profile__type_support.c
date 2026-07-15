// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from rosgraph_msgs:msg/QoSProfile.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "rosgraph_msgs/msg/detail/qo_s_profile__rosidl_typesupport_introspection_c.h"
#include "rosgraph_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosgraph_msgs/msg/detail/qo_s_profile__functions.h"
#include "rosgraph_msgs/msg/detail/qo_s_profile__struct.h"


// Include directives for member types
// Member `deadline`
// Member `lifespan`
// Member `liveliness_lease_duration`
#include "builtin_interfaces/msg/duration.h"
// Member `deadline`
// Member `lifespan`
// Member `liveliness_lease_duration`
#include "builtin_interfaces/msg/detail/duration__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  rosgraph_msgs__msg__QoSProfile__init(message_memory);
}

void rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_fini_function(void * message_memory)
{
  rosgraph_msgs__msg__QoSProfile__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_member_array[8] = {
  {
    "depth",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__QoSProfile, depth),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "deadline",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__QoSProfile, deadline),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "lifespan",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__QoSProfile, lifespan),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "history",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__QoSProfile, history),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "reliability",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__QoSProfile, reliability),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "durability",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__QoSProfile, durability),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "liveliness",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__QoSProfile, liveliness),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "liveliness_lease_duration",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__QoSProfile, liveliness_lease_duration),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_members = {
  "rosgraph_msgs__msg",  // message namespace
  "QoSProfile",  // message name
  8,  // number of fields
  sizeof(rosgraph_msgs__msg__QoSProfile),
  rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_member_array,  // message members
  rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_init_function,  // function to initialize message memory (memory has to be allocated)
  rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_type_support_handle = {
  0,
  &rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_rosgraph_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, QoSProfile)() {
  rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Duration)();
  rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Duration)();
  rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_member_array[7].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, builtin_interfaces, msg, Duration)();
  if (!rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_type_support_handle.typesupport_identifier) {
    rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &rosgraph_msgs__msg__QoSProfile__rosidl_typesupport_introspection_c__QoSProfile_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
