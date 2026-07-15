// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rosgraph_msgs:msg/Node.idl
// generated code does not contain a copyright notice

#ifndef ROSGRAPH_MSGS__MSG__DETAIL__NODE__STRUCT_H_
#define ROSGRAPH_MSGS__MSG__DETAIL__NODE__STRUCT_H_

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
// Member 'parameters'
#include "rcl_interfaces/msg/detail/parameter_descriptor__struct.h"
// Member 'parameter_values'
#include "rcl_interfaces/msg/detail/parameter_value__struct.h"
// Member 'publishers'
// Member 'subscriptions'
#include "rosgraph_msgs/msg/detail/topic__struct.h"
// Member 'service_clients'
// Member 'service_servers'
#include "rosgraph_msgs/msg/detail/service__struct.h"
// Member 'action_clients'
// Member 'action_servers'
#include "rosgraph_msgs/msg/detail/action__struct.h"

/// Struct defined in msg/Node in the package rosgraph_msgs.
/**
  * Represents the observable runtime state of a ROS Node
  * Therefore, does not perfectly align with the abstract specification which created it.
 */
typedef struct rosgraph_msgs__msg__Node
{
  /// Fully qualified node name (FQN)
  rosidl_runtime_c__String name;
  /// Parameter specifications for the node
  rcl_interfaces__msg__ParameterDescriptor__Sequence parameters;
  /// Current values of the node's parameters
  /// NOTE:
  ///   parameter_values[] must be empty, or the same size as parameters[]
  ///   When set, parameter_values[] match 1:1 with the same index in parameters[]
  rcl_interfaces__msg__ParameterValue__Sequence parameter_values;
  /// Communications endpoints - Topics, Services, and Actions
  rosgraph_msgs__msg__Topic__Sequence publishers;
  rosgraph_msgs__msg__Topic__Sequence subscriptions;
  rosgraph_msgs__msg__Service__Sequence service_clients;
  rosgraph_msgs__msg__Service__Sequence service_servers;
  rosgraph_msgs__msg__Action__Sequence action_clients;
  rosgraph_msgs__msg__Action__Sequence action_servers;
} rosgraph_msgs__msg__Node;

// Struct for a sequence of rosgraph_msgs__msg__Node.
typedef struct rosgraph_msgs__msg__Node__Sequence
{
  rosgraph_msgs__msg__Node * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosgraph_msgs__msg__Node__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSGRAPH_MSGS__MSG__DETAIL__NODE__STRUCT_H_
