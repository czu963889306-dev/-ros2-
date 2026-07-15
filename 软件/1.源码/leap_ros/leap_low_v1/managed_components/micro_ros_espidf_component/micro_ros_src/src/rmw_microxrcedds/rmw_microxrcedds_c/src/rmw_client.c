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


#ifdef HAVE_C_TYPESUPPORT
#include <rosidl_typesupport_microxrcedds_c/identifier.h>
#endif /* ifdef HAVE_C_TYPESUPPORT */
#ifdef HAVE_CPP_TYPESUPPORT
#include <rosidl_typesupport_microxrcedds_cpp/identifier.h>
#endif /* ifdef HAVE_CPP_TYPESUPPORT */

#include <rmw/rmw.h>
#include <rmw/allocators.h>

#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_client_t *
rmw_create_client(
  const rmw_node_t * node,
  const rosidl_service_type_support_t * type_support,
  const char * service_name,
  const rmw_qos_profile_t * qos_policies)
{
  rmw_uxrce_client_t * custom_client = NULL;
  rmw_client_t * rmw_client = NULL;
  if (!node) {
    RMW_UROS_TRACE_MESSAGE("node handle is null")
  } else if (!type_support) {
    RMW_UROS_TRACE_MESSAGE("type support is null")
  } else if (!is_uxrce_rmw_identifier_valid(node->implementation_identifier)) {
    RMW_UROS_TRACE_MESSAGE("node handle not from this implementation")
  } else if (!service_name || strlen(service_name) == 0) {
    RMW_UROS_TRACE_MESSAGE("service name is null or empty string")
  } else if (!qos_policies) {
    RMW_UROS_TRACE_MESSAGE("qos_profile is null")
  } else {
    rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)node->data;
    rmw_uxrce_mempool_item_t * memory_node = get_memory(&client_memory);
    if (!memory_node) {
      RMW_UROS_TRACE_MESSAGE("Not available memory node")
      return NULL;
    }
    custom_client = (rmw_uxrce_client_t *)memory_node->data;

    rmw_client = &custom_client->rmw_client;
    rmw_client->data = custom_client;
    rmw_client->implementation_identifier = rmw_get_implementation_identifier();
    rmw_client->service_name = custom_client->service_name;
    if ((strlen(service_name) + 1 ) > sizeof(custom_client->service_name)) {
      RMW_UROS_TRACE_MESSAGE("failed to allocate string")
      goto fail;
    }

    memcpy((void *)rmw_client->service_name, service_name, strlen(service_name) + 1);

    custom_client->owner_node = custom_node;
    custom_client->session_timeout = RMW_UXRCE_PUBLISH_RELIABLE_TIMEOUT;
    custom_client->qos = *qos_policies;

    const rosidl_service_type_support_t * type_support_xrce = NULL;
#ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__IDENTIFIER_VALUE
    type_support_xrce = get_service_typesupport_handle(
      type_support, ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__IDENTIFIER_VALUE);
#endif /* ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_C__IDENTIFIER_VALUE */
#ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE
    if (NULL == type_support_xrce) {
      type_support_xrce = get_service_typesupport_handle(
        type_support, ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE);
    }
#endif /* ifdef ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP__IDENTIFIER_VALUE */
    if (NULL == type_support_xrce) {
      RMW_UROS_TRACE_MESSAGE("Undefined type support")
      goto fail;
    }

    custom_client->type_support_callbacks =
      (const service_type_support_callbacks_t *)type_support_xrce->data;

    if (custom_client->type_support_callbacks == NULL) {
      RMW_UROS_TRACE_MESSAGE("type support data is NULL")
      goto fail;
    }

    custom_client->client_id =
      uxr_object_id(custom_node->context->id_requester++, UXR_REQUESTER_ID);

    uint16_t client_req = UXR_INVALID_REQUEST_ID;

#ifdef RMW_UXRCE_USE_REFS
    // TODO(pablogs9): Use here true references
    // client_req = uxr_buffer_create_replier_ref(&custom_node->context->session,
    //     *custom_node->context->creation_stream, custom_service->subscriber_id,
    //     custom_node->participant_id, "", UXR_REPLACE | UXR_REUSE);
    char service_name_id[20];
    generate_name(&custom_client->client_id, service_name_id, sizeof(service_name_id));
    if (!build_service_xml(
        service_name_id, service_name, true,
        custom_client->type_support_callbacks, qos_policies, rmw_uxrce_entity_naming_buffer,
        sizeof(rmw_uxrce_entity_naming_buffer)))
    {
      RMW_UROS_TRACE_MESSAGE("failed to generate xml request for client creation")
      goto fail;
    }
    client_req = uxr_buffer_create_requester_xml(
      &custom_node->context->session,
      *custom_node->context->creation_stream,
      custom_client->client_id,
      custom_node->participant_id, rmw_uxrce_entity_naming_buffer, UXR_REPLACE | UXR_REUSE);
#else
    static char req_type_name[RMW_UXRCE_TYPE_NAME_MAX_LENGTH];
    static char res_type_name[RMW_UXRCE_TYPE_NAME_MAX_LENGTH];
    if (!generate_service_types(
        custom_client->type_support_callbacks, req_type_name, res_type_name,
        RMW_UXRCE_TYPE_NAME_MAX_LENGTH))
    {
      RMW_UROS_TRACE_MESSAGE("Not enough memory for service type names")
      goto fail;
    }

    static char req_topic_name[RMW_UXRCE_TOPIC_NAME_MAX_LENGTH];
    static char res_topic_name[RMW_UXRCE_TOPIC_NAME_MAX_LENGTH];
    if (!generate_service_topics(
        service_name, req_topic_name, res_topic_name,
        RMW_UXRCE_TOPIC_NAME_MAX_LENGTH))
    {
      RMW_UROS_TRACE_MESSAGE("Not enough memory for service topic names")
      goto fail;
    }

    client_req = uxr_buffer_create_requester_bin(
      &custom_node->context->session,
      *custom_node->context->creation_stream,
      custom_client->client_id,
      custom_node->participant_id,
      (char *) service_name,
      req_type_name,
      res_type_name,
      req_topic_name,
      res_topic_name,
      convert_qos_profile(qos_policies),
      UXR_REPLACE | UXR_REUSE);
#endif /* ifdef RMW_UXRCE_USE_XML */

    if (!run_xrce_session(
        custom_node->context, custom_node->context->creation_stream, client_req,
        custom_node->context->creation_timeout))
    {
      goto fail;
    }

    uxrDeliveryControl delivery_control;
    delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
    delivery_control.min_pace_period = 0;
    delivery_control.max_elapsed_time = UXR_MAX_ELAPSED_TIME_UNLIMITED;
    delivery_control.max_bytes_per_second = UXR_MAX_BYTES_PER_SECOND_UNLIMITED;

    custom_client->stream_id =
      (qos_policies->reliability == RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT) ?
      custom_node->context->best_effort_output :
      custom_node->context->reliable_output;

    uxrStreamId data_request_stream_id =
      (qos_policies->reliability == RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT) ?
      custom_node->context->best_effort_input :
      custom_node->context->reliable_input;

    custom_client->client_data_request = uxr_buffer_request_data(
      &custom_node->context->session,
      *custom_node->context->creation_stream, custom_client->client_id,
      data_request_stream_id, &delivery_control);
  }
  return rmw_client;

