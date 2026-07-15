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

#include <rmw_microros_internal/rmw_node.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microxrcedds_c/rmw_c_macros.h>
#include <rmw/allocators.h>
#include <rmw/rmw.h>

#include "./rmw_microros_internal/types.h"
#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/identifiers.h"
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_node_t * create_node(
  const char * name,
  const char * namespace_,
  size_t domain_id,
  const rmw_context_t * context)
{
  rmw_node_t * node_handle = NULL;

  if (!context) {
    RMW_UROS_TRACE_MESSAGE("context is null");
    return NULL;
  }

  rmw_uxrce_mempool_item_t * memory_node = get_memory(&node_memory);
  if (!memory_node) {
    RMW_UROS_TRACE_MESSAGE("Not available memory node")
    return NULL;
  }

  rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)memory_node->data;

  custom_node->context = context->impl;

  node_handle = &custom_node->rmw_node;

  node_handle->implementation_identifier = rmw_get_implementation_identifier();
  node_handle->data = custom_node;
  node_handle->name = custom_node->node_name;

  if ((strlen(name) + 1 ) > sizeof(custom_node->node_name)) {
    RMW_UROS_TRACE_MESSAGE("failed to allocate string")
    goto fail;
  }

  memcpy((char *)node_handle->name, name, strlen(name) + 1);

  node_handle->namespace_ = custom_node->node_namespace;

  if ((strlen(namespace_) + 1 ) > sizeof(custom_node->node_namespace)) {
    RMW_UROS_TRACE_MESSAGE("failed to allocate string")
    goto fail;
  }

  memcpy((char *)node_handle->namespace_, namespace_, strlen(namespace_) + 1);

  custom_node->participant_id =
    uxr_object_id(custom_node->context->id_participant++, UXR_PARTICIPANT_ID);
  uint16_t participant_req = UXR_INVALID_REQUEST_ID;

#ifdef RMW_UXRCE_USE_REFS
  if (!build_participant_profile(
      rmw_uxrce_entity_naming_buffer,
      sizeof(rmw_uxrce_entity_naming_buffer)))
  {
    RMW_UROS_TRACE_MESSAGE("failed to generate xml request for node creation")
    return NULL;
  }
  participant_req = uxr_buffer_create_participant_ref(
    &custom_node->context->session,
    *custom_node->context->creation_stream,
    custom_node->participant_id,
    (uint16_t)domain_id,
    rmw_uxrce_entity_naming_buffer, UXR_REPLACE | UXR_REUSE);
#else
  static char xrce_node_name[RMW_UXRCE_NODE_NAME_MAX_LENGTH];

  if (strcmp(namespace_, "/") == 0) {
    snprintf(xrce_node_name, RMW_UXRCE_NODE_NAME_MAX_LENGTH, "%s", name);
  } else {
    snprintf(xrce_node_name, RMW_UXRCE_NODE_NAME_MAX_LENGTH, "%s/%s", namespace_, name);
  }

  participant_req = uxr_buffer_create_participant_bin(
    &custom_node->context->session,
    *custom_node->context->creation_stream,
    custom_node->participant_id,
    domain_id,
    xrce_node_name,
    UXR_REPLACE | UXR_REUSE);
#endif /* ifdef RMW_UXRCE_USE_REFS */

  if (!run_xrce_session(
      custom_node->context, custom_node->context->creation_stream, participant_req,
      custom_node->context->creation_timeout))
  {
    rmw_uxrce_fini_node_memory(node_handle);
    return NULL;
  }

  return node_handle;

fail:
  if (node_handle != NULL) {
    rmw_uxrce_fini_node_memory(node_handle);
  }
  node_handle = NULL;
  return node_handle;
}

