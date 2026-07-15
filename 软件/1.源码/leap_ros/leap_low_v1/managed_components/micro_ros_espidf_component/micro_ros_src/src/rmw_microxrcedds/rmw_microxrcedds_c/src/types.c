// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <rmw_microros_internal/types.h>

#ifdef HAVE_C_TYPESUPPORT
#include <rosidl_typesupport_microxrcedds_c/identifier.h>
#endif /* ifdef HAVE_C_TYPESUPPORT */
#ifdef HAVE_CPP_TYPESUPPORT
#include <rosidl_typesupport_microxrcedds_cpp/identifier.h>
#endif /* ifdef HAVE_CPP_TYPESUPPORT */

#include <rmw/allocators.h>
#include <uxr/client/profile/multithread/multithread.h>
#include <rmw_microxrcedds_c/rmw_c_macros.h>

#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/memory.h"
#include "./rmw_microros_internal/rmw_microxrcedds_topic.h"

// Static memory pools

char rmw_uxrce_entity_naming_buffer[RMW_UXRCE_ENTITY_NAMING_BUFFER_LENGTH];

rmw_uxrce_mempool_t session_memory;
rmw_context_impl_t custom_sessions[RMW_UXRCE_MAX_SESSIONS];

rmw_uxrce_mempool_t node_memory;
rmw_uxrce_node_t custom_nodes[RMW_UXRCE_MAX_NODES];

rmw_uxrce_mempool_t publisher_memory;
rmw_uxrce_publisher_t custom_publishers[RMW_UXRCE_MAX_PUBLISHERS];

rmw_uxrce_mempool_t subscription_memory;
rmw_uxrce_subscription_t custom_subscriptions[RMW_UXRCE_MAX_SUBSCRIPTIONS];

rmw_uxrce_mempool_t service_memory;
rmw_uxrce_service_t custom_services[RMW_UXRCE_MAX_SERVICES];

rmw_uxrce_mempool_t client_memory;
rmw_uxrce_client_t custom_clients[RMW_UXRCE_MAX_CLIENTS];

rmw_uxrce_mempool_t topics_memory;
rmw_uxrce_topic_t custom_topics[RMW_UXRCE_MAX_TOPICS_INTERNAL];

rmw_uxrce_mempool_t static_buffer_memory;
rmw_uxrce_static_input_buffer_t custom_static_buffers[RMW_UXRCE_MAX_HISTORY];

rmw_uxrce_mempool_t init_options_memory;
rmw_uxrce_init_options_impl_t custom_init_options[RMW_UXRCE_MAX_OPTIONS];

rmw_uxrce_mempool_t wait_set_memory;
rmw_uxrce_wait_set_t custom_wait_set[RMW_UXRCE_MAX_WAIT_SETS];

rmw_uxrce_mempool_t guard_condition_memory;
rmw_uxrce_guard_condition_t custom_guard_condition[RMW_UXRCE_MAX_GUARD_CONDITION];

// Global mutexs
#ifdef UCLIENT_PROFILE_MULTITHREAD
uxrMutex rmw_uxrce_wait_mutex;
bool rmw_uxrce_wait_mutex_initialized = false;
#endif  // UCLIENT_PROFILE_MULTITHREAD

// Memory init functions

#define RMW_INIT_MEMORY(X) \
  void rmw_uxrce_init_ ## X ## _memory( \
    rmw_uxrce_mempool_t * memory, \
    rmw_uxrce_ ## X ## _t * array, \
    size_t size) \
  { \
    if (size > 0 && !memory->is_initialized) { \
      UXR_INIT_LOCK(&memory->mutex); \
      memory->is_initialized = true; \
      memory->element_size = sizeof(*array); \
      memory->allocateditems = NULL; \
      memory->freeitems = NULL; \
      memory->is_dynamic_allowed = true; \
 \
      for (size_t i = 0; i < size; i++) { \
        put_memory(memory, &array[i].mem); \
        array[i].mem.data = (void *)&array[i]; \
        array[i].mem.is_dynamic_memory = false; \
      } \
    } \
  }


