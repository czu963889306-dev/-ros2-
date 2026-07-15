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

rmw_wait_set_t *
rmw_create_wait_set(
  rmw_context_t * context,
  size_t max_conditions)
{
  (void)context;
  (void)max_conditions;

  rmw_uxrce_mempool_item_t * memory_node = get_memory(&wait_set_memory);
  if (!memory_node) {
    RMW_UROS_TRACE_MESSAGE("Not available memory node")
    return NULL;
  }
  rmw_uxrce_wait_set_t * aux_wait_set = (rmw_uxrce_wait_set_t *)memory_node->data;

  return &aux_wait_set->rmw_wait_set;
}

rmw_ret_t
rmw_destroy_wait_set(
  rmw_wait_set_t * wait_set)
{
  rmw_uxrce_mempool_item_t * item = wait_set_memory.allocateditems;

  while (NULL != item) {
    rmw_uxrce_wait_set_t * aux_wait_set = (rmw_uxrce_wait_set_t *)item->data;
    if (&aux_wait_set->rmw_wait_set == wait_set) {
      put_memory(&wait_set_memory, item);
      return RMW_RET_OK;
    }
    item = item->next;
  }

  return RMW_RET_ERROR;
}
