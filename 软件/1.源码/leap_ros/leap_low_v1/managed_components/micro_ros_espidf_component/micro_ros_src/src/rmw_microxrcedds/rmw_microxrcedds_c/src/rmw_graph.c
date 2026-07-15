// Copyright 2020 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <rmw_microros_internal/rmw_graph.h>

#include <rosidl_runtime_c/string_functions.h>
#include <micro_ros_msgs/msg/detail/graph__rosidl_typesupport_microxrcedds_c.h>

#include "./rmw_microros_internal/utils.h"

rmw_ret_t rmw_graph_init(
  rmw_context_impl_t * context,
  rmw_graph_info_t * graph_info)
{
  rmw_ret_t ret = RMW_RET_OK;

  if (NULL == &context->session) {
    RMW_UROS_TRACE_MESSAGE("Cannot initializate graph context without a XRCE session")
    return RMW_RET_INVALID_ARGUMENT;
  }

  // Init graph_info structure
  graph_info->initialized = false;
  graph_info->has_changed = false;
  graph_info->context = context;

  // Create micro-ROS graph participant
  graph_info->participant_id =
    uxr_object_id(context->id_participant++, UXR_PARTICIPANT_ID);
  size_t microros_domain_id = 0;  // TODO(jamoralp): shall this Domain ID be configurabe, user wise?
  const char * graph_participant_name = "microros_graph";

  uint16_t participant_req = uxr_buffer_create_participant_bin(
    &context->session,
    *context->creation_stream,
    graph_info->participant_id,
    (int16_t)microros_domain_id,
    graph_participant_name,
    UXR_REPLACE | UXR_REUSE);

  // Set graph subscription QoS policies
  // TODO(jamoralp): most of these QoS are not even being used.
  const rmw_qos_profile_t graph_subscription_qos_policies =
  {
    RMW_QOS_POLICY_HISTORY_KEEP_LAST,
    1,
    RMW_QOS_POLICY_RELIABILITY_RELIABLE,
    RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL,
    RMW_QOS_DEADLINE_DEFAULT,
    RMW_QOS_LIFESPAN_DEFAULT,
    RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
    RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
    true
  };

  // Create graph subscriber requests
  graph_info->subscriber_id = uxr_object_id(context->id_subscriber++, UXR_SUBSCRIBER_ID);
  char subscriber_name[20];
  generate_name(&graph_info->subscriber_id, subscriber_name, sizeof(subscriber_name));
  if (!build_subscriber_xml(
      subscriber_name, rmw_uxrce_entity_naming_buffer,
      sizeof(rmw_uxrce_entity_naming_buffer)))
  {
    RMW_UROS_TRACE_MESSAGE("Failed to generate xml request for graph subscriber creation")
    ret = RMW_RET_ERROR;
    goto end;
  }

  uint16_t subscriber_req = uxr_buffer_create_subscriber_xml(
    &context->session, *context->creation_stream, graph_info->subscriber_id,
    graph_info->participant_id, rmw_uxrce_entity_naming_buffer, UXR_REPLACE | UXR_REUSE);

  graph_info->datareader_id = uxr_object_id(context->id_datareader++, UXR_DATAREADER_ID);
  const char * graph_topic_name = "ros_to_microros_graph";
  graph_info->graph_type_support =
    // NOLINTNEXTLINE
    rosidl_typesupport_microxrcedds_c__get_message_type_support_handle__micro_ros_msgs__msg__Graph();

  // Create graph topic request
  graph_info->topic_id = uxr_object_id(context->id_topic++, UXR_TOPIC_ID);
  if (!build_topic_xml(
      graph_topic_name,
      (message_type_support_callbacks_t *)(graph_info->graph_type_support->data),
      &graph_subscription_qos_policies, rmw_uxrce_entity_naming_buffer,
      sizeof(rmw_uxrce_entity_naming_buffer)))
  {
    RMW_UROS_TRACE_MESSAGE("Failed to generate xml request for graph topic creation")
    ret = RMW_RET_ERROR;
    goto end;
  }

  uint16_t topic_req = uxr_buffer_create_topic_xml(
    &context->session, *context->creation_stream, graph_info->topic_id,
    graph_info->participant_id, rmw_uxrce_entity_naming_buffer, UXR_REPLACE | UXR_REUSE);

  // Create graph datareader request
  if (!build_datareader_xml(
      graph_topic_name,
      (message_type_support_callbacks_t *)(graph_info->graph_type_support->data),
      &graph_subscription_qos_policies, rmw_uxrce_entity_naming_buffer,
      sizeof(rmw_uxrce_entity_naming_buffer)))
  {
    RMW_UROS_TRACE_MESSAGE("Failed to generate xml request for graph datareader creation")
    ret = RMW_RET_ERROR;
    goto end;
  }

  uint16_t datareader_req = uxr_buffer_create_datareader_xml(
    &context->session, *context->creation_stream, graph_info->datareader_id,
    graph_info->subscriber_id, rmw_uxrce_entity_naming_buffer, UXR_REPLACE | UXR_REUSE);

  // Run session
  uint16_t requests[] = {
    participant_req, subscriber_req, topic_req, datareader_req
  };
  uint8_t status[sizeof(requests) / 2];

  if (!uxr_run_session_until_all_status(
      &context->session, 1000, requests, status, sizeof(status)))
  {
    RMW_UROS_TRACE_MESSAGE("Issues creating Micro XRCE-DDS graph related entities")
    ret = RMW_RET_ERROR;
    goto end;
  }

  // Request data from topic: set delivery flow
  uxrDeliveryControl delivery_control;
  delivery_control.max_samples = UXR_MAX_SAMPLES_UNLIMITED;
  delivery_control.min_pace_period = 0;
  delivery_control.max_elapsed_time = UXR_MAX_ELAPSED_TIME_UNLIMITED;
  delivery_control.max_bytes_per_second = UXR_MAX_BYTES_PER_SECOND_UNLIMITED;

  uxr_buffer_request_data(
    &context->session,
    context->reliable_output, graph_info->datareader_id,
    context->reliable_input, &delivery_control);

end:
  return ret;
}

