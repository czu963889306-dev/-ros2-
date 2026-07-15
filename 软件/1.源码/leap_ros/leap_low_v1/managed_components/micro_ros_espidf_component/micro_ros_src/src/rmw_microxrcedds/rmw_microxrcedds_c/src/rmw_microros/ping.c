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
#include <rmw/allocators.h>
#include <rmw/ret_types.h>

#include <uxr/client/client.h>
#include <uxr/client/util/ping.h>

#include "../rmw_microros_internal/rmw_uxrce_transports.h"
#include "../rmw_microros_internal/types.h"

extern rmw_uxrce_transport_params_t rmw_uxrce_transport_default_params;

rmw_ret_t rmw_uros_ping_agent(
  const int timeout_ms,
  const uint8_t attempts)
{
  bool success = false;

  if (!session_memory.is_initialized || NULL == session_memory.allocateditems) {
    // There is no session available to ping. Init transport is required.
#ifdef RMW_UXRCE_TRANSPORT_SERIAL
    uxrSerialTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_UDP)
    uxrUDPTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_TCP)
    uxrTCPTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_CUSTOM)
    uxrCustomTransport transport;
    transport.framing = rmw_uxrce_transport_default_params.framing;
    transport.args = rmw_uxrce_transport_default_params.args;
    transport.open = rmw_uxrce_transport_default_params.open_cb;
    transport.close = rmw_uxrce_transport_default_params.close_cb;
    transport.write = rmw_uxrce_transport_default_params.write_cb;
    transport.read = rmw_uxrce_transport_default_params.read_cb;
#endif /* ifdef RMW_UXRCE_TRANSPORT_SERIAL */
    rmw_ret_t ret = rmw_uxrce_transport_init(NULL, NULL, (void *)&transport);

    if (RMW_RET_OK != ret) {
      return ret;
    }

    success = uxr_ping_agent_attempts(&transport.comm, timeout_ms, attempts);
    CLOSE_TRANSPORT(&transport);
  } else {
    // There is a session available to ping. Using session.
    rmw_uxrce_mempool_item_t * item = session_memory.allocateditems;
    do {
      rmw_context_impl_t * context = (rmw_context_impl_t *)item->data;

      success = uxr_ping_agent_session(&context->session, timeout_ms, attempts);

      item = item->next;
    } while (NULL != item && !success);
  }

  return success ? RMW_RET_OK : RMW_RET_ERROR;
}

rmw_ret_t rmw_uros_ping_agent_options(
  const int timeout_ms,
  const uint8_t attempts,
  rmw_init_options_t * rmw_options)
{
  bool success = false;

#ifdef RMW_UXRCE_TRANSPORT_SERIAL
  uxrSerialTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_UDP)
  uxrUDPTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_TCP)
  uxrTCPTransport transport;
#elif defined(RMW_UXRCE_TRANSPORT_CUSTOM)
  uxrCustomTransport transport;
  transport.framing = rmw_options->impl->transport_params.framing;
  transport.args = rmw_options->impl->transport_params.args;
  transport.open = rmw_options->impl->transport_params.open_cb;
  transport.close = rmw_options->impl->transport_params.close_cb;
  transport.write = rmw_options->impl->transport_params.write_cb;
  transport.read = rmw_options->impl->transport_params.read_cb;
#endif /* ifdef RMW_UXRCE_TRANSPORT_SERIAL */
  rmw_ret_t ret = rmw_uxrce_transport_init(NULL, rmw_options->impl, (void *)&transport);

  if (RMW_RET_OK != ret) {
    return ret;
  }

  success = uxr_ping_agent_attempts(&transport.comm, timeout_ms, attempts);
  CLOSE_TRANSPORT(&transport);

  return success ? RMW_RET_OK : RMW_RET_ERROR;
}