rmw_node_t *
rmw_create_node(
  rmw_context_t * context,
  const char * name,
  const char * namespace_)
{
  (void)context;

  rmw_node_t * rmw_node = NULL;
  if (!name || strlen(name) == 0) {
    RMW_UROS_TRACE_MESSAGE("name is null");
  } else if (!namespace_ || strlen(namespace_) == 0) {
    RMW_UROS_TRACE_MESSAGE("namespace is null");
  } else {
    rmw_node = create_node(name, namespace_, context->actual_domain_id, context);
  }
  return rmw_node;
}

rmw_ret_t rmw_destroy_node(
  rmw_node_t * node)
{
  rmw_ret_t ret = RMW_RET_OK;
  if (!node) {
    RMW_UROS_TRACE_MESSAGE("node handle is null")
    return RMW_RET_ERROR;
  }

  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(node->implementation_identifier, RMW_RET_ERROR);

  if (!node->data) {
    RMW_UROS_TRACE_MESSAGE("node impl is null")
    return RMW_RET_ERROR;
  }

  rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)node->data;
  // TODO(Pablo) make sure that other entities are removed from the pools

  rmw_uxrce_mempool_item_t * item = NULL;

  item = publisher_memory.allocateditems;
  while (item != NULL) {
    rmw_uxrce_publisher_t * custom_publisher = (rmw_uxrce_publisher_t *)item->data;
    item = item->next;
    if (custom_publisher->owner_node == custom_node) {
      ret = rmw_destroy_publisher(node, &custom_publisher->rmw_publisher);

      // We should not early return on a RMW_RET_TIMEOUT, as it may be an expected output
      if (RMW_RET_ERROR == ret) {
        return ret;
      }
    }
  }

  item = subscription_memory.allocateditems;
  while (item != NULL) {
    rmw_uxrce_subscription_t * custom_subscription = (rmw_uxrce_subscription_t *)item->data;
    item = item->next;
    if (custom_subscription->owner_node == custom_node) {
      ret = rmw_destroy_subscription(node, &custom_subscription->rmw_subscription);

      if (RMW_RET_ERROR == ret) {
        return ret;
      }
    }
  }

  item = service_memory.allocateditems;
  while (item != NULL) {
    rmw_uxrce_service_t * custom_service = (rmw_uxrce_service_t *)item->data;
    item = item->next;
    if (custom_service->owner_node == custom_node) {
      ret = rmw_destroy_service(node, &custom_service->rmw_service);

      if (RMW_RET_ERROR == ret) {
        return ret;
      }
    }
  }

  item = client_memory.allocateditems;
  while (item != NULL) {
    rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)item->data;
    item = item->next;
    if (custom_client->owner_node == custom_node) {
      ret = rmw_destroy_client(node, &custom_client->rmw_client);

      if (RMW_RET_ERROR == ret) {
        return ret;
      }
    }
  }

  uint16_t delete_participant = uxr_buffer_delete_entity(
    &custom_node->context->session,
    *custom_node->context->destroy_stream,
    custom_node->participant_id);

  if (!run_xrce_session(
      custom_node->context, custom_node->context->destroy_stream, delete_participant,
      custom_node->context->destroy_timeout))
  {
    ret = RMW_RET_TIMEOUT;
  }

  rmw_uxrce_fini_node_memory(node);

  return ret;
}

rmw_ret_t
rmw_node_assert_liveliness(
  const rmw_node_t * node)
{
  (void)node;
  RMW_UROS_TRACE_MESSAGE("function not implemented")
  return RMW_RET_UNSUPPORTED;
}

const rmw_guard_condition_t *
rmw_node_get_graph_guard_condition(
  const rmw_node_t * node)
{
  rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)node->data;
  rmw_context_impl_t * context = custom_node->context;
  rmw_guard_condition_t * graph_guard_condition =
    &context->graph_guard_condition;

#ifdef RMW_UXRCE_GRAPH
  if (NULL == graph_guard_condition->data) {
    graph_guard_condition->data = (void *)(&context->graph_info.has_changed);
  }
#endif  // RMW_UXRCE_GRAPH

  return graph_guard_condition;
}
