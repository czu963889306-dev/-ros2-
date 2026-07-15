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

#ifndef RMW_MICROROS_INTERNAL__RMW_GRAPH_H_
#define RMW_MICROROS_INTERNAL__RMW_GRAPH_H_

#include <rmw/types.h>
#include <uxr/client/client.h>
#include <micro_ros_msgs/msg/graph.h>
#include <micro_ros_msgs/msg/node.h>
#include <micro_ros_msgs/msg/entity.h>

#include "./rmw_microros_internal/types.h"

rmw_ret_t rmw_graph_init(
  rmw_context_impl_t * context,
  rmw_graph_info_t * graph_info);

rmw_ret_t rmw_graph_fill_data_from_buffer(
  rmw_graph_info_t * graph_info,
  micro_ros_msgs__msg__Graph * graph_data);

#endif  // RMW_MICROROS_INTERNAL__RMW_GRAPH_H_
