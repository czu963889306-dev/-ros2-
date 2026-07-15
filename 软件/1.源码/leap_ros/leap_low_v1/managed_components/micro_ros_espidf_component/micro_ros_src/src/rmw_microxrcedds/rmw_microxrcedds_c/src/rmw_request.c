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
#include <uxr/client/profile/multithread/multithread.h>
#include <rmw_microxrcedds_c/rmw_c_macros.h>

#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_ret_t
rmw_send_request(
  const rmw_client_t * client,
  const void * ros_request,
  int64_t * sequence_id)
{
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    client->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)client->data;
  rmw_uxrce_node_t * custom_node = custom_client->owner_node;

  const rosidl_message_type_support_t * req_members =
    custom_client->type_support_callbacks->request_members_();
  const message_type_support_callbacks_t * functions =
    (const message_type_support_callbacks_t *)req_members->data;

  ucdrBuffer mb;
  uint32_t request_length = functions->get_serialized_size(ros_request);
  *sequence_id = uxr_prepare_output_stream(
    &custom_node->context->session,
    custom_client->stream_id, custom_client->client_id, &mb,
    request_length);

  if (UXR_INVALID_REQUEST_ID == *sequence_id) {
    RMW_UROS_TRACE_MESSAGE("Micro XRCE-DDS service request error.")
    return RMW_RET_ERROR;
  }

  functions->cdr_serialize(ros_request, &mb);

  UXR_UNLOCK_STREAM_ID(&custom_node->context->session, custom_client->stream_id);

  if (UXR_BEST_EFFORT_STREAM == custom_client->stream_id.type) {
    uxr_flash_output_streams(&custom_node->context->session);
  } else {
    uxr_run_session_until_confirm_delivery(
      &custom_node->context->session, custom_client->session_timeout);
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_take_request(
  const rmw_service_t * service,
  rmw_service_info_t * request_header,
  void * ros_request,
  bool * taken)
{
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    service->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  if (taken != NULL) {
    *taken = false;
  }

  rmw_uxrce_service_t * custom_service = (rmw_uxrce_service_t *)service->data;

  rmw_uxrce_clean_expired_static_input_buffer();

  UXR_LOCK(&static_buffer_memory.mutex);

  // Find first related item in static buffer memory pool
  rmw_uxrce_mempool_item_t * static_buffer_item =
    rmw_uxrce_find_static_input_buffer_by_owner((void *) custom_service);
  if (static_buffer_item == NULL) {
    UXR_UNLOCK(&static_buffer_memory.mutex);
    return RMW_RET_ERROR;
  }

  rmw_uxrce_static_input_buffer_t * static_buffer =
    (rmw_uxrce_static_input_buffer_t *)static_buffer_item->data;

  // Conversion from SampleIdentity to rmw_request_id_t
  request_header->request_id.sequence_number =
    (((int64_t)static_buffer->related.sample_id.sequence_number.high) << 32) |
    static_buffer->related.sample_id.sequence_number.low;
  request_header->request_id.writer_guid[0] =
    (int8_t)static_buffer->related.sample_id.writer_guid.entityId.entityKind;

  memcpy(
    &request_header->request_id.writer_guid[1],
    static_buffer->related.sample_id.writer_guid.entityId.entityKey,
    3);
  memcpy(
    &request_header->request_id.writer_guid[4],
    static_buffer->related.sample_id.writer_guid.guidPrefix.data, 12);

  const rosidl_message_type_support_t * req_members =
    custom_service->type_support_callbacks->request_members_();
  const message_type_support_callbacks_t * functions =
    (const message_type_support_callbacks_t *)req_members->data;

  ucdrBuffer temp_buffer;
  ucdr_init_buffer(
    &temp_buffer,
    static_buffer->buffer,
    static_buffer->length);

  bool deserialize_rv = functions->cdr_deserialize(&temp_buffer, ros_request);

  put_memory(&static_buffer_memory, static_buffer_item);

  UXR_UNLOCK(&static_buffer_memory.mutex);

  if (taken != NULL) {
    *taken = deserialize_rv;
  }

  if (!deserialize_rv) {
    RMW_UROS_TRACE_MESSAGE("Typesupport desserialize error.")
    return RMW_RET_ERROR;
  }

  return RMW_RET_OK;
}
