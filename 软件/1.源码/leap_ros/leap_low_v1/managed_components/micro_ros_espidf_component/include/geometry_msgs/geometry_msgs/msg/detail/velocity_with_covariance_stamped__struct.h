// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from geometry_msgs:msg/VelocityWithCovarianceStamped.idl
// generated code does not contain a copyright notice

#ifndef GEOMETRY_MSGS__MSG__DETAIL__VELOCITY_WITH_COVARIANCE_STAMPED__STRUCT_H_
#define GEOMETRY_MSGS__MSG__DETAIL__VELOCITY_WITH_COVARIANCE_STAMPED__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"
// Member 'body_frame_id'
// Member 'reference_frame_id'
#include "rosidl_runtime_c/string.h"
// Member 'velocity'
#include "geometry_msgs/msg/detail/twist_with_covariance__struct.h"

/// Struct defined in msg/VelocityWithCovarianceStamped in the package geometry_msgs.
/**
  * A timestamped velocity of a body whose frame is 'body_frame_id', measured
  * relative to the reference frame 'reference_frame_id', with the velocity and
  * covariance both expressed in the basis of the observation frame
  * 'header.frame_id'.
  *
  * - If 'body_frame_id' and 'header.frame_id' are identical, the velocity and
  *   covariance are expressed in the body's own basis. This is functionally
  *   equivalent to the body-twist convention used by
  *   'geometry_msgs/TwistStamped'.
  *
  * This message is the covariance-bearing analogue of
  * 'geometry_msgs/VelocityStamped'.
 */
typedef struct geometry_msgs__msg__VelocityWithCovarianceStamped
{
  std_msgs__msg__Header header;
  rosidl_runtime_c__String body_frame_id;
  rosidl_runtime_c__String reference_frame_id;
  geometry_msgs__msg__TwistWithCovariance velocity;
} geometry_msgs__msg__VelocityWithCovarianceStamped;

// Struct for a sequence of geometry_msgs__msg__VelocityWithCovarianceStamped.
typedef struct geometry_msgs__msg__VelocityWithCovarianceStamped__Sequence
{
  geometry_msgs__msg__VelocityWithCovarianceStamped * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} geometry_msgs__msg__VelocityWithCovarianceStamped__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // GEOMETRY_MSGS__MSG__DETAIL__VELOCITY_WITH_COVARIANCE_STAMPED__STRUCT_H_
