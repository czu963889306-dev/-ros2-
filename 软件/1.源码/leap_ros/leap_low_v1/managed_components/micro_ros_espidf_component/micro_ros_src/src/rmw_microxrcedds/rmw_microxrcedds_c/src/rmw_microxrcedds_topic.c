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

#include <rmw_microros_internal/rmw_microxrcedds_topic.h>

#include <string.h>

#include <rmw/allocators.h>

#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_uxrce_topic_t *
create_topic(
  struct rmw_uxrce_node_t * custom_node,
  const char * topic_name,
  const message_type_support_callbacks_t * message_type_support_callbacks,
  const rmw_qos_profile_t * qos_policies)
{
  (void) qos_policies;

  rmw_uxrce_topic_t * custom_topic = NULL;
  rmw_uxrce_mempool_item_t * memory_node = get_memory(&topics_memory);

  if (!memory_node) {
    RMW_UROS_TRACE_MESSAGE("Not available memory node");
    goto fail;
  }

  custom_topic = (rmw_uxrce_topic_t *)memory_node->data;

  // Init
  custom_topic->owner_node = custom_node;

  // Asociate to typesupport
  custom_topic->message_type_support_callbacks = message_type_support_callbacks;

  // Generate topic id
  custom_topic->topic_id = uxr_object_id(custom_node->context->id_topic++, UXR_TOPIC_ID);

  // Generate request
  uint16_t topic_req = 0;
#ifdef RMW_UXRCE_USE_REFS
  (void)qos_policies;
  if (!build_topic_profile(
      topic_name, rmw_uxrce_entity_naming_buffer,
      sizeof(rmw_uxrce_entity_naming_buffer)))
  {
    RMW_UROS_TRACE_MESSAGE("failed to generate xml request for node creation")
    goto fail;
  }

  topic_req = uxr_buffer_create_topic_ref(
    &custom_node->context->session,
    *custom_node->context->creation_stream, custom_topic->topic_id,
    custom_node->participant_id, rmw_uxrce_entity_naming_buffer, UXR_REPLACE | UXR_REUSE);
#else
  static char full_topic_name[RMW_UXRCE_TOPIC_NAME_MAX_LENGTH];
  static char type_name[RMW_UXRCE_TYPE_NAME_MAX_LENGTH];

  if (!generate_topic_name(topic_name, full_topic_name, sizeof(full_topic_name))) {
    RMW_UROS_TRACE_MESSAGE("Error creating topic name");
    goto fail;
  }

  if (!generate_type_name(message_type_support_callbacks, type_name, sizeof(type_name))) {
    RMW_UROS_TRACE_MESSAGE("Error creating type name");
    goto fail;
  }

  topic_req = uxr_buffer_create_topic_bin(
    &custom_node->context->session,
    *custom_node->context->creation_stream,
    custom_topic->topic_id,
    custom_node->participant_id,
    full_topic_name,
    type_name,
    UXR_REPLACE | UXR_REUSE);
#endif /* ifdef RMW_UXRCE_USE_XML */

  if (!run_xrce_session(
      custom_node->context, custom_node->context->creation_stream, topic_req,
      custom_node->context->creation_timeout))
  {
    goto fail;
  }

  return custom_topic;

fail:
  if (custom_topic != NULL) {
    rmw_uxrce_fini_topic_memory(custom_topic);
  }

  custom_topic = NULL;
  return custom_topic;
}

rmw_ret_t destroy_topic(
  rmw_uxrce_topic_t * topic)
{
  rmw_ret_t result_ret = RMW_RET_OK;
  if (topic->owner_node != NULL) {
    rmw_uxrce_node_t * custom_node = topic->owner_node;

    uint16_t delete_topic = uxr_buffer_delete_entity(
      &custom_node->context->session,
      *custom_node->context->destroy_stream,
      topic->topic_id);

    if (!run_xrce_session(
        custom_node->context, custom_node->context->destroy_stream, delete_topic,
        custom_node->context->destroy_timeout))
    {
      result_ret = RMW_RET_TIMEOUT;
    }
    rmw_uxrce_fini_topic_memory(topic);
  } else {
    result_ret = RMW_RET_ERROR;
  }

  return result_ret;
}

size_t topic_count(
  rmw_uxrce_node_t * custom_node)
{
  size_t count = 0;
  rmw_uxrce_mempool_item_t * item = NULL;

  item = publisher_memory.allocateditems;
  while (item != NULL) {
    rmw_uxrce_publisher_t * custom_publisher = (rmw_uxrce_publisher_t *)item->data;
    item = item->next;
    if (custom_publisher->owner_node == custom_node && custom_publisher->topic != NULL) {
      count++;
    }
  }

  item = subscription_memory.allocateditems;
  while (item != NULL) {
    rmw_uxrce_subscription_t * custom_subscription = (rmw_uxrce_subscription_t *)item->data;
    item = item->next;
    if (custom_subscription->owner_node == custom_node && custom_subscription->topic != NULL) {
      count++;
    }
  }

  return count;
}
