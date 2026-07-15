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

#ifndef RMW_MICROROS_INTERNAL__RMW_MICROXRCEDDS_TOPIC_H_
#define RMW_MICROROS_INTERNAL__RMW_MICROXRCEDDS_TOPIC_H_

#include <stddef.h>
#include <uxr/client/client.h>

#include "./rmw_microros_internal/types.h"

#if defined(__cplusplus)
extern "C"
{
#endif  // if defined(__cplusplus)

rmw_uxrce_topic_t *
create_topic(
  struct rmw_uxrce_node_t * custom_node,
  const char * topic_name,
  const message_type_support_callbacks_t * message_type_support_callbacks,
  const rmw_qos_profile_t * qos_policies);

rmw_ret_t destroy_topic(
  rmw_uxrce_topic_t * topic);
size_t topic_count(
  rmw_uxrce_node_t * custom_node);

#if defined(__cplusplus)
}
#endif  // if defined(__cplusplus)

#endif  // RMW_MICROROS_INTERNAL__RMW_MICROXRCEDDS_TOPIC_H_
