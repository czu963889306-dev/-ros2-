// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#ifndef RCL_ACTION__ACTION_SERVER_IMPL_H_
#define RCL_ACTION__ACTION_SERVER_IMPL_H_

#include "rcl_action/types.h"
#include "rcl/rcl.h"

/// Internal rcl_action implementation struct.
typedef struct rcl_action_server_impl_s
{
  rcl_service_t goal_service;
  rcl_service_t cancel_service;
  rcl_service_t result_service;
  rcl_publisher_t feedback_publisher;
  rcl_publisher_t status_publisher;
  rcl_timer_t expire_timer;
  char * action_name;
  rcl_action_server_options_t options;
  // Array of goal handles
  rcl_action_goal_handle_t ** goal_handles;
  size_t num_goal_handles;
  // Clock
  rcl_clock_t * clock;
  // Wait set records
  size_t wait_set_goal_service_index;
  size_t wait_set_cancel_service_index;
  size_t wait_set_result_service_index;
  size_t wait_set_expire_timer_index;
} rcl_action_server_impl_t;


#endif  // RCL_ACTION__ACTION_SERVER_IMPL_H_
