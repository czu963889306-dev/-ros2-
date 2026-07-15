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

#include <rmw/rmw.h>
#include <rmw/sanity_checks.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microxrcedds_c/rmw_c_macros.h>
#include <rcutils/types/string_array.h>

#include <rmw/get_topic_endpoint_info.h>
#include <rmw/types.h>

#include "./rmw_microros_internal/types.h"
#include "./rmw_microros_internal/identifiers.h"
#include "./rmw_microros_internal/error_handling_internal.h"

#ifdef RMW_UXRCE_GRAPH
#include "./rmw_microros_internal/rmw_graph.h"
#endif  // RMW_UXRCE_GRAPH

#ifdef RMW_UXRCE_GRAPH
static rmw_endpoint_type_t
__endpoint_kind_to_endpoint_type(
  const uint8_t kind)
{
  rmw_endpoint_type_t endpoint_type = RMW_ENDPOINT_INVALID;

  switch (kind) {
    case micro_ros_msgs__msg__Entity__PUBLISHER:
      {
        endpoint_type = RMW_ENDPOINT_PUBLISHER;
        break;
      }

    case micro_ros_msgs__msg__Entity__SUBSCRIBER:
      {
        endpoint_type = RMW_ENDPOINT_SUBSCRIPTION;
        break;
      }

    default:
      {
        break;
      }
  }
  return endpoint_type;
}

static rmw_ret_t
__rmw_get_endpoint_info_by_topic(
  const uint8_t kind,
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rmw_topic_endpoint_info_array_t * endpoints_info)
{
  (void)no_mangle;   // TODO(jamoralp): what is this used for?
  // Perform RMW checks
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    allocator, "Allocator argument is invalid",
    return RMW_RET_INVALID_ARGUMENT);

  if (RMW_RET_OK != rmw_topic_endpoint_info_array_check_zero(endpoints_info)) {
    return RMW_RET_INVALID_ARGUMENT;
  }

  // Get micro_ros_msgs/msg/Graph instance
  rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)(node->data);
  rmw_graph_info_t * graph_info = &custom_node->context->graph_info;

  rmw_ret_t ret = RMW_RET_OK;

  if (!graph_info->initialized) {
    return ret;
  }

  micro_ros_msgs__msg__Graph * graph_data = micro_ros_msgs__msg__Graph__create();

  if (RMW_RET_OK != rmw_graph_fill_data_from_buffer(graph_info, graph_data)) {
    ret = RMW_RET_ERROR;
    goto fini;
  }

  // Look for given topic
  for (size_t i = 0; i < graph_data->nodes.size; ++i) {
    micro_ros_msgs__msg__Node * node = &graph_data->nodes.data[i];
    size_t entities_size = node->entities.size;

    for (size_t j = 0; j < entities_size; ++j) {
      micro_ros_msgs__msg__Entity * entity = &node->entities.data[j];

      if (0 == strcmp(topic_name, entity->name.data) &&
        kind == entity->entity_type)
      {
        size_t current_position = endpoints_info->size;
        if (0 == current_position) {
          // First data introduced: init array
          if (RMW_RET_OK != rmw_topic_endpoint_info_array_init_with_size(
              endpoints_info, 1, allocator))
          {
            ret = RMW_RET_ERROR;
            goto fini;
          }
        } else {
          // Subsequent data: resize
          endpoints_info->info_array = allocator->reallocate(
            endpoints_info->info_array,
            ++endpoints_info->size * sizeof(rmw_topic_endpoint_info_t),
            allocator->state);
        }

        // Retrieve endpoint information
        // Node name
        rmw_topic_endpoint_info_t * endpoint_info =
          &endpoints_info->info_array[current_position];
        endpoint_info->node_name = allocator->zero_allocate(
          strlen(node->node_name.data) + 1,
          sizeof(char), allocator->state);
        if (RMW_RET_OK != rmw_topic_endpoint_info_set_node_name(
            endpoint_info,
            node->node_name.data,
            allocator))
        {
          ret = RMW_RET_ERROR;
          goto fini;
        }
        // Node namespaces
        endpoint_info->node_namespace = allocator->zero_allocate(
          strlen(node->node_namespace.data) + 1,
          sizeof(char), allocator->state);
        if (RMW_RET_OK != rmw_topic_endpoint_info_set_node_namespace(
            endpoint_info,
            node->node_namespace.data,
            allocator))
        {
          ret = RMW_RET_ERROR;
          goto fini;
        }
        // Topic type (fetch first)
        endpoint_info->topic_type = allocator->zero_allocate(
          strlen(entity->types.data[0].data) + 1,
          sizeof(char), allocator->state);
        if (RMW_RET_OK != rmw_topic_endpoint_info_set_topic_type(
            endpoint_info,
            entity->types.data[0].data,
            allocator))
        {
          ret = RMW_RET_ERROR;
          goto fini;
        }
        if (RMW_RET_OK != rmw_topic_endpoint_info_set_endpoint_type(
            endpoint_info,
            __endpoint_kind_to_endpoint_type(kind)))
        {
          ret = RMW_RET_ERROR;
          goto fini;
        }
        // GID and QoS profile: leave blank
        // TODO(jamoralp): should we really fill this information? Is it useful in micro-ROS?
      }
    }
  }

fini:
  micro_ros_msgs__msg__Graph__destroy(graph_data);
  return ret;
}

#endif  // RMW_UXRCE_GRAPH

rmw_ret_t
rmw_get_publishers_info_by_topic(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rmw_topic_endpoint_info_array_t * publishers_info)
{
#ifdef RMW_UXRCE_GRAPH
  return __rmw_get_endpoint_info_by_topic(
    micro_ros_msgs__msg__Entity__PUBLISHER,
    node,
    allocator,
    topic_name,
    no_mangle,
    publishers_info);
#else
  (void)node;
  (void)allocator;
  (void)topic_name;
  (void)no_mangle;
  (void)publishers_info;
  RMW_UROS_TRACE_MESSAGE(
    "Function not available; enable RMW_UXRCE_GRAPH configuration profile before using");
  return RMW_RET_UNSUPPORTED;
#endif  // RMW_UXRCE_GRAPH
}

rmw_ret_t
rmw_get_subscriptions_info_by_topic(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  const char * topic_name,
  bool no_mangle,
  rmw_topic_endpoint_info_array_t * subscriptions_info)
{
#ifdef RMW_UXRCE_GRAPH
  return __rmw_get_endpoint_info_by_topic(
    micro_ros_msgs__msg__Entity__SUBSCRIBER,
    node,
    allocator,
    topic_name,
    no_mangle,
    subscriptions_info);
#else
  (void)node;
  (void)allocator;
  (void)topic_name;
  (void)no_mangle;
  (void)subscriptions_info;
  RMW_UROS_TRACE_MESSAGE(
    "Function not available; enable RMW_UXRCE_GRAPH configuration profile before using");
  return RMW_RET_UNSUPPORTED;
#endif  // RMW_UXRCE_GRAPH
}
