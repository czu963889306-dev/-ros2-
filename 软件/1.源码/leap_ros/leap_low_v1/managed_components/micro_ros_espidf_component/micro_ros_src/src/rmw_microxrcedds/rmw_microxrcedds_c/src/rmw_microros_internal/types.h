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

#ifndef RMW_MICROROS_INTERNAL__TYPES_H_
#define RMW_MICROROS_INTERNAL__TYPES_H_

#include <stddef.h>

#include <rmw/types.h>
#include <ucdr/microcdr.h>
#include <uxr/client/client.h>

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM
#include <uxr/client/profile/transport/custom/custom_transport.h>
#endif  // RMW_MICROROS_INTERNAL__RMW_UXRCE_TRANSPORT_CUSTOM

#include <rosidl_runtime_c/message_type_support_struct.h>
#include <rosidl_typesupport_microxrcedds_c/message_type_support.h>

#include <rosidl_runtime_c/service_type_support_struct.h>
#include <rosidl_typesupport_microxrcedds_c/service_type_support.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>

#include "./rmw_microros_internal/memory.h"

// RMW specific definitions
#ifdef RMW_UXRCE_GRAPH
typedef struct rmw_graph_info_t
{
  bool initialized;
  bool has_changed;
  rmw_context_impl_t * context;

  uxrObjectId participant_id;
  uxrObjectId subscriber_id;
  uxrObjectId topic_id;
  uxrObjectId datareader_id;

  uint8_t micro_buffer[RMW_UXRCE_MAX_INPUT_BUFFER_SIZE];
  size_t micro_buffer_length;

  const rosidl_message_type_support_t * graph_type_support;
} rmw_graph_info_t;
#endif  // RMW_MICROROS_INTERNAL__RMW_UXRCE_GRAPH

struct rmw_context_impl_s
{
  rmw_uxrce_mempool_item_t mem;

#if defined(RMW_UXRCE_TRANSPORT_SERIAL)
  uxrSerialTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_UDP)
  uxrUDPTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_TCP)
  uxrTCPTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_CUSTOM)
  uxrCustomTransport transport;
#endif  // if defined(RMW_UXRCE_TRANSPORT_SERIAL)
  uxrSession session;

#ifdef RMW_UXRCE_GRAPH
  rmw_graph_info_t graph_info;
#endif  // if RMW_UXRCE_GRAPH
  rmw_guard_condition_t graph_guard_condition;

  uxrStreamId reliable_input;
  uxrStreamId reliable_output;
  uxrStreamId best_effort_output;
  uxrStreamId best_effort_input;

  uxrStreamId * creation_stream;
  uxrStreamId * destroy_stream;
  int creation_timeout;
  int destroy_timeout;

  uint8_t input_reliable_stream_buffer[RMW_UXRCE_MAX_INPUT_BUFFER_SIZE];
  uint8_t output_reliable_stream_buffer[RMW_UXRCE_MAX_OUTPUT_BUFFER_SIZE];
  uint8_t output_best_effort_stream_buffer[RMW_UXRCE_MAX_TRANSPORT_MTU];

  uint16_t id_participant;
  uint16_t id_topic;
  uint16_t id_publisher;
  uint16_t id_datawriter;
  uint16_t id_subscriber;
  uint16_t id_datareader;
  uint16_t id_requester;
  uint16_t id_replier;

  bool need_to_be_ran;
};

typedef struct rmw_context_impl_s rmw_uxrce_session_t;

struct rmw_init_options_impl_s
{
  rmw_uxrce_mempool_item_t mem;

  struct rmw_uxrce_transport_params_t transport_params;
};

typedef struct rmw_init_options_impl_s rmw_uxrce_init_options_impl_t;

// ROS2 entities definitions

typedef struct rmw_uxrce_topic_t
{
  rmw_uxrce_mempool_item_t mem;

  uxrObjectId topic_id;
  const message_type_support_callbacks_t * message_type_support_callbacks;

  struct rmw_uxrce_node_t * owner_node;
} rmw_uxrce_topic_t;

typedef struct rmw_uxrce_service_t
{
  rmw_uxrce_mempool_item_t mem;
  uxrObjectId service_id;
  const service_type_support_callbacks_t * type_support_callbacks;
  uint16_t service_data_resquest;

  rmw_qos_profile_t qos;

  uxrStreamId stream_id;
  int session_timeout;
  struct rmw_uxrce_node_t * owner_node;

  rmw_service_t rmw_service;
  char service_name[RMW_UXRCE_TOPIC_NAME_MAX_LENGTH];
} rmw_uxrce_service_t;

