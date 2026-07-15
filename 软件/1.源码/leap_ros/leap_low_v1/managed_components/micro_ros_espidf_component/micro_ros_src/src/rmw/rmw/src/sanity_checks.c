// Copyright 2016-2017 Open Source Robotics Foundation, Inc.
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

#include "rmw/sanity_checks.h"

#include <stdlib.h>
#include <string.h>

#include "rcutils/types.h"

#include "rmw/error_handling.h"
#include "rmw/types.h"

rmw_ret_t
rmw_check_zero_rmw_string_array(
  rcutils_string_array_t * array)
{
  if (!array) {
    RMW_SET_ERROR_MSG("array is null");
    return RMW_RET_ERROR;
  }
  if (array->size != 0) {
    RMW_SET_ERROR_MSG("array size is not zero");
    return RMW_RET_ERROR;
  }
  if (array->data) {
    RMW_SET_ERROR_MSG("array data is not null");
    return RMW_RET_ERROR;
  }
  return RMW_RET_OK;
}
