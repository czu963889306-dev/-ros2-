// Copyright (c) 2021 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
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
#ifndef RCLC__ACTION_CLIENT_INTERNAL_H_
#define RCLC__ACTION_CLIENT_INTERNAL_H_

#if __cplusplus
extern "C"
{
#endif

#include <rclc/types.h>

#include <rclc/client.h>
#include <rclc/action_goal_handle.h>

rcl_ret_t
rclc_action_send_result_request(
  rclc_action_goal_handle_t * goal_handle);

#if __cplusplus
}
#endif

#endif  // RCLC__ACTION_CLIENT_INTERNAL_H_