typedef struct rmw_uxrce_client_t
{
  rmw_uxrce_mempool_item_t mem;
  uxrObjectId client_id;
  const service_type_support_callbacks_t * type_support_callbacks;
  uint16_t client_data_request;

  rmw_qos_profile_t qos;

  uxrStreamId stream_id;
  int session_timeout;
  struct rmw_uxrce_node_t * owner_node;

  rmw_client_t rmw_client;
  char service_name[RMW_UXRCE_TOPIC_NAME_MAX_LENGTH];
} rmw_uxrce_client_t;

typedef struct rmw_uxrce_subscription_t
{
  rmw_uxrce_mempool_item_t mem;
  uxrObjectId subscriber_id;
  uxrObjectId datareader_id;

  const message_type_support_callbacks_t * type_support_callbacks;
  struct rmw_uxrce_topic_t * topic;

  struct rmw_uxrce_node_t * owner_node;
  rmw_qos_profile_t qos;
  uxrStreamId stream_id;

  rmw_subscription_t rmw_subscription;
  char topic_name[RMW_UXRCE_TOPIC_NAME_MAX_LENGTH];
} rmw_uxrce_subscription_t;

typedef struct rmw_uxrce_publisher_t
{
  rmw_uxrce_mempool_item_t mem;
  uxrObjectId publisher_id;
  uxrObjectId datawriter_id;

  const message_type_support_callbacks_t * type_support_callbacks;

  rmw_uros_continous_serialization_size cs_cb_size;
  rmw_uros_continous_serialization cs_cb_serialization;

  struct rmw_uxrce_topic_t * topic;

  rmw_qos_profile_t qos;
  uxrStreamId stream_id;
  int session_timeout;

  struct rmw_uxrce_node_t * owner_node;

  rmw_publisher_t rmw_publisher;
  char topic_name[RMW_UXRCE_TOPIC_NAME_MAX_LENGTH];
} rmw_uxrce_publisher_t;

typedef struct rmw_uxrce_node_t
{
  rmw_uxrce_mempool_item_t mem;
  rmw_uxrce_session_t * context;

  uxrObjectId participant_id;

  rmw_node_t rmw_node;
  char node_name[RMW_UXRCE_NODE_NAME_MAX_LENGTH];
  char node_namespace[RMW_UXRCE_NODE_NAME_MAX_LENGTH];
} rmw_uxrce_node_t;

#define RMW_UXRCE_QOS_LIFESPAN_DEFAULT {30LL, 0LL}

typedef enum rmw_uxrce_entity_type_t
{
  RMW_UXRCE_ENTITY_TYPE_UNKNOWN = 0,
  RMW_UXRCE_ENTITY_TYPE_NODE,
  RMW_UXRCE_ENTITY_TYPE_TOPIC,
  RMW_UXRCE_ENTITY_TYPE_SERVICE,
  RMW_UXRCE_ENTITY_TYPE_CLIENT,
  RMW_UXRCE_ENTITY_TYPE_SUBSCRIPTION,
  RMW_UXRCE_ENTITY_TYPE_PUBLISHER
} rmw_uxrce_entity_type_t;

typedef struct rmw_uxrce_static_input_buffer_t
{
  rmw_uxrce_mempool_item_t mem;

  uint8_t buffer[RMW_UXRCE_MAX_INPUT_BUFFER_SIZE];
  size_t length;
  void * owner;

  int64_t timestamp;
  rmw_uxrce_entity_type_t entity_type;

  union {
    int64_t reply_id;
    SampleIdentity sample_id;
  } related;
} rmw_uxrce_static_input_buffer_t;

typedef struct rmw_uxrce_wait_set_t
{
  rmw_uxrce_mempool_item_t mem;

  rmw_wait_set_t rmw_wait_set;
} rmw_uxrce_wait_set_t;

typedef struct rmw_uxrce_guard_condition_t
{
  rmw_uxrce_mempool_item_t mem;

  bool hasTriggered;

  rmw_guard_condition_t rmw_guard_condition;
} rmw_uxrce_guard_condition_t;

// Static memory pools

extern char rmw_uxrce_entity_naming_buffer[RMW_UXRCE_ENTITY_NAMING_BUFFER_LENGTH];

