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
#ifndef RMW_MICROROS_INTERNAL__RMW_UXRCE_TRANSPORTS_H_
#define RMW_MICROROS_INTERNAL__RMW_UXRCE_TRANSPORTS_H_

#include <uxr/client/client.h>
#include <rmw_microxrcedds_c/config.h>

#ifdef RMW_UXRCE_TRANSPORT_CUSTOM
#include <uxr/client/profile/transport/custom/custom_transport.h>
#endif  // RMW_MICROROS_INTERNAL__ RMW_UXRCE_TRANSPORT_CUSTOM

#ifdef RMW_UXRCE_TRANSPORT_SERIAL
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#endif  // RMW_MICROROS_INTERNAL__ RMW_UXRCE_TRANSPORT_SERIAL

#include "./rmw_microros_internal/types.h"

/**
 * @brief   Takes care of initializing the micro XRCE-DDS transport layer,
 *          used for communicating with the Agent.
 * @param   context The RMW context, holding in its implementation the
 *          XRCE transport entity, if it has been initialized.
 * @param   init_options Pointer to struct holding the provided init options for
 *          micro-ROS.
 * @param   override_transport Alternative opaque transport structure, which will
 *          must come as NULL and will be given as a handle to the user.
 *          This structure is filled when the context_impl pointer is NULL.
 *          Useful for some utilities, such as the "ping" functionality.
 * returns  RMW_RET_OK when success.
 * returns  RMW_RET_ERROR is something bad happened during transport initialization.
 * returns  RMW_RET_INVALID_ARGUMENT
 *          if some argument was not initialized as expected.
 */
rmw_ret_t rmw_uxrce_transport_init(
  rmw_context_impl_t * context,
  rmw_uxrce_init_options_impl_t * init_options,
  void * override_transport);

/**
 * @brief   Helper macros for closing an open micro XRCE-DDS transport.
 * @param   transport The uxrXXXTransport struct pointer used to close the connection.
 */
#if defined(RMW_UXRCE_TRANSPORT_SERIAL)
#define CLOSE_TRANSPORT(transport)    uxr_close_serial_transport(transport)
#elif defined(RMW_UXRCE_TRANSPORT_UDP)
#define CLOSE_TRANSPORT(transport)    uxr_close_udp_transport(transport)
#elif defined(RMW_UXRCE_TRANSPORT_CUSTOM)
#define CLOSE_TRANSPORT(transport)    uxr_close_custom_transport(transport)
#else
#define CLOSE_TRANSPORT(transport)
#endif  // defined(RMW_UXRCE_TRANSPORT_SERIAL)

#endif  // RMW_MICROROS_INTERNAL__RMW_UXRCE_TRANSPORTS_H_
