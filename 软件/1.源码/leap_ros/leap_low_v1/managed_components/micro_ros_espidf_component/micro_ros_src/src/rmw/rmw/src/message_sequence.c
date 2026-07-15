// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include "rmw/message_sequence.h"
#include "rmw/types.h"

rmw_message_sequence_t
rmw_get_zero_initialized_message_sequence(void)
{
  static rmw_message_sequence_t message_sequence = {
    .data = NULL,
    .size = 0u,
    .capacity = 0u,
    .allocator = NULL
  };

  return message_sequence;
}

rmw_ret_t
rmw_message_sequence_init(
  rmw_message_sequence_t * sequence,
  size_t size,
  rcutils_allocator_t * allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(sequence, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);

  void * data = NULL;
  if (size > 0u) {
    data = allocator->allocate(sizeof(void *) * size, allocator->state);
    if (NULL == data) {
      return RMW_RET_BAD_ALLOC;
    }
  }

  sequence->data = data;
  sequence->size = 0u;
  sequence->capacity = size;
  sequence->allocator = allocator;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_message_sequence_fini(rmw_message_sequence_t * sequence)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(sequence, RMW_RET_INVALID_ARGUMENT);

  if (NULL != sequence->data) {
    assert(sequence->capacity > 0u);
    RCUTILS_CHECK_ALLOCATOR(sequence->allocator, return RMW_RET_INVALID_ARGUMENT);
    sequence->allocator->deallocate(sequence->data, sequence->allocator->state);
  }

  sequence->data = NULL;
  sequence->size = 0u;
  sequence->capacity = 0u;
  sequence->allocator = NULL;

  return RMW_RET_OK;
}

rmw_message_info_sequence_t
rmw_get_zero_initialized_message_info_sequence(void)
{
  static rmw_message_info_sequence_t message_info_sequence = {
    .data = NULL,
    .size = 0u,
    .capacity = 0u,
    .allocator = NULL
  };

  return message_info_sequence;
}

rmw_ret_t
rmw_message_info_sequence_init(
  rmw_message_info_sequence_t * sequence,
  size_t size,
  rcutils_allocator_t * allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(sequence, RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(allocator, return RMW_RET_INVALID_ARGUMENT);

  rmw_message_info_t * data = NULL;
  if (size > 0u) {
    data = allocator->allocate(sizeof(rmw_message_info_t) * size, allocator->state);
    if (NULL == data) {
      return RMW_RET_BAD_ALLOC;
    }
  }
  sequence->data = data;
  sequence->size = 0u;
  sequence->capacity = size;
  sequence->allocator = allocator;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_message_info_sequence_fini(rmw_message_info_sequence_t * sequence)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(sequence, RMW_RET_INVALID_ARGUMENT);

  if (NULL != sequence->data) {
    assert(sequence->capacity > 0u);
    RCUTILS_CHECK_ALLOCATOR(sequence->allocator, return RMW_RET_INVALID_ARGUMENT);
    sequence->allocator->deallocate(sequence->data, sequence->allocator->state);
  }

  sequence->data = NULL;
  sequence->size = 0u;
  sequence->capacity = 0u;
  sequence->allocator = NULL;

  return RMW_RET_OK;
}