extern rmw_uxrce_mempool_t session_memory;
extern rmw_uxrce_session_t custom_sessions[RMW_UXRCE_MAX_SESSIONS];

extern rmw_uxrce_mempool_t node_memory;
extern rmw_uxrce_node_t custom_nodes[RMW_UXRCE_MAX_NODES];

extern rmw_uxrce_mempool_t publisher_memory;
extern rmw_uxrce_publisher_t custom_publishers[RMW_UXRCE_MAX_PUBLISHERS];

extern rmw_uxrce_mempool_t subscription_memory;
extern rmw_uxrce_subscription_t custom_subscriptions[RMW_UXRCE_MAX_SUBSCRIPTIONS];

extern rmw_uxrce_mempool_t service_memory;
extern rmw_uxrce_service_t custom_services[RMW_UXRCE_MAX_SERVICES];

extern rmw_uxrce_mempool_t client_memory;
extern rmw_uxrce_client_t custom_clients[RMW_UXRCE_MAX_CLIENTS];

extern rmw_uxrce_mempool_t topics_memory;
extern rmw_uxrce_topic_t custom_topics[RMW_UXRCE_MAX_TOPICS_INTERNAL];

extern rmw_uxrce_mempool_t static_buffer_memory;
extern rmw_uxrce_static_input_buffer_t custom_static_buffers[RMW_UXRCE_MAX_HISTORY];

extern rmw_uxrce_mempool_t init_options_memory;
extern rmw_uxrce_init_options_impl_t custom_init_options[RMW_UXRCE_MAX_OPTIONS];

extern rmw_uxrce_mempool_t wait_set_memory;
extern rmw_uxrce_wait_set_t custom_wait_set[RMW_UXRCE_MAX_WAIT_SETS];

extern rmw_uxrce_mempool_t guard_condition_memory;
extern rmw_uxrce_guard_condition_t custom_guard_condition[RMW_UXRCE_MAX_GUARD_CONDITION];

// Global mutexs
#ifdef UCLIENT_PROFILE_MULTITHREAD
// This mutex protects `need_to_be_ran` member of `session_memory` elements
// between concurrent calls to `rmw_wait()`
extern uxrMutex rmw_uxrce_wait_mutex;
extern bool rmw_uxrce_wait_mutex_initialized;
#endif  // UCLIENT_PROFILE_MULTITHREAD

// Memory init functions

#define RMW_INIT_DEFINE_MEMORY(X) \
  void rmw_uxrce_init_ ## X ## _memory( \
    rmw_uxrce_mempool_t * memory, \
    rmw_uxrce_ ## X ## _t * array, \
    size_t size);

RMW_INIT_DEFINE_MEMORY(service)
RMW_INIT_DEFINE_MEMORY(client)
RMW_INIT_DEFINE_MEMORY(publisher)
RMW_INIT_DEFINE_MEMORY(subscription)
RMW_INIT_DEFINE_MEMORY(node)
RMW_INIT_DEFINE_MEMORY(session)
RMW_INIT_DEFINE_MEMORY(topic)
RMW_INIT_DEFINE_MEMORY(static_input_buffer)
RMW_INIT_DEFINE_MEMORY(init_options_impl)
RMW_INIT_DEFINE_MEMORY(wait_set)
RMW_INIT_DEFINE_MEMORY(guard_condition)

// Memory management functions

void rmw_uxrce_fini_session_memory(
  rmw_context_impl_t * session);
void rmw_uxrce_fini_node_memory(
  rmw_node_t * node);
void rmw_uxrce_fini_publisher_memory(
  rmw_publisher_t * publisher);
void rmw_uxrce_fini_subscription_memory(
  rmw_subscription_t * subscriber);
void rmw_uxrce_fini_client_memory(
  rmw_client_t * client);
void rmw_uxrce_fini_service_memory(
  rmw_service_t * service);
void rmw_uxrce_fini_topic_memory(
  rmw_uxrce_topic_t * topic);

// Memory pools functions

rmw_uxrce_mempool_item_t * rmw_uxrce_get_static_input_buffer_for_entity(
  void * entity,
  const rmw_qos_profile_t qos);
rmw_uxrce_mempool_item_t * rmw_uxrce_find_static_input_buffer_by_owner(
  void * owner);
void rmw_uxrce_clean_expired_static_input_buffer(void);

#endif  // RMW_MICROROS_INTERNAL__TYPES_H_
