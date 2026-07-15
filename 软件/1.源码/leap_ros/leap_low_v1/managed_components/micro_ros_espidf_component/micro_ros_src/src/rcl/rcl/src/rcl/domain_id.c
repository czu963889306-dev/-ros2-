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

#include "rcl/domain_id.h"

#include <errno.h>
#include <limits.h>

#include "rcutils/env.h"

#include "rcl/error_handling.h"
#include "rcl/types.h"

const char * const RCL_DOMAIN_ID_ENV_VAR = "ROS_DOMAIN_ID";

rcl_ret_t
rcl_get_default_domain_id(size_t * domain_id)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  const char * ros_domain_id = NULL;
  const char * get_env_error_str = NULL;

  RCL_CHECK_ARGUMENT_FOR_NULL(domain_id, RCL_RET_INVALID_ARGUMENT);

  get_env_error_str = rcutils_get_env(RCL_DOMAIN_ID_ENV_VAR, &ros_domain_id);
  if (NULL != get_env_error_str) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Error getting env var '" RCUTILS_STRINGIFY(RCL_DOMAIN_ID_ENV_VAR) "': %s\n",
      get_env_error_str);
    return RCL_RET_ERROR;
  }
  if (ros_domain_id && strcmp(ros_domain_id, "") != 0) {
    char * end = NULL;
    unsigned long number = strtoul(ros_domain_id, &end, 0);  // NOLINT(runtime/int)
    if (number == 0UL && *end != '\0') {
      RCL_SET_ERROR_MSG("ROS_DOMAIN_ID is not an integral number");
      return RCL_RET_ERROR;
    }
    if ((number == ULONG_MAX && errno == ERANGE) || number > SIZE_MAX) {
      RCL_SET_ERROR_MSG("ROS_DOMAIN_ID is out of range");
      return RCL_RET_ERROR;
    }
    *domain_id = (size_t)number;
  }
  return RCL_RET_OK;
}
