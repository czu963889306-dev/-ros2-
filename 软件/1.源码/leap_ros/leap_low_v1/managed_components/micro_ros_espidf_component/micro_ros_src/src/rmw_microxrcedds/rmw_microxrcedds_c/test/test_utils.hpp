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

#ifndef TEST_UTILS_HPP_
#define TEST_UTILS_HPP_

#include <rosidl_runtime_c/message_type_support_struct.h>
#include <rosidl_runtime_c/service_type_support_struct.h>
#include <rosidl_typesupport_microxrcedds_c/message_type_support.h>
#include <rosidl_typesupport_microxrcedds_c/service_type_support.h>

#include <rmw/rmw.h>
#include <rmw/validate_namespace.h>
#include <rmw/validate_node_name.h>

#include <string>

typedef struct
{
  rosidl_message_type_support_t type_support;
  message_type_support_callbacks_t callbacks;
  std::string topic_name;
  std::string type_name;
  std::string message_namespace;
} dummy_type_support_t;

typedef struct
{
  rosidl_service_type_support_t type_support;
  service_type_support_callbacks_t callbacks;
  dummy_type_support_t request_members;
  dummy_type_support_t response_members;
  std::string service_name;
  std::string type_name;
  std::string message_namespace;
} dummy_service_type_support_t;

void ConfigureDummyTypeSupport(
  const char * type_name,
  const char * topic_name,
  const char * message_namespace,
  size_t id,
  dummy_type_support_t * dummy_type_support);

void ConfigureDummyServiceTypeSupport(
  const char * type_name,
  const char * service_name,
  const char * message_namespace,
  size_t id,
  dummy_service_type_support_t * dummy_type_support);


void ConfigureDefaultQOSPolices(
  rmw_qos_profile_t * dummy_qos_policies);


bool CheckErrorState();

#endif  // TEST_UTILS_HPP_
