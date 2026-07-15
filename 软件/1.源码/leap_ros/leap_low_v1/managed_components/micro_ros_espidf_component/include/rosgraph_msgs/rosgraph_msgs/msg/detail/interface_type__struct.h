// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rosgraph_msgs:msg/InterfaceType.idl
// generated code does not contain a copyright notice

#ifndef ROSGRAPH_MSGS__MSG__DETAIL__INTERFACE_TYPE__STRUCT_H_
#define ROSGRAPH_MSGS__MSG__DETAIL__INTERFACE_TYPE__STRUCT_H_

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
// Member 'hash'
#include "rosgraph_msgs/msg/detail/type_hash__struct.h"

/// Struct defined in msg/InterfaceType in the package rosgraph_msgs.
/**
  * Represent a type of a ROS Graph Interface
 */
typedef struct rosgraph_msgs__msg__InterfaceType
{
  /// The plaintext namespaced name of the type - e.g. sensor_msgs/Image
  rosidl_runtime_c__String name;
  /// The hash uniquely identifies the exact structure of the type,
  /// the definition of which may change between package version
  rosgraph_msgs__msg__TypeHash hash;
} rosgraph_msgs__msg__InterfaceType;

// Struct for a sequence of rosgraph_msgs__msg__InterfaceType.
typedef struct rosgraph_msgs__msg__InterfaceType__Sequence
{
  rosgraph_msgs__msg__InterfaceType * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosgraph_msgs__msg__InterfaceType__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSGRAPH_MSGS__MSG__DETAIL__INTERFACE_TYPE__STRUCT_H_