fail:
  rmw_uxrce_fini_client_memory(rmw_client);
  rmw_client = NULL;
  return rmw_client;
}

rmw_ret_t
rmw_destroy_client(
  rmw_node_t * node,
  rmw_client_t * client)
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
  } else if (!client) {
    RMW_UROS_TRACE_MESSAGE("client handle is null")
    result_ret = RMW_RET_ERROR;
  } else if (!is_uxrce_rmw_identifier_valid(client->implementation_identifier)) {
    RMW_UROS_TRACE_MESSAGE("client handle not from this implementation")
    result_ret = RMW_RET_ERROR;
  } else if (!client->data) {
    RMW_UROS_TRACE_MESSAGE("client imp is null")
    result_ret = RMW_RET_ERROR;
  } else {
    rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)node->data;
    rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)client->data;

    uint16_t client_req = uxr_buffer_cancel_data(
      &custom_node->context->session,
      *custom_node->context->destroy_stream,
      custom_client->client_id);

    run_xrce_session(
      custom_node->context, custom_node->context->destroy_stream, client_req,
      custom_node->context->destroy_timeout);

    uint16_t delete_client =
      uxr_buffer_delete_entity(
      &custom_node->context->session,
      *custom_node->context->destroy_stream,
      custom_client->client_id);

    if (!run_xrce_session(
        custom_node->context, custom_node->context->destroy_stream, delete_client,
        custom_node->context->destroy_timeout))
    {
      result_ret = RMW_RET_TIMEOUT;
    }
    rmw_uxrce_fini_client_memory(client);
  }

  return result_ret;
}

rmw_ret_t
rmw_client_request_publisher_get_actual_qos(
  const rmw_client_t * client,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)client->data;
  *qos = custom_client->qos;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_client_response_subscription_get_actual_qos(
  const rmw_client_t * client,
  rmw_qos_profile_t * qos)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(qos, RMW_RET_INVALID_ARGUMENT);

  rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)client->data;
  *qos = custom_client->qos;

  return RMW_RET_OK;
}
