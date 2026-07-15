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

#include "./rmw_microros_internal/types.h"
#include "./rmw_microros_internal/identifiers.h"
#include "./rmw_microros_internal/error_handling_internal.h"

#ifdef RMW_UXRCE_GRAPH
#include "./rmw_microros_internal/rmw_graph.h"
#endif  // RMW_UXRCE_GRAPH

rmw_ret_t
rmw_get_node_names(
  const rmw_node_t * node,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces)
{
#ifdef RMW_UXRCE_GRAPH
  // Perform RMW checks
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  if (RMW_RET_OK != rmw_check_zero_rmw_string_array(node_names)) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (RMW_RET_OK != rmw_check_zero_rmw_string_array(node_namespaces)) {
    return RMW_RET_INVALID_ARGUMENT;
  }

  // Get micro_ros_msgs/msg/Graph instance
  rmw_uxrce_node_t * custom_node = (rmw_uxrce_node_t *)(node->data);
  rmw_graph_info_t * graph_info = &custom_node->context->graph_info;

  if (!graph_info->initialized) {
    return RMW_RET_OK;
  }

  rmw_ret_t ret = RMW_RET_OK;
  micro_ros_msgs__msg__Graph * graph_data = micro_ros_msgs__msg__Graph__create();

  if (RMW_RET_OK != rmw_graph_fill_data_from_buffer(graph_info, graph_data)) {
    ret = RMW_RET_ERROR;
    goto fini;
  }

  // Init node name and namespaces string array
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (RCUTILS_RET_OK != rcutils_string_array_init(
      node_names, graph_data->nodes.size, &allocator))
  {
    ret = RMW_RET_ERROR;
    goto fini;
  }
  if (RCUTILS_RET_OK != rcutils_string_array_init(
      node_namespaces, graph_data->nodes.size, &allocator))
  {
    ret = RMW_RET_ERROR;
    goto fini;
  }

  // Copy information into result arrays
  for (size_t i = 0; i < graph_data->nodes.size; ++i) {
    micro_ros_msgs__msg__Node * node = &graph_data->nodes.data[i];
    node_namespaces->data[i] = allocator.zero_allocate(
      strlen(node->node_namespace.data) + 1, sizeof(char), allocator.state);
    snprintf(node_namespaces->data[i], node_namespaces->size, "%s", node->node_namespace.data);
    node_names->data[i] = allocator.zero_allocate(
      strlen(node->node_name.data) + 1, sizeof(char), allocator.state);
    snprintf(node_names->data[i], node_names->size, "%s", node->node_name.data);
  }

fini:
  micro_ros_msgs__msg__Graph__destroy(graph_data);
  return ret;
#else
  (void)node;
  (void)node_names;
  (void)node_namespaces;
  RMW_UROS_TRACE_MESSAGE(
    "Function not available: enable RMW_UXRCE_GRAPH configuration profile before using");
  return RMW_RET_UNSUPPORTED;
#endif  // RMW_UXRCE_GRAPH
}

rmw_ret_t
rmw_get_node_names_with_enclaves(
  const rmw_node_t * node,
  rcutils_string_array_t * node_names,
  rcutils_string_array_t * node_namespaces,
  rcutils_string_array_t * enclaves)
{
  (void)enclaves;   // TODO(jamoralp): what is this used for?
  return rmw_get_node_names(node, node_names, node_namespaces);
}
