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
#include <rmw/names_and_types.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microxrcedds_c/rmw_c_macros.h>

#include "./rmw_microros_internal/types.h"
#include "./rmw_microros_internal/identifiers.h"
#include "./rmw_microros_internal/error_handling_internal.h"

#ifdef RMW_UXRCE_GRAPH
#include "./rmw_microros_internal/rmw_graph.h"
#endif  // RMW_UXRCE_GRAPH

rmw_ret_t
rmw_get_topic_names_and_types(
  const rmw_node_t * node,
  rcutils_allocator_t * allocator,
  bool no_demangle,
  rmw_names_and_types_t * topic_names_and_types)
{
#ifdef RMW_UXRCE_GRAPH
  (void)no_demangle;   // TODO(jamoralp): what to use this for?

  // Perform RMW checks
  RMW_CHECK_ARGUMENT_FOR_NULL(node, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    node->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    allocator, "Allocator argument is invalid",
    return RMW_RET_INVALID_ARGUMENT);

  if (RMW_RET_OK != rmw_names_and_types_check_zero(topic_names_and_types)) {
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

  // Initialize names_and_types structure according to Graph size
  for (size_t i = 0; i < graph_data->nodes.size; ++i) {
    micro_ros_msgs__msg__Node * node = &graph_data->nodes.data[i];
    size_t entities_size = node->entities.size;

    for (size_t j = 0; j < entities_size; ++j) {
      micro_ros_msgs__msg__Entity * entity = &node->entities.data[j];

      switch (entity->entity_type) {
        case micro_ros_msgs__msg__Entity__PUBLISHER:
        case micro_ros_msgs__msg__Entity__SUBSCRIBER:
          {
            // Do not include in the list if it already exists
            bool already_included = false;
            for (size_t k = 0; k < topic_names_and_types->names.size; ++k) {
              if (0 == strcmp(topic_names_and_types->names.data[k], entity->name.data)) {
                already_included = true;
                break;
              }
            }
            if (already_included) {
              break;
            }

            // Retrieve name
            size_t current_position = topic_names_and_types->names.size;
            if (0 == current_position) {
              // First data introduced: init array
              if (RCUTILS_RET_OK != rcutils_string_array_init(
                  &topic_names_and_types->names, 1, allocator))
              {
                ret = RMW_RET_ERROR;
                goto fini;
              }
            } else {
              // Subsequent data: resize
              if (RCUTILS_RET_OK != rcutils_string_array_resize(
                  &topic_names_and_types->names, topic_names_and_types->names.size + 1))
              {
                ret = RMW_RET_ERROR;
                goto fini;
              }
            }

            // Make space for entity name string
            topic_names_and_types->names.data[current_position] = allocator->reallocate(
              topic_names_and_types->names.data[current_position],
              strlen(entity->name.data) + 1, allocator->state);
            snprintf(
              topic_names_and_types->names.data[current_position],
              topic_names_and_types->names.capacity,
              "%s",
              entity->name.data);

            // Retrieve types
            if (NULL == topic_names_and_types->types) {
              topic_names_and_types->types = allocator->zero_allocate(
                1, sizeof(rcutils_string_array_t), allocator->state);
            } else {
              topic_names_and_types->types = allocator->reallocate(
                topic_names_and_types->types,
                topic_names_and_types->names.size * sizeof(rcutils_string_array_t),
                allocator->state);
            }

            size_t types_size = entity->types.size;
            if (RCUTILS_RET_OK != rcutils_string_array_init(
                &topic_names_and_types->types[current_position],
                types_size, allocator))
            {
              ret = RMW_RET_ERROR;
              goto fini;
            }

            for (size_t k = 0; k < types_size; ++k) {
              topic_names_and_types->types[current_position].data[k] = allocator->zero_allocate(
                strlen(entity->types.data[k].data) + 1,
                sizeof(char), allocator->state);
              snprintf(
                topic_names_and_types->types[current_position].data[k],
                topic_names_and_types->types[current_position].capacity,
                "%s",
                entity->types.data[k].data);
            }
            break;
          }

        default:
          {
            break;
          }
      }
    }
  }

fini:
  micro_ros_msgs__msg__Graph__destroy(graph_data);
  return ret;
#else
  (void)node;
  (void)allocator;
  (void)no_demangle;
  (void)topic_names_and_types;
  RMW_UROS_TRACE_MESSAGE(
    "Function not available: enable RMW_UXRCE_GRAPH configuration profile before using");
  return RMW_RET_UNSUPPORTED;
#endif  // RMW_UXRCE_GRAPH
}
