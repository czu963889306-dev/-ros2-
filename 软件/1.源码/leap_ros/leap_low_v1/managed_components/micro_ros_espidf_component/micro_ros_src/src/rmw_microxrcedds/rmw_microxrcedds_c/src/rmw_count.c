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
#include <rmw/error_handling.h>
#include <rmw/sanity_checks.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microxrcedds_c/rmw_c_macros.h>

#include "./rmw_microros_internal/types.h"
#include "./rmw_microros_internal/identifiers.h"
#include "./rmw_microros_internal/error_handling_internal.h"

#ifdef RMW_UXRCE_GRAPH
#include "./rmw_microros_internal/rmw_graph.h"
#endif  // RMW_UXRCE_GRAPH

#ifdef RMW_UXRCE_GRAPH
static rmw_ret_t
__rmw_count_entities(
  uint8_t kind,
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count)
{
  // Perform RMW checks
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_ARGUMENT_FOR_NULL(topic_name, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(count, RMW_RET_INVALID_ARGUMENT);
  // Set count to zero, just in case it was holding another value
  *count = 0;

  // Get micro_ros_msg/msg/Graph instance
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
      if (0 == strcmp(topic_name, entity->name.data) && kind == entity->entity_type) {
        // We found an entity that matches, increment counter
        ++(*count);
      }
    }
  }

fini:
  micro_ros_msgs__msg__Graph__destroy(graph_data);
  return ret;
}

#endif  // RMW_UXRCE_GRAPH

rmw_ret_t
rmw_count_publishers(
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count)
{
#ifdef RMW_UXRCE_GRAPH
  return __rmw_count_entities(
    micro_ros_msgs__msg__Entity__PUBLISHER,
    node,
    topic_name,
    count);
#else
  (void)node;
  (void)topic_name;
  (void)count;
  RMW_UROS_TRACE_MESSAGE(
    "Function not available; enable RMW_UXRCE_GRAPH configuration profile before using");
  return RMW_RET_UNSUPPORTED;
#endif  // RMW_UXRCE_GRAPH
}

rmw_ret_t
rmw_count_subscribers(
  const rmw_node_t * node,
  const char * topic_name,
  size_t * count)
{
#ifdef RMW_UXRCE_GRAPH
  return __rmw_count_entities(
    micro_ros_msgs__msg__Entity__SUBSCRIBER,
    node,
    topic_name,
    count);
#else
  (void)node;
  (void)topic_name;
  (void)count;
  RMW_UROS_TRACE_MESSAGE(
    "Function not available; enable RMW_UXRCE_GRAPH configuration profile before using");
  return RMW_RET_UNSUPPORTED;
#endif  // RMW_UXRCE_GRAPH
}
