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

#include <rmw_microxrcedds_c/config.h>

#ifdef RMW_UXRCE_GRAPH
#include <rmw/get_topic_endpoint_info.h>
#endif  // RMW_UXRCE_GRAPH

#ifdef HAVE_C_TYPESUPPORT
#include <rosidl_typesupport_microxrcedds_c/identifier.h>
#endif /* ifdef HAVE_C_TYPESUPPORT */
#ifdef HAVE_CPP_TYPESUPPORT
#include <rosidl_typesupport_microxrcedds_cpp/identifier.h>
#endif /* ifdef HAVE_CPP_TYPESUPPORT */

#include <rmw/rmw.h>
#include <rmw/types.h>
#include <rmw/allocators.h>

#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/rmw_microxrcedds_topic.h"
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_ret_t
rmw_init_subscription_allocation(
  const rosidl_message_type_support_t * type_support,
  const rosidl_runtime_c__Sequence__bound * message_bounds,
  rmw_subscription_allocation_t * allocation)
{
  (void)type_support;
  (void)message_bounds;
  (void)allocation;
  RMW_UROS_TRACE_MESSAGE("function not implemented")
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_fini_subscription_allocation(
  rmw_subscription_allocation_t * allocation)
{
  (void)allocation;
  RMW_UROS_TRACE_MESSAGE("function not implemented")
  return RMW_RET_UNSUPPORTED;
}

rmw_subscription_t *
rmw_create_subscription(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies,
  const rmw_subscription_options_t * subscription_options)
{
  (void)subscription_options;

  rmw_uxrce_subscription_t * custom_subscription = NULL;
  rmw_subscription_t * rmw_subscription = NULL;
  if (!node) {
    RMW_UROS_TRACE_MESSAGE("node handle is null")
  } else if (!type_support) {
    RMW_UROS_TRACE_MESSAGE("type support is null")
  } else if (!is_uxrce_rmw_identifier_valid(node->implementation_identifier)) {
    RMW_UROS_TRACE_MESSAGE("node handle not from this implementation")
  } else if (!topic_name || strlen(topic_name) == 0) {
    RMW_UROS_TRACE_MESSAGE("subscription topic is null or empty string")
    return NULL;
  } else if (!qos_policies) {
    RMW_UROS_TRACE_MESSAGE("qos_profile is null")
    return NULL;
  } else {
    rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)node->data;
    rmw_uxrce_mempool_item_t * memory_node = get_memory(&subscription_memory);
    if (!memory_node) {
      RMW_UROS_TRACE_MESSAGE("Not available memory node")
      return NULL;
    }
    custom_subscription = (rmw_uxrce_subscription_t *)memory_node->data;

    rmw_subscription = &custom_subscription->rmw_subscription;
    rmw_subscription->data = custom_subscription;
    rmw_subscription->implementation_identifier = rmw_get_implementation_identifier();
    rmw_subscription->topic_name = custom_subscription->topic_name;
    if ((strlen(topic_name) + 1 ) > sizeof(custom_subscription->topic_name)) {
      RMW_UROS_TRACE_MESSAGE("failed to allocate string")
      goto fail;
    }

    snprintf(
      (char *)rmw_subscription->topic_name, sizeof(custom_subscription->topic_name), "%s",
      topic_name);

    custom_subscription->owner_node = custom_node;
    custom_subscription->qos = *qos_policies;

    const rosidl_message_type_support_t * type_support_xrce = NULL;
#ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__IDENTIFIER_VALUE
    type_support_xrce = get_message_typesupport_handle(
      type_support, ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__IDENTIFIER_VALUE);
#endif /* ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__IDENTIFIER_VALUE */
#ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE
    if (NULL == type_support_xrce) {
      type_support_xrce = get_message_typesupport_handle(
        type_support, ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE);
    }
#endif /* ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE */
    if (NULL == type_support_xrce) {
      RMW_UROS_TRACE_MESSAGE("Undefined type support")
      goto fail;
    }

    custom_subscription->type_support_callbacks =
      (const message_type_support_callbacks_t *)type_support_xrce->data;

    if (custom_subscription->type_support_callbacks == NULL) {
      RMW_UROS_TRACE_MESSAGE("type support data is NULL")
      goto fail;
    }

    // Create topic
    custom_subscription->topic = create_topic(
      custom_node, topic_name,
      custom_subscription->type_support_callbacks, qos_policies);
    if (custom_subscription->topic == NULL) {
      goto fail;
    }

    // Create subscriber
    custom_subscription->subscriber_id = uxr_object_id(
      custom_node->context->id_subscriber++,
      UXR_SUBSCRIBER_ID);
    uint16_t subscriber_req = UXR_INVALID_REQUEST_ID;

#ifdef RMW_UXRCE_USE_REFS
    subscriber_req = uxr_buffer_create_subscriber_xml(
      &custom_node->context->session,
      *custom_node->context->creation_stream, custom_subscription->subscriber_id,
      custom_node->participant_id, "", UXR_REPLACE | UXR_REUSE);
#else
    subscriber_req = uxr_buffer_create_subscriber_bin(
      &custom_node->context->session,
      *custom_node->context->creation_stream,
      custom_subscription->subscriber_id,
      custom_node->participant_id,
      UXR_REPLACE | UXR_REUSE);
#endif /* ifdef RMW_UXRCE_USE_REFS */

    if (!run_xrce_session(
        custom_node->context, custom_node->context->creation_stream, subscriber_req,
        custom_node->context->creation_timeout))
    {
      goto fail;
    }

    // Create datareader
    custom_subscription->datareader_id = uxr_object_id(
      custom_node->context->id_datareader++,
      UXR_DATAREADER_ID);
    uint16_t datareader_req = UXR_INVALID_REQUEST_ID;

#ifdef RMW_UXRCE_USE_REFS
    if (!build_datareader_profile(
        topic_name, rmw_uxrce_entity_naming_buffer,
        sizeof(rmw_uxrce_entity_naming_buffer)))
    {
      RMW_UROS_TRACE_MESSAGE("failed to generate xml request for node creation")
      goto fail;
    }

    datareader_req = uxr_buffer_create_datareader_ref(
      &custom_node->context->session,
      *custom_node->context->creation_stream, custom_subscription->datareader_id,
      custom_subscription->subscriber_id, rmw_uxrce_entity_naming_buffer, UXR_REPLACE | UXR_REUSE);
#else
    datareader_req = uxr_buffer_create_datareader_bin(
      &custom_node->context->session,
      *custom_node->context->creation_stream,
      custom_subscription->datareader_id,
      custom_subscription->subscriber_id,
      custom_subscription->topic->topic_id,
      convert_qos_profile(qos_policies),
      UXR_REPLACE | UXR_REUSE);
#endif /* ifdef RMW_UXRCE_USE_XML */

    if (!run_xrce_session(
        custom_node->context, custom_node->context->creation_stream, datareader_req,
        custom_node->context->creation_timeout))
    {
      RMW_UROS_TRACE_MESSAGE("Issues creating Micro XRCE-DDS entities")
      goto fail;
    }

    uxrDeliveryControl delivery_control;
    delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    delivery_control.min_pace_period = 0;
    delivery_control.max_elapsed_time = UXR_MAX_ELAPSED_TIME_UNLIMITED;
    delivery_control.max_bytes_per_second = UXR_MAX_BYTES_PER_SECOND_UNLIMITED;

    uxrStreamId data_request_stream_id =
      (qos_policies->reliability == RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT) ?
      custom_node->context->best_effort_input :
      custom_node->context->reliable_input;

    uxr_buffer_request_data(
      &custom_node->context->session,
      *custom_node->context->creation_stream, custom_subscription->datareader_id,
      data_request_stream_id, &delivery_control);
  }
  return rmw_subscription;

fail:
  if (custom_subscription != NULL && custom_subscription->topic != NULL) {
    rmw_uxrce_fini_topic_memory(custom_subscription->topic);
  }

  rmw_uxrce_fini_subscription_memory(rmw_subscription);
  rmw_subscription = NULL;
  return rmw_subscription;
}

