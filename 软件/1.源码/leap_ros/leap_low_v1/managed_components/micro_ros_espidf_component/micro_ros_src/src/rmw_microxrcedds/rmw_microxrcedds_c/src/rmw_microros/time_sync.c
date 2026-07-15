// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
#include <rmw/ret_types.h>

#include <uxr/client/util/time.h>

#include "../rmw_microros_internal/types.h"
#include "./rmw_microros_internal/error_handling_internal.h"

bool rmw_uros_epoch_synchronized()
{
  // Check session is initialized
  if (NULL == session_memory.allocateditems) {
    RMW_UROS_TRACE_MESSAGE("Uninitialized session.");
    return false;
  }
  rmw_uxrce_mempool_item_t * item = session_memory.allocateditems;
  rmw_context_impl_t * context = (rmw_context_impl_t *)item->data;

  bool ret = context->session.synchronized;
  return ret;
}

int64_t rmw_uros_epoch_millis()
{
  // Check session is initialized
  if (NULL == session_memory.allocateditems) {
    RMW_UROS_TRACE_MESSAGE("Uninitialized session.");
    return 0;
  }

  rmw_uxrce_mempool_item_t * item = session_memory.allocateditems;
  rmw_context_impl_t * context = (rmw_context_impl_t *)item->data;

  int64_t ret = uxr_epoch_millis(&context->session);
  return ret;
}

int64_t rmw_uros_epoch_nanos()
{
  // Check session is initialized
  if (NULL == session_memory.allocateditems) {
    RMW_UROS_TRACE_MESSAGE("Uninitialized session.");
    return 0;
  }

  rmw_uxrce_mempool_item_t * item = session_memory.allocateditems;
  rmw_context_impl_t * context = (rmw_context_impl_t *)item->data;

  int64_t ret = uxr_epoch_nanos(&context->session);
  return ret;
}

rmw_ret_t rmw_uros_sync_session(
  const int timeout_ms)
{
  rmw_ret_t ret = RMW_RET_OK;

  // Check session is initialized
  if (NULL == session_memory.allocateditems) {
    RMW_UROS_TRACE_MESSAGE("Uninitialized session.");
    return RMW_RET_ERROR;
  }

  rmw_uxrce_mempool_item_t * item = session_memory.allocateditems;
  rmw_context_impl_t * context = (rmw_context_impl_t *)item->data;

  if (!uxr_sync_session(&context->session, timeout_ms)) {
    RMW_UROS_TRACE_MESSAGE("Time synchronization failed.")
    ret = RMW_RET_ERROR;
  }
  return ret;
}
