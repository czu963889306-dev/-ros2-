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

#ifndef RMW_MICROROS_INTERNAL__CALLBACKS_H_
#define RMW_MICROROS_INTERNAL__CALLBACKS_H_

#include <rmw_microxrcedds_c/config.h>

#include "./rmw_microros_internal/types.h"

void on_status(
  struct uxrSession * session,
  uxrObjectId object_id,
  uint16_t request_id,
  uint8_t status,
  void * args);

void on_topic(
  struct uxrSession * session,
  uxrObjectId object_id,
  uint16_t request_id,
  uxrStreamId stream_id,
  struct ucdrBuffer * ub,
  uint16_t length,
  void * args);

void on_request(
  struct uxrSession * session,
  uxrObjectId object_id,
  uint16_t request_id,
  SampleIdentity * sample_id,
  struct ucdrBuffer * ub,
  uint16_t length,
  void * args);

void on_reply(
  struct uxrSession * session,
  uxrObjectId object_id,
  uint16_t request_id,
  uint16_t reply_id,
  struct ucdrBuffer * ub,
  uint16_t length,
  void * args);

#endif  // RMW_MICROROS_INTERNAL__CALLBACKS_H_