RMW_INIT_MEMORY(service)
RMW_INIT_MEMORY(client)
RMW_INIT_MEMORY(publisher)
RMW_INIT_MEMORY(subscription)
RMW_INIT_MEMORY(node)
RMW_INIT_MEMORY(session)
RMW_INIT_MEMORY(topic)
RMW_INIT_MEMORY(static_input_buffer)
RMW_INIT_MEMORY(init_options_impl)
RMW_INIT_MEMORY(wait_set)
RMW_INIT_MEMORY(guard_condition)

// Memory management functions

void rmw_uxrce_fini_session_memory(
  rmw_context_impl_t * session)
{
  put_memory(&session_memory, &session->mem);
}

void rmw_uxrce_fini_node_memory(
  rmw_node_t * node)
{
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(node->implementation_identifier, );

  if (node->implementation_identifier) {
    node->implementation_identifier = NULL;
  }
  if (node->data) {
    rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)node->data;
    custom_node->context = NULL;

    put_memory(&node_memory, &custom_node->mem);

    node->data = NULL;
  }

  node = NULL;
}

void rmw_uxrce_fini_publisher_memory(
  rmw_publisher_t * publisher)
{
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(publisher->implementation_identifier, );

  if (publisher->implementation_identifier) {
    publisher->implementation_identifier = NULL;
  }
  if (publisher->data) {
    rmw_uxrce_publisher_t * custom_publisher = (rmw_uxrce_publisher_t *)publisher->data;

    put_memory(&publisher_memory, &custom_publisher->mem);
    publisher->data = NULL;
  }

  publisher = NULL;
}

void rmw_uxrce_fini_subscription_memory(
  rmw_subscription_t * subscriber)
{
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(subscriber->implementation_identifier, );

  if (subscriber->implementation_identifier) {
    subscriber->implementation_identifier = NULL;
  }
  if (subscriber->data) {
    rmw_uxrce_subscription_t * custom_subscription = (rmw_uxrce_subscription_t *)subscriber->data;

    put_memory(&subscription_memory, &custom_subscription->mem);
    subscriber->data = NULL;
  }

  subscriber = NULL;
}

void rmw_uxrce_fini_service_memory(
  rmw_service_t * service)
{
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(service->implementation_identifier, );

  if (service->implementation_identifier) {
    service->implementation_identifier = NULL;
  }
  if (service->data) {
    rmw_uxrce_service_t * custom_service = (rmw_uxrce_service_t *)service->data;

    put_memory(&service_memory, &custom_service->mem);
    service->data = NULL;
  }

  service = NULL;
}

void rmw_uxrce_fini_client_memory(
  rmw_client_t * client)
{
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(client->implementation_identifier, );

  if (client->implementation_identifier) {
    client->implementation_identifier = NULL;
  }
  if (client->data) {
    rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)client->data;

    put_memory(&client_memory, &custom_client->mem);
    client->data = NULL;
  }

  client = NULL;
}

void rmw_uxrce_fini_topic_memory(
  rmw_uxrce_topic_t * topic)
{
  put_memory(&topics_memory, &topic->mem);
  topic->owner_node = NULL;
}

size_t rmw_uxrce_count_static_input_buffer_for_entity(
  void * entity)
{
  size_t count = 0;

  UXR_LOCK(&static_buffer_memory.mutex);
  rmw_uxrce_mempool_item_t * item = static_buffer_memory.allocateditems;

  while (item != NULL) {
    rmw_uxrce_static_input_buffer_t * data = (rmw_uxrce_static_input_buffer_t *)item->data;
    if (data->owner == entity) {
      count++;
    }
    item = item->next;
  }
  UXR_UNLOCK(&static_buffer_memory.mutex);

  return count;
}

