// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/wait.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "rcl/error_handling.h"
#include "rcl/time.h"
#include "rcutils/logging_macros.h"
#include "rmw/error_handling.h"
#include "rmw/rmw.h"
#include "rmw/event.h"

#include "./context_impl.h"

struct rcl_wait_set_impl_s
{
  // number of subscriptions that have been added to the wait set
  size_t subscription_index;
  rmw_subscriptions_t rmw_subscriptions;
  // number of guard_conditions that have been added to the wait set
  size_t guard_condition_index;
  rmw_guard_conditions_t rmw_guard_conditions;
  // number of clients that have been added to the wait set
  size_t client_index;
  rmw_clients_t rmw_clients;
  // number of services that have been added to the wait set
  size_t service_index;
  rmw_services_t rmw_services;
  // number of events that have been added to the wait set
  size_t event_index;
  rmw_events_t rmw_events;

  rmw_wait_set_t * rmw_wait_set;
  // number of timers that have been added to the wait set
  size_t timer_index;
  // context with which the wait set is associated
  rcl_context_t * context;
  // allocator used in the wait set
  rcl_allocator_t allocator;
};

rcl_wait_set_t
rcl_get_zero_initialized_wait_set()
{
  static rcl_wait_set_t null_wait_set = {
    .subscriptions = NULL,
    .size_of_subscriptions = 0,
    .guard_conditions = NULL,
    .size_of_guard_conditions = 0,
    .clients = NULL,
    .size_of_clients = 0,
    .services = NULL,
    .size_of_services = 0,
    .timers = NULL,
    .size_of_timers = 0,
    .events = NULL,
    .size_of_events = 0,
    .impl = NULL,
  };
  return null_wait_set;
}

bool
rcl_wait_set_is_valid(const rcl_wait_set_t * wait_set)
{
  return wait_set && wait_set->impl;
}

static void
__wait_set_clean_up(rcl_wait_set_t * wait_set)
{
  rcl_ret_t ret = rcl_wait_set_resize(wait_set, 0, 0, 0, 0, 0, 0);
  (void)ret;  // NO LINT
  assert(RCL_RET_OK == ret);  // Defensive, shouldn't fail with size 0.
  if (wait_set->impl) {
    wait_set->impl->allocator.deallocate(wait_set->impl, wait_set->impl->allocator.state);
    wait_set->impl = NULL;
  }
}

rcl_ret_t
rcl_wait_set_init(
  rcl_wait_set_t * wait_set,
  size_t number_of_subscriptions,
  size_t number_of_guard_conditions,
  size_t number_of_timers,
  size_t number_of_clients,
  size_t number_of_services,
  size_t number_of_events,
  rcl_context_t * context,
  rcl_allocator_t allocator)
{
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Initializing wait set with "
    "'%zu' subscriptions, '%zu' guard conditions, '%zu' timers, '%zu' clients, '%zu' services",
    number_of_subscriptions, number_of_guard_conditions, number_of_timers, number_of_clients,
    number_of_services);
  rcl_ret_t fail_ret = RCL_RET_ERROR;

  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set, RCL_RET_INVALID_ARGUMENT);
  if (rcl_wait_set_is_valid(wait_set)) {
    RCL_SET_ERROR_MSG("wait_set already initialized, or memory was uninitialized.");
    return RCL_RET_ALREADY_INIT;
  }
  // Make sure rcl has been initialized.
  RCL_CHECK_ARGUMENT_FOR_NULL(context, RCL_RET_INVALID_ARGUMENT);
  if (!rcl_context_is_valid(context)) {
    RCL_SET_ERROR_MSG(
      "the given context is not valid, "
      "either rcl_init() was not called or rcl_shutdown() was called.");
    return RCL_RET_NOT_INIT;
  }
  // Allocate space for the implementation struct.
  wait_set->impl = (rcl_wait_set_impl_t *)allocator.allocate(
    sizeof(rcl_wait_set_impl_t), allocator.state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    wait_set->impl, "allocating memory failed", return RCL_RET_BAD_ALLOC);
  memset(wait_set->impl, 0, sizeof(rcl_wait_set_impl_t));
  wait_set->impl->rmw_subscriptions.subscribers = NULL;
  wait_set->impl->rmw_subscriptions.subscriber_count = 0;
  wait_set->impl->rmw_guard_conditions.guard_conditions = NULL;
  wait_set->impl->rmw_guard_conditions.guard_condition_count = 0;
  wait_set->impl->rmw_clients.clients = NULL;
  wait_set->impl->rmw_clients.client_count = 0;
  wait_set->impl->rmw_services.services = NULL;
  wait_set->impl->rmw_services.service_count = 0;
  wait_set->impl->rmw_events.events = NULL;
  wait_set->impl->rmw_events.event_count = 0;
  // Set context.
  wait_set->impl->context = context;
  // Set allocator.
  wait_set->impl->allocator = allocator;

  size_t num_conditions =
    (2 * number_of_subscriptions) +
    number_of_guard_conditions +
    number_of_clients +
    number_of_services +
    number_of_events;

  wait_set->impl->rmw_wait_set = rmw_create_wait_set(&(context->impl->rmw_context), num_conditions);
  if (!wait_set->impl->rmw_wait_set) {
    goto fail;
  }

  // Initialize subscription space.
  rcl_ret_t ret = rcl_wait_set_resize(
    wait_set, number_of_subscriptions, number_of_guard_conditions, number_of_timers,
    number_of_clients, number_of_services, number_of_events);
  if (RCL_RET_OK != ret) {
    fail_ret = ret;
    goto fail;
  }
  return RCL_RET_OK;
