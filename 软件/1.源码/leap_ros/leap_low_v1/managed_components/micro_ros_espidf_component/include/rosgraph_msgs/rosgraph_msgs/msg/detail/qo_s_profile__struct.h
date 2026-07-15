// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rosgraph_msgs:msg/QoSProfile.idl
// generated code does not contain a copyright notice

#ifndef ROSGRAPH_MSGS__MSG__DETAIL__QO_S_PROFILE__STRUCT_H_
#define ROSGRAPH_MSGS__MSG__DETAIL__QO_S_PROFILE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Constant 'HISTORY_SYSTEM_DEFAULT'.
/**
  * History policy
 */
enum
{
  rosgraph_msgs__msg__QoSProfile__HISTORY_SYSTEM_DEFAULT = 0
};

/// Constant 'HISTORY_KEEP_LAST'.
enum
{
  rosgraph_msgs__msg__QoSProfile__HISTORY_KEEP_LAST = 1
};

/// Constant 'HISTORY_KEEP_ALL'.
enum
{
  rosgraph_msgs__msg__QoSProfile__HISTORY_KEEP_ALL = 2
};

/// Constant 'HISTORY_UNKNOWN'.
enum
{
  rosgraph_msgs__msg__QoSProfile__HISTORY_UNKNOWN = 3
};

/// Constant 'RELIABILITY_SYSTEM_DEFAULT'.
/**
  * Reliability policy
 */
enum
{
  rosgraph_msgs__msg__QoSProfile__RELIABILITY_SYSTEM_DEFAULT = 0
};

/// Constant 'RELIABILITY_RELIABLE'.
enum
{
  rosgraph_msgs__msg__QoSProfile__RELIABILITY_RELIABLE = 1
};

/// Constant 'RELIABILITY_BEST_EFFORT'.
enum
{
  rosgraph_msgs__msg__QoSProfile__RELIABILITY_BEST_EFFORT = 2
};

/// Constant 'RELIABILITY_UNKNOWN'.
enum
{
  rosgraph_msgs__msg__QoSProfile__RELIABILITY_UNKNOWN = 3
};

/// Constant 'RELIABILITY_BEST_AVAILABLE'.
enum
{
  rosgraph_msgs__msg__QoSProfile__RELIABILITY_BEST_AVAILABLE = 4
};

/// Constant 'DURABILITY_SYSTEM_DEFAULT'.
/**
  * Durability policy
 */
enum
{
  rosgraph_msgs__msg__QoSProfile__DURABILITY_SYSTEM_DEFAULT = 0
};

/// Constant 'DURABILITY_TRANSIENT_LOCAL'.
enum
{
  rosgraph_msgs__msg__QoSProfile__DURABILITY_TRANSIENT_LOCAL = 1
};

/// Constant 'DURABILITY_VOLATILE'.
enum
{
  rosgraph_msgs__msg__QoSProfile__DURABILITY_VOLATILE = 2
};

/// Constant 'DURABILITY_UNKNOWN'.
enum
{
  rosgraph_msgs__msg__QoSProfile__DURABILITY_UNKNOWN = 3
};

/// Constant 'DURABILITY_BEST_AVAILABLE'.
enum
{
  rosgraph_msgs__msg__QoSProfile__DURABILITY_BEST_AVAILABLE = 4
};

/// Constant 'LIVELINESS_SYSTEM_DEFAULT'.
/**
  * Liveliness policy
 */
enum
{
  rosgraph_msgs__msg__QoSProfile__LIVELINESS_SYSTEM_DEFAULT = 0
};

/// Constant 'LIVELINESS_AUTOMATIC'.
enum
{
  rosgraph_msgs__msg__QoSProfile__LIVELINESS_AUTOMATIC = 1
};

/// Constant 'LIVELINESS_MANUAL_BY_TOPIC'.
enum
{
  rosgraph_msgs__msg__QoSProfile__LIVELINESS_MANUAL_BY_TOPIC = 3
};

/// Constant 'LIVELINESS_UNKNOWN'.
enum
{
  rosgraph_msgs__msg__QoSProfile__LIVELINESS_UNKNOWN = 4
};

/// Constant 'LIVELINESS_BEST_AVAILABLE'.
enum
{
  rosgraph_msgs__msg__QoSProfile__LIVELINESS_BEST_AVAILABLE = 5
};

// Include directives for member types
// Member 'deadline'
// Member 'lifespan'
// Member 'liveliness_lease_duration'
#include "builtin_interfaces/msg/detail/duration__struct.h"

/// Struct defined in msg/QoSProfile in the package rosgraph_msgs.
/**
  * Message-based representation of ROS 2 Quality of Service settings
  * Default values are kept in sync with RMW by integration test
  * Note that SYSTEM_DEFAULT and BEST_AVAILABLE values cannot be an observed value,
  * because they resolve concretely at runtime.
  * They are included here for completeness to match the data structures in RMW
 */
typedef struct rosgraph_msgs__msg__QoSProfile
{
  /// Depth of the message queue (only meaningful when history==KEEP_LAST)
  uint32_t depth;
  /// Deadline between messages (0 for no deadline)
  builtin_interfaces__msg__Duration deadline;
  /// Lifespan of each message (0 for infinite)
  builtin_interfaces__msg__Duration lifespan;
  uint8_t history;
  uint8_t reliability;
  uint8_t durability;
  uint8_t liveliness;
  /// Lease duration for liveliness (0 for infinite)
  builtin_interfaces__msg__Duration liveliness_lease_duration;
} rosgraph_msgs__msg__QoSProfile;

// Struct for a sequence of rosgraph_msgs__msg__QoSProfile.
typedef struct rosgraph_msgs__msg__QoSProfile__Sequence
{
  rosgraph_msgs__msg__QoSProfile * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rosgraph_msgs__msg__QoSProfile__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ROSGRAPH_MSGS__MSG__DETAIL__QO_S_PROFILE__STRUCT_H_
