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

#include <rmw/rmw.h>
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_ret_t
rmw_subscription_set_on_new_message_callback(
  rmw_subscription_t * subscription,
  rmw_event_callback_t callback,
  const void * user_data)
{
  (void) subscription;
  (void) callback;
  (void) user_data;

  RMW_UROS_TRACE_MESSAGE("function not implemented");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_service_set_on_new_request_callback(
  rmw_service_t * service,
  rmw_event_callback_t callback,
  const void * user_data)
{
  (void) service;
  (void) callback;
  (void) user_data;

  RMW_UROS_TRACE_MESSAGE("function not implemented");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_client_set_on_new_response_callback(
  rmw_client_t * client,
  rmw_event_callback_t callback,
  const void * user_data)
{
  (void) client;
  (void) callback;
  (void) user_data;

  RMW_UROS_TRACE_MESSAGE("function not implemented");
  return RMW_RET_UNSUPPORTED;
}

rmw_ret_t
rmw_event_set_callback(
  rmw_event_t * event,
  rmw_event_callback_t callback,
  const void * user_data)
{
  (void) event;
  (void) callback;
  (void) user_data;

  RMW_UROS_TRACE_MESSAGE("function not implemented");
  return RMW_RET_UNSUPPORTED;
}
