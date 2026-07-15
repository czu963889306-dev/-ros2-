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
#include <rmw/allocators.h>

#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_guard_condition_t *
rmw_create_guard_condition(
  rmw_context_t * context)
{
  (void)context;

  rmw_uxrce_mempool_item_t * memory_node = get_memory(&guard_condition_memory);
  if (!memory_node) {
    RMW_UROS_TRACE_MESSAGE("Not available memory node")
    return NULL;
  }
  rmw_uxrce_guard_condition_t * aux_guard_condition =
    (rmw_uxrce_guard_condition_t *)memory_node->data;
  aux_guard_condition->hasTriggered = false;
  aux_guard_condition->rmw_guard_condition.context = context;
  aux_guard_condition->rmw_guard_condition.implementation_identifier =
    rmw_get_implementation_identifier();
  aux_guard_condition->rmw_guard_condition.data = aux_guard_condition;

  return &aux_guard_condition->rmw_guard_condition;
}

rmw_ret_t
rmw_destroy_guard_condition(
  rmw_guard_condition_t * guard_condition)
{
  rmw_uxrce_mempool_item_t * item = guard_condition_memory.allocateditems;

  while (NULL != item) {
    rmw_uxrce_guard_condition_t * aux_guard_condition =
      (rmw_uxrce_guard_condition_t *)item->data;
    if (&aux_guard_condition->rmw_guard_condition == guard_condition) {
      put_memory(&guard_condition_memory, item);
      return RMW_RET_OK;
    }
    item = item->next;
  }

  return RMW_RET_ERROR;
}
