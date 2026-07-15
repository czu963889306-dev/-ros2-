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

#include <time.h>

#include <rmw_microxrcedds_c/rmw_c_macros.h>
#include <rmw_microros/rmw_microros.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw/rmw.h>
#include <rmw/allocators.h>
#include <uxr/client/util/time.h>

#include "rcutils/strdup.h"

#include "./rmw_microros_internal/callbacks.h"
#include "./rmw_microros_internal/types.h"
#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/rmw_node.h"
#include "./rmw_microros_internal/identifiers.h"
#include "./rmw_microros_internal/rmw_uxrce_transports.h"
#include "./rmw_microros_internal/error_handling_internal.h"

#ifdef RMW_UXRCE_GRAPH
#include "./rmw_microros_internal/rmw_graph.h"
#endif  // RMW_UXRCE_GRAPH

extern rmw_uxrce_transport_params_t rmw_uxrce_transport_default_params;

rmw_ret_t
rmw_init_options_init(
  rmw_init_options_t * init_options,
  rcutils_allocator_t allocator)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(init_options, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(&allocator, return RMW_RET_INVALID_ARGUMENT);

  if (NULL != init_options->implementation_identifier) {
    RMW_UROS_TRACE_MESSAGE("expected zero-initialized init_options")

    return RMW_RET_INVALID_ARGUMENT;
  }

  init_options->instance_id = 0;
  init_options->implementation_identifier = eprosima_microxrcedds_identifier;
  init_options->allocator = allocator;
  init_options->enclave = NULL;
  init_options->domain_id = 0;
  init_options->security_options = rmw_get_default_security_options();
  init_options->localhost_only = RMW_LOCALHOST_ONLY_DEFAULT;

  // This can be call before rmw_init()
  rmw_uxrce_init_init_options_impl_memory(
    &init_options_memory, custom_init_options,
    RMW_UXRCE_MAX_OPTIONS);

  rmw_uxrce_mempool_item_t * memory_node = get_memory(&init_options_memory);
  if (!memory_node) {
    RMW_UROS_TRACE_MESSAGE("Not available memory node")
    return RMW_RET_ERROR;
  }
  init_options->impl = memory_node->data;

#if defined(RMW_UXRCE_TRANSPORT_SERIAL)
  if (strlen(RMW_UXRCE_DEFAULT_SERIAL_DEVICE) <= MAX_SERIAL_DEVICE) {
    snprintf(
      init_options->impl->transport_params.serial_device,
      MAX_SERIAL_DEVICE,
      "%s",
      RMW_UXRCE_DEFAULT_SERIAL_DEVICE);
  } else {
    RMW_UROS_TRACE_MESSAGE("default serial port configuration overflow")
    return RMW_RET_INVALID_ARGUMENT;
  }
#elif defined(RMW_UXRCE_TRANSPORT_UDP) || defined(RMW_UXRCE_TRANSPORT_TCP)
  if (strlen(RMW_UXRCE_DEFAULT_IP) <= MAX_IP_LEN) {
    snprintf(
      init_options->impl->transport_params.agent_address,
      MAX_IP_LEN,
      "%s",
      RMW_UXRCE_DEFAULT_IP);
  } else {
    RMW_UROS_TRACE_MESSAGE("default ip configuration overflow")
    return RMW_RET_INVALID_ARGUMENT;
  }

  if (strlen(RMW_UXRCE_DEFAULT_PORT) < MAX_PORT_LEN) {
    snprintf(
      init_options->impl->transport_params.agent_port,
      MAX_PORT_LEN, "%s", RMW_UXRCE_DEFAULT_PORT);
  } else {
    RMW_UROS_TRACE_MESSAGE("default port configuration overflow")
    return RMW_RET_INVALID_ARGUMENT;
  }
#elif defined(RMW_UXRCE_TRANSPORT_CUSTOM)
  init_options->impl->transport_params.framing = rmw_uxrce_transport_default_params.framing;
  init_options->impl->transport_params.args = rmw_uxrce_transport_default_params.args;
  init_options->impl->transport_params.open_cb = rmw_uxrce_transport_default_params.open_cb;
  init_options->impl->transport_params.close_cb = rmw_uxrce_transport_default_params.close_cb;
  init_options->impl->transport_params.write_cb = rmw_uxrce_transport_default_params.write_cb;
  init_options->impl->transport_params.read_cb = rmw_uxrce_transport_default_params.read_cb;
#endif /* if defined(RMW_UXRCE_TRANSPORT_SERIAL) */

  srand(uxr_nanos());

  do {
    init_options->impl->transport_params.client_key = rand(); //NOLINT
  } while (init_options->impl->transport_params.client_key == 0);

  return RMW_RET_OK;
}