rmw_ret_t
rmw_subscription_count_matched_publishers(
  const rmw_subscription_t * subscription,
  size_t * publisher_count)
{
#ifdef RMW_UXRCE_GRAPH
  rmw_ret_t ret = RMW_RET_OK;
  const char * topic_name = subscription->topic_name;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rmw_uxrce_subscription_t * custom_subscription = (rmw_uxrce_subscription_t *)subscription->data;
  const rmw_node_t * node = &custom_subscription->owner_node->rmw_node;

  rmw_topic_endpoint_info_array_t publishers_info =
    rmw_get_zero_initialized_topic_endpoint_info_array();

  if (RMW_RET_OK != rmw_get_publishers_info_by_topic(
      node,
      &allocator,
      topic_name,
      false,
      &publishers_info))
  {
    ret = RMW_RET_ERROR;
    goto sub_count_pub_fail;
  } else {
    *publisher_count = publishers_info.size;
  }

sub_count_pub_fail:
  if (RMW_RET_OK != rmw_topic_endpoint_info_array_fini(&publishers_info, &allocator)) {
    ret = RMW_RET_ERROR;
  }
  return ret;
#else
  (void)subscription;
  (void)publisher_count;
  RMW_UROS_TRACE_MESSAGE(
    "Function not available; enable RMW_UXRCE_GRAPH configuration profile before using");
  return RMW_RET_UNSUPPORTED;
#endif  // RMW_UXRCE_GRAPH
}

