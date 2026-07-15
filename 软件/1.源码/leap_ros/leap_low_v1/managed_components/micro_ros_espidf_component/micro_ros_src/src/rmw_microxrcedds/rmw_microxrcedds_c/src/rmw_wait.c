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

#include <limits.h>
#include <math.h>

#include <rmw/rmw.h>
#include <rmw/time.h>
#include <uxr/client/core/session/session.h>

#include "./rmw_microros_internal/utils.h"

rmw_ret_t
rmw_wait(
  rmw_subscriptions_t * subscriptions,
  rmw_guard_conditions_t * guard_conditions,
  rmw_services_t * services,
  rmw_clients_t * clients,
  rmw_events_t * events,
  rmw_wait_set_t * wait_set,
  const rmw_time_t * wait_timeout)
{
  (void)events;
  (void)wait_set;

  // With `rmw_uxrce_wait_mutex` member `need_to_be_ran` is protected.
  // `session_memory` itself is not protected because it is not modified between
  // rmw_init and rmw_shutdown, and rmw_wait cannot be called concurrently with
  // those functions.
  UXR_LOCK(&rmw_uxrce_wait_mutex);

  if (!services && !clients && !subscriptions && !guard_conditions) {
    UXR_UNLOCK(&rmw_uxrce_wait_mutex);
    return RMW_RET_OK;
  }

  // Check if timeout
  union {
    int64_t i64;
    int32_t i32;
  } timeout;

  if (NULL == wait_timeout || rmw_time_equal(*wait_timeout, (rmw_time_t)RMW_DURATION_INFINITE)) {
    timeout.i32 = UXR_TIMEOUT_INF;
  } else {
    timeout.i64 = rmw_time_total_nsec(*wait_timeout) / 1000000ULL;
    timeout.i32 = (timeout.i64 > INT32_MAX) ? INT32_MAX : timeout.i64;
  }

  rmw_uxrce_clean_expired_static_input_buffer();

  // Clear run flag for all sessions
  rmw_uxrce_mempool_item_t * item = session_memory.allocateditems;
  while (item != NULL) {
    rmw_context_impl_t * custom_context = (rmw_context_impl_t *)item->data;
    custom_context->need_to_be_ran = false;
    item = item->next;
  }

  // TODO(pablogs9): What happens if there already data in one entity?
  // Enable flag for every XRCE session available in the entities
  for (size_t i = 0; services && i < services->service_count; ++i) {
    rmw_uxrce_service_t * custom_service = (rmw_uxrce_service_t *)services->services[i];
    custom_service->owner_node->context->need_to_be_ran = true;
  }

  for (size_t i = 0; clients && i < clients->client_count; ++i) {
    rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)clients->clients[i];
    custom_client->owner_node->context->need_to_be_ran = true;
  }

  for (size_t i = 0; subscriptions && i < subscriptions->subscriber_count; ++i) {
    rmw_uxrce_subscription_t * custom_subscription =
      (rmw_uxrce_subscription_t *)subscriptions->subscribers[i];
    custom_subscription->owner_node->context->need_to_be_ran = true;
  }

  // Count sessions to be ran
  uint8_t available_contexts = 0;
  item = session_memory.allocateditems;
  while (item != NULL) {
    rmw_context_impl_t * custom_context = (rmw_context_impl_t *)item->data;
    available_contexts += custom_context->need_to_be_ran ? 1 : 0;
    item = item->next;
  }

  // There is no context that contais any of the wait set entities. Nothing to wait here.
  if (available_contexts != 0) {
    int32_t per_session_timeout =
      (timeout.i32 == UXR_TIMEOUT_INF) ? UXR_TIMEOUT_INF :
      (int32_t)((float)timeout.i32 / (float)available_contexts);

    item = session_memory.allocateditems;
    while (item != NULL) {
      rmw_context_impl_t * custom_context = (rmw_context_impl_t *)item->data;
      if (custom_context->need_to_be_ran) {
        uxr_run_session_until_data(&custom_context->session, per_session_timeout);
      }
      item = item->next;
    }
  } else {
    // Spin with no blocking to handle session metatraffic
    item = session_memory.allocateditems;
    while (item != NULL) {
      rmw_context_impl_t * custom_context = (rmw_context_impl_t *)item->data;
      uxr_run_session_timeout(&custom_context->session, 0);
      item = item->next;
    }
  }

  UXR_UNLOCK(&rmw_uxrce_wait_mutex);

  bool buffered_status = false;

  // Check services
  for (size_t i = 0; services && i < services->service_count; ++i) {
    rmw_uxrce_service_t * custom_service = (rmw_uxrce_service_t *)services->services[i];

    if (NULL == rmw_uxrce_find_static_input_buffer_by_owner((void *) custom_service)) {
      services->services[i] = NULL;
    } else {
      buffered_status = true;
    }
  }

  // Check clients
  for (size_t i = 0; clients && i < clients->client_count; ++i) {
    rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)clients->clients[i];

    if (NULL == rmw_uxrce_find_static_input_buffer_by_owner((void *) custom_client)) {
      clients->clients[i] = NULL;
    } else {
      buffered_status = true;
    }
  }

  // Check subscriptions
  for (size_t i = 0; subscriptions && i < subscriptions->subscriber_count; ++i) {
    rmw_uxrce_subscription_t * custom_subscription =
      (rmw_uxrce_subscription_t *)subscriptions->subscribers[i];

    if (NULL == rmw_uxrce_find_static_input_buffer_by_owner((void *) custom_subscription)) {
      subscriptions->subscribers[i] = NULL;
    } else {
      buffered_status = true;
    }
  }

  // Check guard conditions
  for (size_t i = 0; guard_conditions && i < guard_conditions->guard_condition_count; ++i) {
    rmw_uxrce_guard_condition_t * custom_guard_condition =
      (rmw_uxrce_guard_condition_t *)guard_conditions->guard_conditions[i];
    if (custom_guard_condition->hasTriggered == false) {
      guard_conditions->guard_conditions[i] = NULL;
    } else {
      custom_guard_condition->hasTriggered = false;
      buffered_status = true;
    }
  }

  return (buffered_status) ? RMW_RET_OK : RMW_RET_TIMEOUT;
}
