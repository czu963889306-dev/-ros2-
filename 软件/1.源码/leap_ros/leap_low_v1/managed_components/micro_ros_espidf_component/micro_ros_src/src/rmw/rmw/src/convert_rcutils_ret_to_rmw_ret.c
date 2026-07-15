// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#include "rmw/convert_rcutils_ret_to_rmw_ret.h"

rmw_ret_t
rmw_convert_rcutils_ret_to_rmw_ret(rcutils_ret_t rcutils_ret)
{
  switch (rcutils_ret) {
    case RCUTILS_RET_OK:
      return RMW_RET_OK;
    case RCUTILS_RET_INVALID_ARGUMENT:
      return RMW_RET_INVALID_ARGUMENT;
    case RCUTILS_RET_BAD_ALLOC:
      return RMW_RET_BAD_ALLOC;
    case RCUTILS_RET_ERROR:
      return RMW_RET_ERROR;
    default:
      return RMW_RET_ERROR;
  }
}
