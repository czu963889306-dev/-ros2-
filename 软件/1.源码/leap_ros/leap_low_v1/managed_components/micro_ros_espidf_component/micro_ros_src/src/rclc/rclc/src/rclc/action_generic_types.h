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
#ifndef RCLC__ACTION_GENERIC_TYPES_H_
#define RCLC__ACTION_GENERIC_TYPES_H_

#if __cplusplus
extern "C"
{
#endif

#include "rclc/action_server.h"

typedef struct Generic_SendGoal_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
  void * goal;
} Generic_SendGoal_Request;

typedef struct Generic_SendGoal_Response
{
  bool accepted;
  builtin_interfaces__msg__Time stamp;
} Generic_SendGoal_Response;

typedef struct Generic_GetResult_Request
{
  unique_identifier_msgs__msg__UUID goal_id;
} Generic_GetResult_Request;

typedef struct Generic_GetResult_Response
{
  int8_t status;
  void * result;
} Generic_GetResult_Response;

typedef struct Generic_FeedbackMessage
{
  unique_identifier_msgs__msg__UUID goal_id;
  void * feedback;
} Generic_FeedbackMessage;


#if __cplusplus
}
#endif

#endif  // RCLC__ACTION_GENERIC_TYPES_H_