fail:
  if (rcl_wait_set_is_valid(wait_set)) {
    rmw_ret_t ret = rmw_destroy_wait_set(wait_set->impl->rmw_wait_set);
    if (ret != RMW_RET_OK) {
      fail_ret = RCL_RET_WAIT_SET_INVALID;
    }
  }
  __wait_set_clean_up(wait_set);
  return fail_ret;
}

rcl_ret_t
rcl_wait_set_fini(rcl_wait_set_t * wait_set)
{
  rcl_ret_t result = RCL_RET_OK;
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set, RCL_RET_INVALID_ARGUMENT);

  if (rcl_wait_set_is_valid(wait_set)) {
    rmw_ret_t ret = rmw_destroy_wait_set(wait_set->impl->rmw_wait_set);
    if (ret != RMW_RET_OK) {
      RCL_SET_ERROR_MSG(rmw_get_error_string().str);
      result = RCL_RET_WAIT_SET_INVALID;
    }
    __wait_set_clean_up(wait_set);
  }
  return result;
}

rcl_ret_t
rcl_wait_set_get_allocator(const rcl_wait_set_t * wait_set, rcl_allocator_t * allocator)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set, RCL_RET_INVALID_ARGUMENT);
  if (!rcl_wait_set_is_valid(wait_set)) {
    RCL_SET_ERROR_MSG("wait set is invalid");
    return RCL_RET_WAIT_SET_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(allocator, RCL_RET_INVALID_ARGUMENT);
  *allocator = wait_set->impl->allocator;
  return RCL_RET_OK;
}