#define UCDR_DESERIALIZE_AND_CHECK_RETVAL(RETVAL, FUNCTION_NAME, ...) \
  { \
    bool func_ret = FUNCTION_NAME(__VA_ARGS__); \
    if (!func_ret) { \
      RMW_UROS_TRACE_MESSAGE("Error deserializing graph information"); \
      RETVAL = RMW_RET_ERROR; \
    } \
  }

rmw_ret_t rmw_graph_fill_data_from_buffer(
  rmw_graph_info_t * graph_info,
  micro_ros_msgs__msg__Graph * graph_data)
{
  rmw_ret_t ret = RMW_RET_OK;
  ucdrBuffer temp_buffer;

  ucdr_init_buffer(&temp_buffer, graph_info->micro_buffer, graph_info->micro_buffer_length);
  // Get nodes size
  uint32_t nodes_size;
  UCDR_DESERIALIZE_AND_CHECK_RETVAL(ret, ucdr_deserialize_uint32_t, &temp_buffer, &nodes_size);

  // Init nodes sequence and deserialize
  micro_ros_msgs__msg__Node__Sequence__init(&graph_data->nodes, nodes_size);
  for (size_t i = 0; i < nodes_size && (ret == RMW_RET_OK); ++i) {
    micro_ros_msgs__msg__Node * node = &graph_data->nodes.data[i];
    char temp_string[256];
    memset(temp_string, 0, sizeof(temp_string));

    // Deserialize node_namespace string
    UCDR_DESERIALIZE_AND_CHECK_RETVAL(
      ret, ucdr_deserialize_string, &temp_buffer, temp_string, sizeof(temp_string));
    rosidl_runtime_c__String__assign(&node->node_namespace, temp_string);

    // Deserialize node_name string
    memset(temp_string, 0, sizeof(temp_string));
    UCDR_DESERIALIZE_AND_CHECK_RETVAL(
      ret, ucdr_deserialize_string, &temp_buffer, temp_string, sizeof(temp_string));
    rosidl_runtime_c__String__assign(&node->node_name, temp_string);

    // Get entities size
    uint32_t entities_size;
    UCDR_DESERIALIZE_AND_CHECK_RETVAL(ret, ucdr_deserialize_uint32_t, &temp_buffer, &entities_size);

    // Init entities sequence and deserialize
    micro_ros_msgs__msg__Entity__Sequence__init(&node->entities, entities_size);
    for (size_t j = 0; j < entities_size && (ret == RMW_RET_OK); ++j) {
      micro_ros_msgs__msg__Entity * entity = &node->entities.data[j];

      // Deserialize entity type
      UCDR_DESERIALIZE_AND_CHECK_RETVAL(
        ret, ucdr_deserialize_uint8_t, &temp_buffer,
        &entity->entity_type);

      // Deserialize entity name
      memset(temp_string, 0, sizeof(temp_string));
      UCDR_DESERIALIZE_AND_CHECK_RETVAL(
        ret, ucdr_deserialize_string, &temp_buffer, temp_string, sizeof(temp_string));
      rosidl_runtime_c__String__assign(&entity->name, temp_string);

      // Get entity types size
      uint32_t types_size;
      UCDR_DESERIALIZE_AND_CHECK_RETVAL(ret, ucdr_deserialize_uint32_t, &temp_buffer, &types_size);

      // Init entity types sequence and deserialize
      rosidl_runtime_c__String__Sequence__init(&entity->types, types_size);
      for (size_t k = 0; k < types_size && (ret == RMW_RET_OK); ++k) {
        memset(temp_string, 0, sizeof(temp_string));
        UCDR_DESERIALIZE_AND_CHECK_RETVAL(
          ret, ucdr_deserialize_string, &temp_buffer, temp_string, sizeof(temp_string));
        rosidl_runtime_c__String__assign(&entity->types.data[k], temp_string);
      }
    }
  }
  return ret;
}
