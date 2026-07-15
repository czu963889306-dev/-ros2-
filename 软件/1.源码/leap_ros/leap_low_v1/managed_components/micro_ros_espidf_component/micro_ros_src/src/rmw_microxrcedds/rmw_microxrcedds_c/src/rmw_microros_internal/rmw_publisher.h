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

#ifndef RMW_MICROROS_INTERNAL__RMW_PUBLISHER_H_
#define RMW_MICROROS_INTERNAL__RMW_PUBLISHER_H_

#include <rmw/types.h>
#include <rosidl_runtime_c/message_type_support_struct.h>

rmw_publisher_t * create_publisher(
  const rmw_node_t * node,
  const rosidl_message_type_support_t * type_support,
  const char * topic_name,
  const rmw_qos_profile_t * qos_policies);

#endif  // RMW_MICROROS_INTERNAL__RMW_PUBLISHER_H_
