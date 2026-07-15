// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <rmw_microxrcedds_c/config.h>
#include <rmw/rmw.h>
#include <rmw/error_handling.h>
#include <rmw/allocators.h>
#include <rmw/ret_types.h>

#include "../rmw_microros_internal/types.h"

rmw_ret_t rmw_uros_set_publisher_session_timeout(
  rmw_publisher_t * publisher,
  int session_timeout)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(publisher, RMW_RET_INVALID_ARGUMENT);
  rmw_uxrce_publisher_t * custom_publisher = (rmw_uxrce_publisher_t *)publisher->data;

  custom_publisher->session_timeout = session_timeout;
  return RMW_RET_OK;
}

rmw_ret_t rmw_uros_set_service_session_timeout(
  rmw_service_t * service,
  int session_timeout)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(service, RMW_RET_INVALID_ARGUMENT);
  rmw_uxrce_service_t * custom_service = (rmw_uxrce_service_t *)service->data;

  custom_service->session_timeout = session_timeout;
  return RMW_RET_OK;
}

rmw_ret_t rmw_uros_set_client_session_timeout(
  rmw_client_t * client,
  int session_timeout)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(client, RMW_RET_INVALID_ARGUMENT);
  rmw_uxrce_client_t * custom_client = (rmw_uxrce_client_t *)client->data;

  custom_client->session_timeout = session_timeout;
  return RMW_RET_OK;
}

rmw_ret_t rmw_uros_set_context_entity_creation_session_timeout(
  rmw_context_t * context,
  int session_timeout)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(context, RMW_RET_INVALID_ARGUMENT);
  rmw_uxrce_session_t * custom_session = (rmw_uxrce_session_t *)context->impl;

  rmw_context_impl_t * context_impl = (rmw_context_impl_t *) context->impl;
  custom_session->creation_stream = session_timeout <= 0 ?
    &context_impl->best_effort_output : &context_impl->reliable_output;

  custom_session->creation_timeout = session_timeout;
  return RMW_RET_OK;
}

rmw_ret_t rmw_uros_set_context_entity_destroy_session_timeout(
  rmw_context_t * context,
  int session_timeout)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(context, RMW_RET_INVALID_ARGUMENT);
  rmw_uxrce_session_t * custom_session = (rmw_uxrce_session_t *)context->impl;

  rmw_context_impl_t * context_impl = (rmw_context_impl_t *) context->impl;
  custom_session->destroy_stream = session_timeout <= 0 ?
    &context_impl->best_effort_output : &context_impl->reliable_output;

  custom_session->destroy_timeout = session_timeout;
  return RMW_RET_OK;
}
