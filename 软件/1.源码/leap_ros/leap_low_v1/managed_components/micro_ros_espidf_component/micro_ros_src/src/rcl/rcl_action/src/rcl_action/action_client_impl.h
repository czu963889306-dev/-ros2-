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

#ifndef RCL_ACTION__ACTION_CLIENT_IMPL_H_
#define RCL_ACTION__ACTION_CLIENT_IMPL_H_

#include "rcl_action/types.h"
#include "rcl/rcl.h"

typedef struct rcl_action_client_impl_s
{
  rcl_client_t goal_client;
  rcl_client_t cancel_client;
  rcl_client_t result_client;
  rcl_subscription_t feedback_subscription;
  rcl_subscription_t status_subscription;
  rcl_action_client_options_t options;
  char * action_name;
  // Wait set records
  size_t wait_set_goal_client_index;
  size_t wait_set_cancel_client_index;
  size_t wait_set_result_client_index;
  size_t wait_set_feedback_subscription_index;
  size_t wait_set_status_subscription_index;
} rcl_action_client_impl_t;


#endif  // RCL_ACTION__ACTION_CLIENT_IMPL_H_