#define SET_ADD(Type) \
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set, RCL_RET_INVALID_ARGUMENT); \
  if (!wait_set->impl) { \
    RCL_SET_ERROR_MSG("wait set is invalid"); \
    return RCL_RET_WAIT_SET_INVALID; \
  } \
  RCL_CHECK_ARGUMENT_FOR_NULL(Type, RCL_RET_INVALID_ARGUMENT); \
  if (!(wait_set->impl->Type ## _index < wait_set->size_of_ ## Type ## s)) { \
    RCL_SET_ERROR_MSG(#Type "s set is full"); \
    return RCL_RET_WAIT_SET_FULL; \
  } \
  size_t current_index = wait_set->impl->Type ## _index++; \
  wait_set->Type ## s[current_index] = Type; \
  /* Set optional output argument */ \
  if (NULL != index) { \
    *index = current_index; \
  }

#define SET_ADD_RMW(Type, RMWStorage, RMWCount) \
  /* Also place into rmw storage. */ \
  rmw_ ## Type ## _t * rmw_handle = rcl_ ## Type ## _get_rmw_handle(Type); \
  RCL_CHECK_FOR_NULL_WITH_MSG( \
    rmw_handle, rcl_get_error_string().str, return RCL_RET_ERROR); \
  wait_set->impl->RMWStorage[current_index] = rmw_handle->data; \
  wait_set->impl->RMWCount++;

#define SET_CLEAR(Type) \
  do { \
    if (NULL != wait_set->Type ## s) { \
      memset( \
        (void *)wait_set->Type ## s, \
        0, \
        sizeof(rcl_ ## Type ## _t *) * wait_set->size_of_ ## Type ## s); \
      wait_set->impl->Type ## _index = 0; \
    } \
  } while (false)

#define SET_CLEAR_RMW(Type, RMWStorage, RMWCount) \
  do { \
    if (NULL != wait_set->impl->RMWStorage) { \
      /* Also clear the rmw storage. */ \
      memset( \
        wait_set->impl->RMWStorage, \
        0, \
        sizeof(void *) * wait_set->impl->RMWCount); \
      wait_set->impl->RMWCount = 0; \
    } \
  } while (false)

#define SET_RESIZE(Type, ExtraDealloc, ExtraRealloc) \
  do { \
    rcl_allocator_t allocator = wait_set->impl->allocator; \
    wait_set->size_of_ ## Type ## s = 0; \
    wait_set->impl->Type ## _index = 0; \
    if (0 == Type ## s_size) { \
      if (wait_set->Type ## s) { \
        allocator.deallocate((void *)wait_set->Type ## s, allocator.state); \
        wait_set->Type ## s = NULL; \
      } \
      ExtraDealloc \
    } else { \
      wait_set->Type ## s = (const rcl_ ## Type ## _t **)allocator.reallocate( \
        (void *)wait_set->Type ## s, sizeof(rcl_ ## Type ## _t *) * Type ## s_size, \
        allocator.state); \
      RCL_CHECK_FOR_NULL_WITH_MSG( \
        wait_set->Type ## s, "allocating memory failed", return RCL_RET_BAD_ALLOC); \
      memset((void *)wait_set->Type ## s, 0, sizeof(rcl_ ## Type ## _t *) * Type ## s_size); \
      wait_set->size_of_ ## Type ## s = Type ## s_size; \
      ExtraRealloc \
    } \
  } while (false)

#define SET_RESIZE_RMW_DEALLOC(RMWStorage, RMWCount) \
  /* Also deallocate the rmw storage. */ \
  if (wait_set->impl->RMWStorage) { \
    allocator.deallocate((void *)wait_set->impl->RMWStorage, allocator.state); \
    wait_set->impl->RMWStorage = NULL; \
    wait_set->impl->RMWCount = 0; \
  }

#define SET_RESIZE_RMW_REALLOC(Type, RMWStorage, RMWCount) \
  /* Also resize the rmw storage. */ \
  wait_set->impl->RMWCount = 0; \
  wait_set->impl->RMWStorage = (void **)allocator.reallocate( \
    wait_set->impl->RMWStorage, sizeof(void *) * Type ## s_size, allocator.state); \
  if (!wait_set->impl->RMWStorage) { \
    allocator.deallocate((void *)wait_set->Type ## s, allocator.state); \
    wait_set->Type ## s = NULL; \
    wait_set->size_of_ ## Type ## s = 0; \
    RCL_SET_ERROR_MSG("allocating memory failed"); \
    return RCL_RET_BAD_ALLOC; \
  } \
  memset(wait_set->impl->RMWStorage, 0, sizeof(void *) * Type ## s_size);

/* Implementation-specific notes:
 *
 * Add the rmw representation to the underlying rmw array and increment
 * the rmw array count.
 */
rcl_ret_t
rcl_wait_set_add_subscription(
  rcl_wait_set_t * wait_set,
  const rcl_subscription_t * subscription,
  size_t * index)
{
  SET_ADD(subscription)
  SET_ADD_RMW(subscription, rmw_subscriptions.subscribers, rmw_subscriptions.subscriber_count)
  return RCL_RET_OK;
}

/* Implementation-specific notes:
 *
 * Sets all of the entries in the underlying rmw array to null, and sets the
 * count in the rmw array to 0.
 */
rcl_ret_t
rcl_wait_set_clear(rcl_wait_set_t * wait_set)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set->impl, RCL_RET_WAIT_SET_INVALID);

  SET_CLEAR(subscription);
  SET_CLEAR(guard_condition);
  SET_CLEAR(client);
  SET_CLEAR(service);
  SET_CLEAR(event);
  SET_CLEAR(timer);

  SET_CLEAR_RMW(
    subscription,
    rmw_subscriptions.subscribers,
    rmw_subscriptions.subscriber_count);
  SET_CLEAR_RMW(
    guard_condition,
    rmw_guard_conditions.guard_conditions,
    rmw_guard_conditions.guard_condition_count);
  SET_CLEAR_RMW(
    clients,
    rmw_clients.clients,
    rmw_clients.client_count);
  SET_CLEAR_RMW(
    services,
    rmw_services.services,
    rmw_services.service_count);
  SET_CLEAR_RMW(
    events,
    rmw_events.events,
    rmw_events.event_count);

  return RCL_RET_OK;
}

/* Implementation-specific notes:
 *
 * Similarly, the underlying rmw representation is reallocated and reset:
 * all entries are set to null and the count is set to zero.
 */
rcl_ret_t
rcl_wait_set_resize(
  rcl_wait_set_t * wait_set,
  size_t subscriptions_size,
  size_t guard_conditions_size,
  size_t timers_size,
  size_t clients_size,
  size_t services_size,
  size_t events_size)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set->impl, RCL_RET_WAIT_SET_INVALID);
  SET_RESIZE(
    subscription,
    SET_RESIZE_RMW_DEALLOC(
      rmw_subscriptions.subscribers, rmw_subscriptions.subscriber_count),
    SET_RESIZE_RMW_REALLOC(
      subscription, rmw_subscriptions.subscribers, rmw_subscriptions.subscriber_count)
  );
  // Guard condition RCL size is the resize amount given
  SET_RESIZE(guard_condition,;,;);  // NOLINT

  // Guard condition RMW size needs to be guard conditions + timers
  rmw_guard_conditions_t * rmw_gcs = &(wait_set->impl->rmw_guard_conditions);
  const size_t num_rmw_gc = guard_conditions_size + timers_size;
  // Clear added guard conditions
  rmw_gcs->guard_condition_count = 0u;
  if (0u == num_rmw_gc) {
    if (rmw_gcs->guard_conditions) {
      wait_set->impl->allocator.deallocate(
        (void *)rmw_gcs->guard_conditions, wait_set->impl->allocator.state);
      rmw_gcs->guard_conditions = NULL;
    }
  } else {
    rmw_gcs->guard_conditions = (void **)wait_set->impl->allocator.reallocate(
      rmw_gcs->guard_conditions, sizeof(void *) * num_rmw_gc, wait_set->impl->allocator.state);
    if (!rmw_gcs->guard_conditions) {
      // Deallocate rcl arrays to match unallocated rmw guard conditions
      wait_set->impl->allocator.deallocate(
        (void *)wait_set->guard_conditions, wait_set->impl->allocator.state);
      wait_set->size_of_guard_conditions = 0u;
      wait_set->guard_conditions = NULL;
      wait_set->impl->allocator.deallocate(
        (void *)wait_set->timers, wait_set->impl->allocator.state);
      wait_set->size_of_timers = 0u;
      wait_set->timers = NULL;
      RCL_SET_ERROR_MSG("allocating memory failed");
      return RCL_RET_BAD_ALLOC;
    }
    memset(rmw_gcs->guard_conditions, 0, sizeof(void *) * num_rmw_gc);
  }

  SET_RESIZE(timer,;,;);  // NOLINT
  SET_RESIZE(
    client,
    SET_RESIZE_RMW_DEALLOC(
      rmw_clients.clients, rmw_clients.client_count),
    SET_RESIZE_RMW_REALLOC(
      client, rmw_clients.clients, rmw_clients.client_count)
  );
  SET_RESIZE(
    service,
    SET_RESIZE_RMW_DEALLOC(
      rmw_services.services, rmw_services.service_count),
    SET_RESIZE_RMW_REALLOC(
      service, rmw_services.services, rmw_services.service_count)
  );
  SET_RESIZE(
    event,
    SET_RESIZE_RMW_DEALLOC(
      rmw_events.events, rmw_events.event_count),
    SET_RESIZE_RMW_REALLOC(
      event, rmw_events.events, rmw_events.event_count)
  );

  return RCL_RET_OK;
}

rcl_ret_t
rcl_wait_set_add_guard_condition(
  rcl_wait_set_t * wait_set,
  const rcl_guard_condition_t * guard_condition,
  size_t * index)
{
  SET_ADD(guard_condition)
  SET_ADD_RMW(
    guard_condition, rmw_guard_conditions.guard_conditions,
    rmw_guard_conditions.guard_condition_count)

  return RCL_RET_OK;
}

rcl_ret_t
rcl_wait_set_add_timer(
  rcl_wait_set_t * wait_set,
  const rcl_timer_t * timer,
  size_t * index)
{
  SET_ADD(timer)
  // Add timer guard conditions to end of rmw guard condtion set.
  rcl_guard_condition_t * guard_condition = rcl_timer_get_guard_condition(timer);
  if (NULL != guard_condition) {
    // rcl_wait() will take care of moving these backwards and setting guard_condition_count.
    const size_t index = wait_set->size_of_guard_conditions + (wait_set->impl->timer_index - 1);
    rmw_guard_condition_t * rmw_handle = rcl_guard_condition_get_rmw_handle(guard_condition);
    RCL_CHECK_FOR_NULL_WITH_MSG(
      rmw_handle, rcl_get_error_string().str, return RCL_RET_ERROR);
    wait_set->impl->rmw_guard_conditions.guard_conditions[index] = rmw_handle->data;
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_wait_set_add_client(
  rcl_wait_set_t * wait_set,
  const rcl_client_t * client,
  size_t * index)
{
  SET_ADD(client)
  SET_ADD_RMW(client, rmw_clients.clients, rmw_clients.client_count)
  return RCL_RET_OK;
}

rcl_ret_t
rcl_wait_set_add_service(
  rcl_wait_set_t * wait_set,
  const rcl_service_t * service,
  size_t * index)
{
  SET_ADD(service)
  SET_ADD_RMW(service, rmw_services.services, rmw_services.service_count)
  return RCL_RET_OK;
}

rcl_ret_t
rcl_wait_set_add_event(
  rcl_wait_set_t * wait_set,
  const rcl_event_t * event,
  size_t * index)
{
  SET_ADD(event)
  SET_ADD_RMW(event, rmw_events.events, rmw_events.event_count)
  wait_set->impl->rmw_events.events[current_index] = rmw_handle;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_wait(rcl_wait_set_t * wait_set, int64_t timeout)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(wait_set, RCL_RET_INVALID_ARGUMENT);
  if (!rcl_wait_set_is_valid(wait_set)) {
    RCL_SET_ERROR_MSG("wait set is invalid");
    return RCL_RET_WAIT_SET_INVALID;
  }
  if (
    wait_set->size_of_subscriptions == 0 &&
    wait_set->size_of_guard_conditions == 0 &&
    wait_set->size_of_timers == 0 &&
    wait_set->size_of_clients == 0 &&
    wait_set->size_of_services == 0 &&
    wait_set->size_of_events == 0)
  {
    RCL_SET_ERROR_MSG("wait set is empty");
    return RCL_RET_WAIT_SET_EMPTY;
  }
  // Calculate the timeout argument.
  // By default, set the timer to block indefinitely if none of the below conditions are met.
  rmw_time_t * timeout_argument = NULL;
  rmw_time_t temporary_timeout_storage;

  bool is_timer_timeout = false;
  int64_t min_timeout = timeout > 0 ? timeout : INT64_MAX;
  {  // scope to prevent i from colliding below
    uint64_t i = 0;
    for (i = 0; i < wait_set->impl->timer_index; ++i) {
      if (!wait_set->timers[i]) {
        continue;  // Skip NULL timers.
      }
      rmw_guard_conditions_t * rmw_gcs = &(wait_set->impl->rmw_guard_conditions);
      size_t gc_idx = wait_set->size_of_guard_conditions + i;
      if (NULL != rmw_gcs->guard_conditions[gc_idx]) {
        // This timer has a guard condition, so move it to make a legal wait set.
        rmw_gcs->guard_conditions[rmw_gcs->guard_condition_count] =
          rmw_gcs->guard_conditions[gc_idx];
        ++(rmw_gcs->guard_condition_count);
      }
      // use timer time to to set the rmw_wait timeout
      // TODO(sloretz) fix spurious wake-ups on ROS_TIME timers with ROS_TIME enabled
      int64_t timer_timeout = INT64_MAX;
      rcl_ret_t ret = rcl_timer_get_time_until_next_call(wait_set->timers[i], &timer_timeout);
      if (ret == RCL_RET_TIMER_CANCELED) {
        wait_set->timers[i] = NULL;
        continue;
      }
      if (ret != RCL_RET_OK) {
        return ret;  // The rcl error state should already be set.
      }
      if (timer_timeout < min_timeout) {
        is_timer_timeout = true;
        min_timeout = timer_timeout;
      }
    }
  }

  if (timeout == 0) {
    // Then it is non-blocking, so set the temporary storage to 0, 0 and pass it.
    temporary_timeout_storage.sec = 0;
    temporary_timeout_storage.nsec = 0;
    timeout_argument = &temporary_timeout_storage;
  } else if (timeout > 0 || is_timer_timeout) {
    // If min_timeout was negative, we need to wake up immediately.
    if (min_timeout < 0) {
      min_timeout = 0;
    }
    temporary_timeout_storage.sec = RCL_NS_TO_S(min_timeout);
    temporary_timeout_storage.nsec = min_timeout % 1000000000;
    timeout_argument = &temporary_timeout_storage;
  }

  // Wait.
  rmw_ret_t ret = rmw_wait(
    &wait_set->impl->rmw_subscriptions,
    &wait_set->impl->rmw_guard_conditions,
    &wait_set->impl->rmw_services,
    &wait_set->impl->rmw_clients,
    &wait_set->impl->rmw_events,
    wait_set->impl->rmw_wait_set,
    timeout_argument);

  // Items that are not ready will have been set to NULL by rmw_wait.
  // We now update our handles accordingly.

  // Check for ready timers
  // and set not ready timers (which includes canceled timers) to NULL.
  size_t i;
  for (i = 0; i < wait_set->impl->timer_index; ++i) {
    if (!wait_set->timers[i]) {
      continue;
    }
    bool is_ready = false;
    rcl_ret_t ret = rcl_timer_is_ready(wait_set->timers[i], &is_ready);
    if (ret != RCL_RET_OK) {
      return ret;  // The rcl error state should already be set.
    }
    if (!is_ready) {
      wait_set->timers[i] = NULL;
    }
  }
  // Check for timeout, return RCL_RET_TIMEOUT only if it wasn't a timer.
  if (ret != RMW_RET_OK && ret != RMW_RET_TIMEOUT) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return RCL_RET_ERROR;
  }
  // Set corresponding rcl subscription handles NULL.
  for (i = 0; i < wait_set->size_of_subscriptions; ++i) {
    bool is_ready = wait_set->impl->rmw_subscriptions.subscribers[i] != NULL;
    if (!is_ready) {
      wait_set->subscriptions[i] = NULL;
    }
  }
  // Set corresponding rcl guard_condition handles NULL.
  for (i = 0; i < wait_set->size_of_guard_conditions; ++i) {
    bool is_ready = wait_set->impl->rmw_guard_conditions.guard_conditions[i] != NULL;
    if (!is_ready) {
      wait_set->guard_conditions[i] = NULL;
    }
  }
  // Set corresponding rcl client handles NULL.
  for (i = 0; i < wait_set->size_of_clients; ++i) {
    bool is_ready = wait_set->impl->rmw_clients.clients[i] != NULL;
    if (!is_ready) {
      wait_set->clients[i] = NULL;
    }
  }
  // Set corresponding rcl service handles NULL.
  for (i = 0; i < wait_set->size_of_services; ++i) {
    bool is_ready = wait_set->impl->rmw_services.services[i] != NULL;
    if (!is_ready) {
      wait_set->services[i] = NULL;
    }
  }
  // Set corresponding rcl event handles NULL.
  for (i = 0; i < wait_set->size_of_events; ++i) {
    bool is_ready = wait_set->impl->rmw_events.events[i] != NULL;
    if (!is_ready) {
      wait_set->events[i] = NULL;
    }
  }

  if (RMW_RET_TIMEOUT == ret && !is_timer_timeout) {
    return RCL_RET_TIMEOUT;
  }
  return RCL_RET_OK;
}

#ifdef __cplusplus
}
#endif
