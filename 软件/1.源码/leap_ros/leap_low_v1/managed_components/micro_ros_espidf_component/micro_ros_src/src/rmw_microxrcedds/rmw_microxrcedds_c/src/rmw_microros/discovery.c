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

#include "../rmw_microros_internal/types.h"
#include "./rmw_microros_internal/error_handling_internal.h"

#ifdef UCLIENT_PROFILE_DISCOVERY
bool on_agent_found(
  const TransportLocator * locator,
  void * args)
{
  rmw_init_options_t * rmw_options = (rmw_init_options_t *)args;
  uxrIpProtocol ip_protocol;
  char ip[MAX_IP_LEN];
  char port_str[MAX_PORT_LEN];
  uint16_t port;

  uxr_locator_to_ip(locator, ip, sizeof(ip), &port, &ip_protocol);
  if (0 > snprintf(port_str, MAX_PORT_LEN, "%d", port)) {
    return false;
  }

  uxrUDPTransport transport;
  if (uxr_init_udp_transport(&transport, ip_protocol, ip, port_str)) {
    uxrSession session;
    uxr_init_session(&session, &transport.comm, rmw_options->impl->transport_params.client_key);
    if (uxr_create_session_retries(&session, 5)) {
      if (0 > snprintf(rmw_options->impl->transport_params.agent_port, MAX_PORT_LEN, "%d", port)) {
        return false;
      }
      snprintf(rmw_options->impl->transport_params.agent_address, MAX_IP_LEN, "%s", ip);
      uxr_delete_session(&session);
      return true;
    }
  }
  return false;
}

#endif /* ifdef UCLIENT_PROFILE_DISCOVERY */

rmw_ret_t rmw_uros_discover_agent(
  rmw_init_options_t * rmw_options)
{
#ifdef UCLIENT_PROFILE_DISCOVERY
  if (NULL == rmw_options) {
    RMW_UROS_TRACE_MESSAGE("Uninitialised rmw_init_options.")
    return RMW_RET_INVALID_ARGUMENT;
  }

  memset(rmw_options->impl->transport_params.agent_address, 0, MAX_IP_LEN);
  memset(rmw_options->impl->transport_params.agent_port, 0, MAX_PORT_LEN);

  uxr_discovery_agents_default(1, 1000, on_agent_found, (void *)rmw_options);

  return (strlen(rmw_options->impl->transport_params.agent_address) >
         0) ? RMW_RET_OK : RMW_RET_TIMEOUT;
#else
  (void)rmw_options;

  RMW_UROS_TRACE_MESSAGE("UCLIENT_PROFILE_DISCOVERY not set.")
  return RMW_RET_INVALID_ARGUMENT;
#endif /* ifdef UCLIENT_PROFILE_DISCOVERY */
}
