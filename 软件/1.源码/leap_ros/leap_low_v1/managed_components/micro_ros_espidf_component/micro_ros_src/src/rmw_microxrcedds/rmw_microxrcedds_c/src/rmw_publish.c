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
#include <rmw_microros/rmw_microros.h>
#include <uxr/client/profile/multithread/multithread.h>

#include "./rmw_microros_internal/types.h"
#include "./rmw_microros_internal/utils.h"
#include "./rmw_microros_internal/error_handling_internal.h"

bool flush_session(
  uxrSession * session,
  void * args)
{
  rmw_uxrce_publisher_t * custom_publisher = (rmw_uxrce_publisher_t *)args;
  return uxr_run_session_until_confirm_delivery(session, custom_publisher->session_timeout);
}

rmw_ret_t
rmw_publish(
  const rmw_publisher_t * publisher,
  const void * ros_message,
  rmw_publisher_allocation_t * allocation)
{
  (void)allocation;
  rmw_ret_t ret = RMW_RET_OK;
  if (!publisher) {
    RMW_UROS_TRACE_MESSAGE("publisher pointer is null")
    ret = RMW_RET_ERROR;
  } else if (!ros_message) {
    RMW_UROS_TRACE_MESSAGE("ros_message pointer is null")
    ret = RMW_RET_ERROR;
  } else if (!is_uxrce_rmw_identifier_valid(publisher->implementation_identifier)) {
    RMW_UROS_TRACE_MESSAGE("publisher handle not from this implementation")
    ret = RMW_RET_ERROR;
  } else if (!publisher->data) {
    RMW_UROS_TRACE_MESSAGE("publisher imp is null");
    ret = RMW_RET_ERROR;
  } else {
    rmw_uxrce_publisher_t * custom_publisher = (rmw_uxrce_publisher_t *)publisher->data;
    const message_type_support_callbacks_t * functions = custom_publisher->type_support_callbacks;
    uint32_t topic_length = functions->get_serialized_size(ros_message);

    if (custom_publisher->cs_cb_size) {
      custom_publisher->cs_cb_size(&topic_length);
    }

    ucdrBuffer mb;
    bool written = false;
    if (uxr_prepare_output_stream(
        &custom_publisher->owner_node->context->session,
        custom_publisher->stream_id, custom_publisher->datawriter_id, &mb,
        topic_length) ||
      uxr_prepare_output_stream_fragmented(
        &custom_publisher->owner_node->context->session,
        custom_publisher->stream_id, custom_publisher->datawriter_id, &mb,
        topic_length, flush_session, custom_publisher))
    {
      written = functions->cdr_serialize(ros_message, &mb);
      if (custom_publisher->cs_cb_serialization) {
        custom_publisher->cs_cb_serialization(&mb);
      }

      UXR_UNLOCK_STREAM_ID(
        &custom_publisher->owner_node->context->session,
        custom_publisher->stream_id);

      if (UXR_BEST_EFFORT_STREAM == custom_publisher->stream_id.type) {
        uxr_flash_output_streams(&custom_publisher->owner_node->context->session);
      } else {
        written &= uxr_run_session_until_confirm_delivery(
          &custom_publisher->owner_node->context->session, custom_publisher->session_timeout);
      }
    }
    if (!written) {
      RMW_UROS_TRACE_MESSAGE("error publishing message")
      ret = RMW_RET_ERROR;
    }
  }
  return ret;
}

rmw_ret_t
rmw_publish_serialized_message(
  const rmw_publisher_t * publisher,
  const rmw_serialized_message_t * serialized_message,
  rmw_publisher_allocation_t * allocation)
{
  (void)publisher;
  (void)serialized_message;
  (void)allocation;
  RMW_UROS_TRACE_MESSAGE("function not implemented")
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_publish_loaned_message(
  const rmw_publisher_t * publisher,
  void * ros_message,
  rmw_publisher_allocation_t * allocation)
{
  (void)publisher;
  (void)ros_message;
  (void)allocation;

  RMW_UROS_TRACE_MESSAGE("function not implemented")
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_publisher_wait_for_all_acked(
  const rmw_publisher_t * publisher,
  rmw_time_t wait_timeout)
{
  (void)publisher;
  (void)wait_timeout;

  RMW_SET_ERROR_MSG("function not implemented");
  return RMW_RET_UNSUPPORTED;
}