rmw_ret_t
rmw_init_options_copy(
  const rmw_init_options_t * src,
  rmw_init_options_t * dst)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(src, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(dst, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    src->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  if (NULL != dst->implementation_identifier) {
    RMW_UROS_TRACE_MESSAGE("expected zero-initialized dst")

    return RMW_RET_INVALID_ARGUMENT;
  }
  memcpy(dst, src, sizeof(rmw_init_options_t));

  rcutils_allocator_t allocator = src->allocator;
  RCUTILS_CHECK_ALLOCATOR(&allocator, return RMW_RET_INVALID_ARGUMENT);
  dst->enclave = rcutils_strdup(src->enclave, allocator);
  if (NULL != src->enclave && NULL == dst->enclave) {
    return RMW_RET_BAD_ALLOC;
  }

  rmw_uxrce_mempool_item_t * memory_node = get_memory(&init_options_memory);
  if (!memory_node) {
    allocator.deallocate(dst->enclave, allocator.state);
    RMW_UROS_TRACE_MESSAGE("Not available memory node")
    return RMW_RET_ERROR;
  }
  dst->impl = memory_node->data;

  rmw_uxrce_init_options_impl_t * dst_impl = dst->impl;
  rmw_uxrce_init_options_impl_t * src_impl = src->impl;

  dst_impl->transport_params = src_impl->transport_params;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_init_options_fini(
  rmw_init_options_t * init_options)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(init_options, RMW_RET_INVALID_ARGUMENT);
  rcutils_allocator_t * allocator = &init_options->allocator;
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    init_options->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  rmw_uxrce_mempool_item_t * item = init_options_memory.allocateditems;

  while (NULL != item) {
    rmw_uxrce_init_options_impl_t * aux_init_options =
      (rmw_uxrce_init_options_impl_t *)item->data;
    if (aux_init_options == init_options->impl) {
      put_memory(&init_options_memory, item);
      break;
    }
    item = item->next;
  }

  if (NULL == item) {
    return RMW_RET_ERROR;
  }

  allocator->deallocate(init_options->enclave, allocator->state);

  *init_options = rmw_get_zero_initialized_init_options();

  return RMW_RET_OK;
}

rmw_ret_t
rmw_init(
  const rmw_init_options_t * options,
  rmw_context_t * context)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(options, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(context, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(options->impl, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    options->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  context->instance_id = options->instance_id;
  context->implementation_identifier = eprosima_microxrcedds_identifier;
  context->actual_domain_id = options->domain_id;

#ifdef UCLIENT_PROFILE_MULTITHREAD
  if (!rmw_uxrce_wait_mutex_initialized) {
    UXR_INIT_LOCK(&rmw_uxrce_wait_mutex);
    rmw_uxrce_wait_mutex_initialized = true;
  }
#endif  // UCLIENT_PROFILE_MULTITHREAD

  rmw_uxrce_init_session_memory(&session_memory, custom_sessions, RMW_UXRCE_MAX_SESSIONS);
  rmw_uxrce_init_static_input_buffer_memory(
    &static_buffer_memory, custom_static_buffers,
    RMW_UXRCE_MAX_HISTORY);
  static_buffer_memory.is_dynamic_allowed = false;

  rmw_uxrce_mempool_item_t * memory_node = get_memory(&session_memory);
  if (!memory_node) {
    RMW_UROS_TRACE_MESSAGE("Not available session memory node")

    return RMW_RET_ERROR;
  }

  rmw_context_impl_t * context_impl = (rmw_context_impl_t *)memory_node->data;

  #if defined(RMW_UXRCE_TRANSPORT_CUSTOM)
  uxr_set_custom_transport_callbacks(
    &context_impl->transport,
    options->impl->transport_params.framing,
    options->impl->transport_params.open_cb,
    options->impl->transport_params.close_cb,
    options->impl->transport_params.write_cb,
    options->impl->transport_params.read_cb);
  #endif  // RMW_UXRCE_TRANSPORT_CUSTOM

  context_impl->creation_timeout = RMW_UXRCE_ENTITY_CREATION_TIMEOUT;
  context_impl->destroy_timeout = RMW_UXRCE_ENTITY_DESTROY_TIMEOUT;

  context_impl->creation_stream = (RMW_UXRCE_ENTITY_CREATION_TIMEOUT > 0) ?
    &context_impl->reliable_output :
    &context_impl->best_effort_output;

  context_impl->destroy_stream = (RMW_UXRCE_ENTITY_DESTROY_TIMEOUT > 0) ?
    &context_impl->reliable_output :
    &context_impl->best_effort_output;

  context_impl->id_participant = 0;
  context_impl->id_topic = 0;
  context_impl->id_publisher = 0;
  context_impl->id_datawriter = 0;
  context_impl->id_subscriber = 0;
  context_impl->id_datareader = 0;
  context_impl->id_requester = 0;
  context_impl->id_replier = 0;

  context_impl->graph_guard_condition.implementation_identifier = eprosima_microxrcedds_identifier;
  context_impl->graph_guard_condition.data = NULL;

  context->impl = context_impl;

  rmw_uxrce_init_node_memory(&node_memory, custom_nodes, RMW_UXRCE_MAX_NODES);
  rmw_uxrce_init_subscription_memory(
    &subscription_memory, custom_subscriptions,
    RMW_UXRCE_MAX_SUBSCRIPTIONS);
  rmw_uxrce_init_publisher_memory(&publisher_memory, custom_publishers, RMW_UXRCE_MAX_PUBLISHERS);
  rmw_uxrce_init_service_memory(&service_memory, custom_services, RMW_UXRCE_MAX_SERVICES);
  rmw_uxrce_init_client_memory(&client_memory, custom_clients, RMW_UXRCE_MAX_CLIENTS);
  rmw_uxrce_init_topic_memory(&topics_memory, custom_topics, RMW_UXRCE_MAX_TOPICS_INTERNAL);
  rmw_uxrce_init_init_options_impl_memory(
    &init_options_memory, custom_init_options,
    RMW_UXRCE_MAX_OPTIONS);
  rmw_uxrce_init_wait_set_memory(&wait_set_memory, custom_wait_set, RMW_UXRCE_MAX_WAIT_SETS);
  rmw_uxrce_init_guard_condition_memory(
    &guard_condition_memory, custom_guard_condition,
    RMW_UXRCE_MAX_GUARD_CONDITION);

  // Micro-XRCE-DDS Client transport initialization
  rmw_ret_t transport_init_ret = rmw_uxrce_transport_init(
    context->impl, options->impl, NULL);
  if (RMW_RET_OK != transport_init_ret) {
    CLOSE_TRANSPORT(&context_impl->transport);
    put_memory(&session_memory, &context_impl->mem);
    context->impl = NULL;
    RMW_UROS_TRACE_MESSAGE("failed to open transport in rmw_init.")
    return transport_init_ret;
  }

  uxr_init_session(
    &context_impl->session, &context_impl->transport.comm,
    options->impl->transport_params.client_key);

  uxr_set_topic_callback(&context_impl->session, on_topic, (void *)(context_impl));
  uxr_set_status_callback(&context_impl->session, on_status, NULL);
  uxr_set_request_callback(&context_impl->session, on_request, NULL);
  uxr_set_reply_callback(&context_impl->session, on_reply, NULL);

  context_impl->reliable_input = uxr_create_input_reliable_stream(
    &context_impl->session, context_impl->input_reliable_stream_buffer,
    context_impl->transport.comm.mtu * RMW_UXRCE_STREAM_HISTORY_INPUT,
    RMW_UXRCE_STREAM_HISTORY_INPUT);
  context_impl->reliable_output =
    uxr_create_output_reliable_stream(
    &context_impl->session, context_impl->output_reliable_stream_buffer,
    context_impl->transport.comm.mtu * RMW_UXRCE_STREAM_HISTORY_OUTPUT,
    RMW_UXRCE_STREAM_HISTORY_OUTPUT);

  context_impl->best_effort_input = uxr_create_input_best_effort_stream(&context_impl->session);
  context_impl->best_effort_output = uxr_create_output_best_effort_stream(
    &context_impl->session,
    context_impl->output_best_effort_stream_buffer, context_impl->transport.comm.mtu);

  if (!uxr_create_session(&context_impl->session)) {
    CLOSE_TRANSPORT(&context_impl->transport);
    put_memory(&session_memory, &context_impl->mem);
    context->impl = NULL;
    RMW_UROS_TRACE_MESSAGE("failed to create node session on Micro ROS Agent.")
    return RMW_RET_ERROR;
  }

#ifdef RMW_UXRCE_GRAPH
  // Create graph manager information
  if (RMW_RET_OK != rmw_graph_init(context_impl, &context_impl->graph_info)) {
    uxr_delete_session(&context_impl->session);
    return RMW_RET_ERROR;
  }
#endif  // RMW_UXRCE_GRAPH

  return RMW_RET_OK;
}

rmw_ret_t
rmw_shutdown(
  rmw_context_t * context)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(context, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    context->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  rmw_ret_t ret = rmw_context_fini(context);

  if (RMW_RET_OK == ret) {
    *context = rmw_get_zero_initialized_context();
  }

  rmw_uxrce_mempool_item_t * item = static_buffer_memory.allocateditems;

  while (item != NULL) {
    rmw_uxrce_mempool_item_t * aux_next = item->next;
    put_memory(&static_buffer_memory, item);
    item = aux_next;
  }

  return ret;
}

rmw_ret_t
rmw_context_fini(
  rmw_context_t * context)
{
  rmw_ret_t ret = RMW_RET_OK;

  rmw_uxrce_mempool_item_t * item = node_memory.allocateditems;

  while (item != NULL) {
    rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)item->data;
    item = item->next;
    if (custom_node->context == context->impl) {
      ret = rmw_destroy_node(&custom_node->rmw_node);
    }
  }

  if (NULL != context->impl) {
    size_t retries = UXR_CONFIG_MAX_SESSION_CONNECTION_ATTEMPTS;

    rmw_context_impl_t * context_impl = context->impl;
    if (context_impl->destroy_stream->type == UXR_BEST_EFFORT_STREAM) {
      retries = 0;
    }

    uxr_delete_session_retries(&context->impl->session, retries);
    rmw_uxrce_fini_session_memory(context->impl);
    CLOSE_TRANSPORT(&context->impl->transport);
  }


  context->impl = NULL;

  return ret;
}