rmw_uxrce_mempool_item_t * rmw_uxrce_get_static_input_buffer_for_entity(
  void * entity,
  const rmw_qos_profile_t qos)
{
  rmw_uxrce_mempool_item_t * ret = NULL;

  UXR_LOCK(&static_buffer_memory.mutex);
  size_t count = rmw_uxrce_count_static_input_buffer_for_entity(entity);
  switch (qos.history) {
    case RMW_QOS_POLICY_HISTORY_UNKNOWN:
    case RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT:
    case RMW_QOS_POLICY_HISTORY_KEEP_LAST:
      if (qos.depth == 0 || count < qos.depth) {
        ret = get_memory(&static_buffer_memory);
      } else {
        ret = rmw_uxrce_find_static_input_buffer_by_owner(entity);
      }
      break;
    case RMW_QOS_POLICY_HISTORY_KEEP_ALL:
      if (qos.depth == 0 || count < qos.depth) {
        ret = get_memory(&static_buffer_memory);
      } else {
        // There aren't more slots for this entity
      }
      break;
    default:
      break;
  }
  UXR_UNLOCK(&static_buffer_memory.mutex);

  return ret;
}

rmw_uxrce_mempool_item_t * rmw_uxrce_find_static_input_buffer_by_owner(
  void * owner)
{
  rmw_uxrce_mempool_item_t * ret = NULL;
  int64_t min_time = INT64_MAX;

  UXR_LOCK(&static_buffer_memory.mutex);

  // Return the oldest
  rmw_uxrce_mempool_item_t * static_buffer_item = static_buffer_memory.allocateditems;
  while (static_buffer_item != NULL) {
    rmw_uxrce_static_input_buffer_t * data =
      (rmw_uxrce_static_input_buffer_t *)static_buffer_item->data;

    if (data->owner == owner && data->timestamp < min_time) {
      ret = static_buffer_item;
      min_time = data->timestamp;
    }

    static_buffer_item = static_buffer_item->next;
  }
  UXR_UNLOCK(&static_buffer_memory.mutex);

  return ret;
}

void rmw_uxrce_clean_expired_static_input_buffer(void)
{
  UXR_LOCK(&static_buffer_memory.mutex);

  rmw_uxrce_mempool_item_t * static_buffer_item = static_buffer_memory.allocateditems;
  int64_t now_ns = rmw_uros_epoch_nanos();

  while (static_buffer_item != NULL) {
    rmw_uxrce_static_input_buffer_t * data =
      (rmw_uxrce_static_input_buffer_t *)static_buffer_item->data;
    rmw_time_t lifespan;
    switch (data->entity_type) {
      case RMW_UXRCE_ENTITY_TYPE_SUBSCRIPTION:
        lifespan = ((rmw_uxrce_subscription_t *)data->owner)->qos.lifespan;
        break;
      case RMW_UXRCE_ENTITY_TYPE_CLIENT:
        lifespan = ((rmw_uxrce_client_t *)data->owner)->qos.lifespan;
        break;
      case RMW_UXRCE_ENTITY_TYPE_SERVICE:
        lifespan = ((rmw_uxrce_service_t *)data->owner)->qos.lifespan;
        break;
      default:
        // Not recognized, clean this buffer as soon as possible
        lifespan = (rmw_time_t) {0LL, 1LL};
        break;
    }

    if (rmw_time_equal(lifespan, (rmw_time_t)RMW_DURATION_UNSPECIFIED)) {
      lifespan = (rmw_time_t) RMW_UXRCE_QOS_LIFESPAN_DEFAULT;
    }

    rmw_uxrce_mempool_item_t * aux_next = static_buffer_item->next;

    int64_t expiration_time = data->timestamp + rmw_time_total_nsec(lifespan);
    if (expiration_time < now_ns || data->timestamp > now_ns) {
      put_memory(&static_buffer_memory, static_buffer_item);
    }

    static_buffer_item = aux_next;
  }
  UXR_UNLOCK(&static_buffer_memory.mutex);
}
