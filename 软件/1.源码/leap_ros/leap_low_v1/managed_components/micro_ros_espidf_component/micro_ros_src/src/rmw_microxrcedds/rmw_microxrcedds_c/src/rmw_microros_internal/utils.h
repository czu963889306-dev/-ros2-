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

#ifndef RMW_MICROROS_INTERNAL__UTILS_H_
#define RMW_MICROROS_INTERNAL__UTILS_H_

#include <rmw/rmw.h>

#include "./rmw_microros_internal/types.h"

bool run_xrce_session(
  rmw_context_impl_t * context,
  uxrStreamId * target_stream,
  uint16_t requests,
  int timeout);

uxrQoS_t convert_qos_profile(const rmw_qos_profile_t * rmw_qos);

int generate_name(
  const uxrObjectId * id,
  char name[],
  size_t buffer_size);

bool generate_type_name(
  const message_type_support_callbacks_t * members,
  char type_name[],
  size_t buffer_size);

bool generate_topic_name(
  const char * topic_name,
  char * full_topic_name,
  size_t full_topic_name_size);

bool generate_service_types(
  const service_type_support_callbacks_t * members,
  char * request_type,
  char * reply_type,
  size_t buffer_size);

bool generate_service_topics(
  const char * service_name,
  char * request_topic,
  char * reply_topic,
  size_t buffer_size);

int build_service_xml(
  const char * service_name_id,
  const char * service_name,
  bool requester,
  const service_type_support_callbacks_t * members,
  const rmw_qos_profile_t * qos_policies,
  char xml[],
  size_t buffer_size);

int build_participant_xml(
  size_t domain_id,
  const char * participant_name,
  char xml[],
  size_t buffer_size);
int build_publisher_xml(
  const char * publisher_name,
  char xml[],
  size_t buffer_size);
int build_subscriber_xml(
  const char * subscriber_name,
  char xml[],
  size_t buffer_size);
int build_topic_xml(
  const char * topic_name,
  const message_type_support_callbacks_t * members,
  const rmw_qos_profile_t * qos_policies,
  char xml[],
  size_t buffer_size);
int build_datawriter_xml(
  const char * topic_name,
  const message_type_support_callbacks_t * members,
  const rmw_qos_profile_t * qos_policies,
  char xml[],
  size_t buffer_size);
int build_datareader_xml(
  const char * topic_name,
  const message_type_support_callbacks_t * members,
  const rmw_qos_profile_t * qos_policies,
  char xml[],
  size_t buffer_size);

bool build_participant_profile(
  char profile_name[],
  size_t buffer_size);
bool build_topic_profile(
  const char * topic_name,
  char profile_name[],
  size_t buffer_size);
bool build_datawriter_profile(
  const char * topic_name,
  char profile_name[],
  size_t buffer_size);
bool build_datareader_profile(
  const char * topic_name,
  char profile_name[],
  size_t buffer_size);

bool is_uxrce_rmw_identifier_valid(
  const char * id);

#endif  // RMW_MICROROS_INTERNAL__UTILS_H_