rmw_ret_t
rmw_subscription_get_actual_qos(
  const rmw_subscription_t * subscription,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(subscription, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  rmw_uxrce_subscription_t * custom_subscription = (rmw_uxrce_subscription_t *)subscription->data;
  *qos = custom_subscription->qos;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_destroy_subscription(
  rmw_node_t * node,
  rmw_subscription_t * subscription)
{
  rmw_ret_t result_ret = RMW_RET_OK;
  if (!node) {
    RMW_UROS_TRACE_MESSAGE("node handle is null")
    result_ret = RMW_RET_ERROR;
  } else if (!is_uxrce_rmw_identifier_valid(node->implementation_identifier)) {
    RMW_UROS_TRACE_MESSAGE("node handle not from this implementation")
    result_ret = RMW_RET_ERROR;
  } else if (!node->data) {
    RMW_UROS_TRACE_MESSAGE("node imp is null")
    result_ret = RMW_RET_ERROR;
  } else if (!subscription) {
    RMW_UROS_TRACE_MESSAGE("subscription handle is null")
    result_ret = RMW_RET_ERROR;
  } else if (!is_uxrce_rmw_identifier_valid(subscription->implementation_identifier)) {
    RMW_UROS_TRACE_MESSAGE("subscription handle not from this implementation")
    result_ret = RMW_RET_ERROR;
  } else if (!subscription->data) {
    RMW_UROS_TRACE_MESSAGE("subscription imp is null")
    result_ret = RMW_RET_ERROR;
  } else {
    rmw_uxrce_subscription_t * custom_subscription = (rmw_uxrce_subscription_t *)subscription->data;
    rmw_uxrce_node_t * custom_node = custom_subscription->owner_node;

    uint16_t datareader_req = uxr_buffer_cancel_data(
      &custom_node->context->session,
      *custom_node->context->destroy_stream,
      custom_subscription->datareader_id);

    run_xrce_session(
      custom_node->context, custom_node->context->destroy_stream, datareader_req,
      custom_node->context->destroy_timeout);

    destroy_topic(custom_subscription->topic);

    uint16_t delete_datareader =
      uxr_buffer_delete_entity(
      &custom_subscription->owner_node->context->session,
      *custom_subscription->owner_node->context->destroy_stream,
      custom_subscription->datareader_id);
    uint16_t delete_subscriber =
      uxr_buffer_delete_entity(
      &custom_subscription->owner_node->context->session,
      *custom_subscription->owner_node->context->destroy_stream,
      custom_subscription->subscriber_id);

    bool ret = run_xrce_session(
      custom_node->context, custom_node->context->destroy_stream, delete_datareader,
      custom_node->context->destroy_timeout);
    ret &= run_xrce_session(
      custom_node->context, custom_node->context->destroy_stream, delete_subscriber,
      custom_node->context->destroy_timeout);
    if (!ret) {
      result_ret = RMW_RET_TIMEOUT;
    }
    rmw_uxrce_fini_subscription_memory(subscription);
  }

  return result_ret;
}

rmw_ret_t
rmw_subscription_set_content_filter(
  rmw_subscription_t * subscription,
  const rmw_subscription_content_filter_options_t * options)
{
  (void) subscription;
  (void) options;

  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_subscription_get_content_filter(
  const rmw_subscription_t * subscription,
  rcutils_allocator_t * allocator,
  rmw_subscription_content_filter_options_t * options)
{
  (void) subscription;
  (void) allocator;
  (void) options;

  return RMW_RET_UNSUPPORTED;
}
