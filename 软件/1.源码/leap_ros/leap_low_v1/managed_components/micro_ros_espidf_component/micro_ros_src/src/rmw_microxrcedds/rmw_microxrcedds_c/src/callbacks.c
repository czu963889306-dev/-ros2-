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

#include <rmw_microros_internal/callbacks.h>
#include "./rmw_microros_internal/error_handling_internal.h"

void on_status(
  struct uxrSession * session,
  uxrObjectId object_id,
  uint16_t request_id,
  uint8_t status,
  void * args)
{
  (void)session;
  (void)object_id;
  (void)request_id;
  (void)status;
  (void)args;
}

void on_topic(
  struct uxrSession * session,
  uxrObjectId object_id,
  uint16_t request_id,
  uxrStreamId stream_id,
  struct ucdrBuffer * ub,
  uint16_t length,
  void * args)
{
  (void)session;
  (void)request_id;
  (void)stream_id;

#ifdef RMW_UXRCE_GRAPH
  rmw_context_impl_t * context_impl = (rmw_context_impl_t *)(args);
  rmw_graph_info_t * graph_info = &context_impl->graph_info;

  if (object_id.id == graph_info->datareader_id.id &&
    object_id.type == graph_info->datareader_id.type)
  {
    graph_info->micro_buffer_length = (size_t)length;
    ucdr_deserialize_array_uint8_t(ub, graph_info->micro_buffer, length);
    graph_info->initialized = true;
    graph_info->has_changed = true;
    return;
  }
#else
  (void)args;
#endif  // RMW_UXRCE_GRAPH

  // Iterate along the allocated subscriptions
  rmw_uxrce_mempool_item_t * subscription_item = subscription_memory.allocateditems;
  while (subscription_item != NULL) {
    rmw_uxrce_subscription_t * custom_subscription =
      (rmw_uxrce_subscription_t *)subscription_item->data;

    // Check if topic is related to the subscription
    if ((custom_subscription->datareader_id.id == object_id.id) &&
      (custom_subscription->datareader_id.type == object_id.type))
    {
      UXR_LOCK(&static_buffer_memory.mutex);

      rmw_uxrce_mempool_item_t * memory_node = rmw_uxrce_get_static_input_buffer_for_entity(
        custom_subscription, custom_subscription->qos);
      if (!memory_node) {
        UXR_UNLOCK(&static_buffer_memory.mutex);
        RMW_UROS_TRACE_ERROR(
          RMW_UROS_ERROR_ON_SUBSCRIPTION, RMW_UROS_ERROR_MIDDLEWARE_ALLOCATION,
          "Not available static buffer memory node in on_topic callback",
          .node = custom_subscription->owner_node->node_name,
          .node_namespace = custom_subscription->owner_node->node_namespace,
          .topic_name = custom_subscription->topic_name, .ucdr = ub,
          .size = length,
          .type_support.message_callbacks = custom_subscription->type_support_callbacks);
        return;
      }

      rmw_uxrce_static_input_buffer_t * static_buffer =
        (rmw_uxrce_static_input_buffer_t *)memory_node->data;

      if (!ucdr_deserialize_array_uint8_t(
          ub,
          static_buffer->buffer,
          length))
      {
        put_memory(&static_buffer_memory, memory_node);
      } else {
        static_buffer->owner = (void *) custom_subscription;
        static_buffer->length = length;
        static_buffer->timestamp = rmw_uros_epoch_nanos();
        static_buffer->entity_type = RMW_UXRCE_ENTITY_TYPE_SUBSCRIPTION;
      }

      UXR_UNLOCK(&static_buffer_memory.mutex);
      return;
    }
    subscription_item = subscription_item->next;
  }
}

