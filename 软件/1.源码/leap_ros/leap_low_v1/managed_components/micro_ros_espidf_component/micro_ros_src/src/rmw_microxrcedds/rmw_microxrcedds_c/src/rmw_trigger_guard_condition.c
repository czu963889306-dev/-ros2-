// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <rmw/rmw.h>
#include <rmw/names_and_types.h>

#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_ret_t
rmw_trigger_guard_condition(
  const rmw_guard_condition_t * guard_condition)
{
  rmw_ret_t ret = RMW_RET_OK;
  if (!guard_condition) {
    RMW_UROS_TRACE_MESSAGE("guard condition pointer is null")
    ret = RMW_RET_ERROR;
  } else if (!is_uxrce_rmw_identifier_valid(guard_condition->implementation_identifier)) {
    RMW_UROS_TRACE_MESSAGE("guard condition handle not from this implementation")
    ret = RMW_RET_ERROR;
  } else {
    rmw_uxrce_guard_condition_t * aux_guard_condition =
      (rmw_uxrce_guard_condition_t *)guard_condition->data;
    aux_guard_condition->hasTriggered = true;
  }

  return ret;
}
