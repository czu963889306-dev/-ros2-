// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rosgraph_msgs:msg/TypeHash.idl
// generated code does not contain a copyright notice

#ifndef ROSGRAPH_MSGS__MSG__DETAIL__TYPE_HASH__STRUCT_H_
#define ROSGRAPH_MSGS__MSG__DETAIL__TYPE_HASH__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/TypeHash in the package rosgraph_msgs.
/**
  * RIHS spec version
 */
typedef struct rosgraph_msgs__msg__TypeHash
{
  uint8_t version;
  /// ROSIDL_TYPE_HASH_SIZE == 32
  uint8_t value[32];
} rosgraph_msgs__msg__TypeHash;

// Struct for a sequence of rosgraph_msgs__msg__TypeHash.
typedef struct rosgraph_msgs__msg__TypeHash__Sequence
{
  rosgraph_msgs__msg__TypeHash * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosgraph_msgs__msg__TypeHash__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSGRAPH_MSGS__MSG__DETAIL__TYPE_HASH__STRUCT_H_
