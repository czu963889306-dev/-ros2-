// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rosgraph_msgs:msg/Graph.idl
// generated code does not contain a copyright notice

#ifndef ROSGRAPH_MSGS__MSG__DETAIL__GRAPH__STRUCT_H_
#define ROSGRAPH_MSGS__MSG__DETAIL__GRAPH__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'nodes'
#include "rosgraph_msgs/msg/detail/node__struct.h"

/// Struct defined in msg/Graph in the package rosgraph_msgs.
/**
  * Represents a ROS node graph, which is only a collection of nodes
 */
typedef struct rosgraph_msgs__msg__Graph
{
  rosgraph_msgs__msg__Node__Sequence nodes;
} rosgraph_msgs__msg__Graph;

// Struct for a sequence of rosgraph_msgs__msg__Graph.
typedef struct rosgraph_msgs__msg__Graph__Sequence
{
  rosgraph_msgs__msg__Graph * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosgraph_msgs__msg__Graph__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSGRAPH_MSGS__MSG__DETAIL__GRAPH__STRUCT_H_
