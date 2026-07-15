// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include "rcl/localhost.h"

#include <stdlib.h>
#include <string.h>

#include "rcutils/env.h"

#include "rcl/error_handling.h"
#include "rcl/types.h"

const char * const RCL_LOCALHOST_ENV_VAR = "ROS_LOCALHOST_ONLY";

rcl_ret_t
rcl_get_localhost_only(rmw_localhost_only_t * localhost_only)
{
  const char * ros_local_host_env_val = NULL;
  const char * get_env_error_str = NULL;

  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);
  RCL_CHECK_ARGUMENT_FOR_NULL(localhost_only, RCL_RET_INVALID_ARGUMENT);

  get_env_error_str = rcutils_get_env(RCL_LOCALHOST_ENV_VAR, &ros_local_host_env_val);
  if (NULL != get_env_error_str) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Error getting env var '" RCUTILS_STRINGIFY(RCL_LOCALHOST_ENV_VAR) "': %s\n",
      get_env_error_str);
    return RCL_RET_ERROR;
  }
  *localhost_only = (ros_local_host_env_val != NULL &&
    strcmp(
      ros_local_host_env_val,
      "1") == 0) ? RMW_LOCALHOST_ONLY_ENABLED : RMW_LOCALHOST_ONLY_DISABLED;
  return RCL_RET_OK;
}