void on_request(
  struct uxrSession * session,
  uxrObjectId object_id,
  uint16_t request_id,
  SampleIdentity * sample_id,
  struct ucdrBuffer * ub,
  uint16_t length,
  void * args)
{
  (void)session;
  (void)object_id;
  (void)args;

  // Iterate along the allocated services
  rmw_uxrce_mempool_item_t * service_item = service_memory.allocateditems;
  while (service_item != NULL) {
    // Check if request is related to the service
    rmw_uxrce_service_t * custom_service = (rmw_uxrce_service_t *)service_item->data;
    if (custom_service->service_data_resquest == request_id) {
      UXR_LOCK(&static_buffer_memory.mutex);

      rmw_uxrce_mempool_item_t * memory_node = rmw_uxrce_get_static_input_buffer_for_entity(
        custom_service, custom_service->qos);
      if (!memory_node) {
        UXR_UNLOCK(&static_buffer_memory.mutex);
        RMW_UROS_TRACE_ERROR(
          RMW_UROS_ERROR_ON_SERVICE, RMW_UROS_ERROR_MIDDLEWARE_ALLOCATION,
          "Not available static buffer memory node in on_request callback",
          .node = custom_service->owner_node->node_name,
          .node_namespace = custom_service->owner_node->node_namespace,
          .topic_name = custom_service->service_name, .ucdr = ub,
          .size = length,
          .type_support.service_callbacks = custom_service->type_support_callbacks);
        return;
      }

      rmw_uxrce_static_input_buffer_t * static_buffer =
        (rmw_uxrce_static_input_buffer_t *)memory_node->data;

      if (!ucdr_deserialize_array_uint8_t(
          ub,
          static_buffer->buffer,
          length))
      {
        put_memory(&static_buffer_memory, memory_node);
      } else {
        static_buffer->owner = (void *) custom_service;
        static_buffer->length = length;
        static_buffer->related.sample_id = *sample_id;
        static_buffer->timestamp = rmw_uros_epoch_nanos();
        static_buffer->entity_type = RMW_UXRCE_ENTITY_TYPE_SERVICE;
      }

      UXR_UNLOCK(&static_buffer_memory.mutex);

      return;
    }
    service_item = service_item->next;
  }
}

void on_reply(
  struct uxrSession * session,
  uxrObjectId object_id,
  uint16_t request_id,
  uint16_t reply_id,
  struct ucdrBuffer * ub,
  uint16_t length,
  void * args)
{
  (void)session;
  (void)object_id;
  (void)args;

  // Iterate along the allocated clients
  rmw_uxrce_mempool_item_t * client_item = client_memory.allocateditems;
  while (client_item != NULL) {
    // Check if reply is related to the client
    rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)client_item->data;
    if (custom_client->client_data_request == request_id) {
      UXR_LOCK(&static_buffer_memory.mutex);

      rmw_uxrce_mempool_item_t * memory_node = rmw_uxrce_get_static_input_buffer_for_entity(
        custom_client, custom_client->qos);
      if (!memory_node) {
        UXR_UNLOCK(&static_buffer_memory.mutex);
        RMW_UROS_TRACE_ERROR(
          RMW_UROS_ERROR_ON_CLIENT, RMW_UROS_ERROR_MIDDLEWARE_ALLOCATION,
          "Not available static buffer memory node in on_reply callback",
          .node = custom_client->owner_node->node_name,
          .node_namespace = custom_client->owner_node->node_namespace,
          .topic_name = custom_client->service_name, .ucdr = ub,
          .size = length,
          .type_support.service_callbacks = custom_client->type_support_callbacks);
        return;
      }

      rmw_uxrce_static_input_buffer_t * static_buffer =
        (rmw_uxrce_static_input_buffer_t *)memory_node->data;

      if (!ucdr_deserialize_array_uint8_t(
          ub,
          static_buffer->buffer,
          length))
      {
        put_memory(&static_buffer_memory, memory_node);
      } else {
        static_buffer->owner = (void *) custom_client;
        static_buffer->length = length;
        static_buffer->related.reply_id = reply_id;
        static_buffer->timestamp = rmw_uros_epoch_nanos();
        static_buffer->entity_type = RMW_UXRCE_ENTITY_TYPE_CLIENT;
      }
      UXR_UNLOCK(&static_buffer_memory.mutex);

      return;
    }
    client_item = client_item->next;
  }
}
