// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rosgraph_msgs:msg/Service.idl
// generated code does not contain a copyright notice

#ifndef ROSGRAPH_MSGS__MSG__DETAIL__SERVICE__STRUCT_H_
#define ROSGRAPH_MSGS__MSG__DETAIL__SERVICE__STRUCT_H_

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
// Member 'request_type'
// Member 'response_type'
#include "rosgraph_msgs/msg/detail/interface_type__struct.h"
// Member 'request_qos'
// Member 'response_qos'
#include "rosgraph_msgs/msg/detail/qo_s_profile__struct.h"

/// Struct defined in msg/Service in the package rosgraph_msgs.
/**
  * Describes a single Service endpoint, which may be a Server or Client
 */
typedef struct rosgraph_msgs__msg__Service
{
  /// Fully qualified name of the Service
  rosidl_runtime_c__String name;
  /// Type and actual QoS of the request publisher (Client) or subscription (Server)
  rosgraph_msgs__msg__InterfaceType request_type;
  rosgraph_msgs__msg__QoSProfile request_qos;
  /// Type and actual QoS of the request subscription (Client) or publisher (Server)
  rosgraph_msgs__msg__InterfaceType response_type;
  rosgraph_msgs__msg__QoSProfile response_qos;
} rosgraph_msgs__msg__Service;

// Struct for a sequence of rosgraph_msgs__msg__Service.
typedef struct rosgraph_msgs__msg__Service__Sequence
{
  rosgraph_msgs__msg__Service * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosgraph_msgs__msg__Service__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSGRAPH_MSGS__MSG__DETAIL__SERVICE__STRUCT_H_
