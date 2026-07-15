// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rosgraph_msgs:msg/Action.idl
// generated code does not contain a copyright notice

#ifndef ROSGRAPH_MSGS__MSG__DETAIL__ACTION__STRUCT_H_
#define ROSGRAPH_MSGS__MSG__DETAIL__ACTION__STRUCT_H_

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
// Member 'send_goal'
// Member 'get_result'
// Member 'cancel_goal'
#include "rosgraph_msgs/msg/detail/service__struct.h"
// Member 'feedback'
// Member 'status'
#include "rosgraph_msgs/msg/detail/topic__struct.h"

/// Struct defined in msg/Action in the package rosgraph_msgs.
/**
  * Describes a single Action endpoint, which may be a Server or Client
 */
typedef struct rosgraph_msgs__msg__Action
{
  /// Fully qualified name of the Action
  rosidl_runtime_c__String name;
  /// An action is actually a composition of the following fundamental ROS entities
  rosgraph_msgs__msg__Service send_goal;
  rosgraph_msgs__msg__Service get_result;
  rosgraph_msgs__msg__Service cancel_goal;
  rosgraph_msgs__msg__Topic feedback;
  rosgraph_msgs__msg__Topic status;
} rosgraph_msgs__msg__Action;

// Struct for a sequence of rosgraph_msgs__msg__Action.
typedef struct rosgraph_msgs__msg__Action__Sequence
{
  rosgraph_msgs__msg__Action * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosgraph_msgs__msg__Action__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSGRAPH_MSGS__MSG__DETAIL__ACTION__STRUCT_H_
