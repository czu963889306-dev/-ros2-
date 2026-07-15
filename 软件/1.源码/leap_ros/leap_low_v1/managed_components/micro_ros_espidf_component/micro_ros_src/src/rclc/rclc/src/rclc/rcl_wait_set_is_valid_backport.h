// Copyright (c) 2019 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
// Copyright 2014 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCLC__RCL_WAIT_SET_IS_VALID_BACKPORT_H_
#define RCLC__RCL_WAIT_SET_IS_VALID_BACKPORT_H_

#if __cplusplus
extern "C"
{
#endif

#include <rcl/wait.h>
/**
 *  RCLC-Executor built for ROS2 Version Eloquent
 *  This is a compatability function, which is not available in ROS2 Dashing.
 *
 *  * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param[in] wait_set rcl wait set
 * \return true if wait_set is initialized
 * \return false otherwise
 */

bool
rcl_wait_set_is_valid(const rcl_wait_set_t * wait_set);

#if __cplusplus
}
#endif

#endif  // RCLC__RCL_WAIT_SET_IS_VALID_BACKPORT_H_
