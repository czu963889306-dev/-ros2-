// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rosgraph_msgs:msg/Topic.idl
// generated code does not contain a copyright notice

#ifndef ROSGRAPH_MSGS__MSG__DETAIL__TOPIC__STRUCT_H_
#define ROSGRAPH_MSGS__MSG__DETAIL__TOPIC__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'name'
#include "rosidl_runtime_c/string.h"
// Member 'type'
#include "rosgraph_msgs/msg/detail/interface_type__struct.h"
// Member 'qos'
#include "rosgraph_msgs/msg/detail/qo_s_profile__struct.h"

/// Struct defined in msg/Topic in the package rosgraph_msgs.
/**
  * Describes a single topic endpoint, which may be a Publisher or Subscription
 */
typedef struct rosgraph_msgs__msg__Topic
{
  /// Fully qualified name of the topic
  rosidl_runtime_c__String name;
  /// Type of the topic
  rosgraph_msgs__msg__InterfaceType type;
  /// Observed QoS of the endpoint
  rosgraph_msgs__msg__QoSProfile qos;
} rosgraph_msgs__msg__Topic;

// Struct for a sequence of rosgraph_msgs__msg__Topic.
typedef struct rosgraph_msgs__msg__Topic__Sequence
{
  rosgraph_msgs__msg__Topic * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosgraph_msgs__msg__Topic__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSGRAPH_MSGS__MSG__DETAIL__TOPIC__STRUCT_H_
