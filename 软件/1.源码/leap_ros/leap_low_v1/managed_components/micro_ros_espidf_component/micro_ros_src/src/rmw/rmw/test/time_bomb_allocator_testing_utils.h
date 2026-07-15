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

#ifndef TIME_BOMB_ALLOCATOR_TESTING_UTILS_H_
#define TIME_BOMB_ALLOCATOR_TESTING_UTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#include "rcutils/allocator.h"

typedef struct __time_bomb_allocator_state
{
  // Set these to negative if you want to disable time bomb for the associated function call.
  int malloc_count_until_failure;
  int realloc_count_until_failure;
  int free_count_until_failure;
  int calloc_count_until_failure;
} __time_bomb_allocator_state;

static void *
time_bomb_malloc(size_t size, void * state)
{
  if (((__time_bomb_allocator_state *)state)->malloc_count_until_failure >= 0 &&
    ((__time_bomb_allocator_state *)state)->malloc_count_until_failure-- == 0)
  {
    printf("Malloc time bomb countdown reached 0, returning nullptr\n");
    return nullptr;
  }
  return rcutils_get_default_allocator().allocate(size, rcutils_get_default_allocator().state);
}

static void *
time_bomb_realloc(void * pointer, size_t size, void * state)
{
  if (((__time_bomb_allocator_state *)state)->realloc_count_until_failure >= 0 &&
    ((__time_bomb_allocator_state *)state)->realloc_count_until_failure-- == 0)
  {
    printf("Realloc time bomb countdown reached 0, returning nullptr\n");
    return nullptr;
  }
  return rcutils_get_default_allocator().reallocate(
    pointer, size, rcutils_get_default_allocator().state);
}

static void
time_bomb_free(void * pointer, void * state)
{
  if (((__time_bomb_allocator_state *)state)->free_count_until_failure >= 0 &&
    ((__time_bomb_allocator_state *)state)->free_count_until_failure-- == 0)
  {
    printf("Free time bomb countdown reached 0, not freeing memory\n");
    return;
  }
  rcutils_get_default_allocator().deallocate(pointer, rcutils_get_default_allocator().state);
}

static void *
time_bomb_calloc(size_t number_of_elements, size_t size_of_element, void * state)
{
  if (((__time_bomb_allocator_state *)state)->calloc_count_until_failure >= 0 &&
    ((__time_bomb_allocator_state *)state)->calloc_count_until_failure-- == 0)
  {
    printf("Calloc time bomb countdown reached 0, returning nullptr\n");
    return nullptr;
  }
  return rcutils_get_default_allocator().zero_allocate(
    number_of_elements, size_of_element, rcutils_get_default_allocator().state);
}

/**
 * This allocator uses the rcutils default allocator functions, but decrements a time bomb counter
 * for each function call. When the counter reaches 0, that call will fail.
 * In the case of the allocating functions, it will return a nullptr. In the case of free,
 * it will fail to free the memory.
 *
 * Use this allocator when you need a fixed amount of calls to succeed before it fails.
 *
 * Set the count to negative for the time bomb effect to be disabled for that function.
 */
static inline rcutils_allocator_t
get_time_bomb_allocator(void)
{
  static __time_bomb_allocator_state state;
  state.malloc_count_until_failure = -1;
  state.realloc_count_until_failure = -1;
  state.free_count_until_failure = -1;
  state.calloc_count_until_failure = -1;
  auto time_bomb_allocator = rcutils_get_default_allocator();
  time_bomb_allocator.allocate = time_bomb_malloc;
  time_bomb_allocator.deallocate = time_bomb_free;
  time_bomb_allocator.reallocate = time_bomb_realloc;
  time_bomb_allocator.zero_allocate = time_bomb_calloc;
  time_bomb_allocator.state = &state;
  return time_bomb_allocator;
}

/**
 * Set count to the number of times you want the call to succeed before it fails.
 * After it fails once, it will succeed until this count is reset.
 * Set it to a negative value to disable the time bomb effect for that function.
 */
static inline void
set_time_bomb_allocator_malloc_count(rcutils_allocator_t & time_bomb_allocator, int count)
{
  ((__time_bomb_allocator_state *)time_bomb_allocator.state)->malloc_count_until_failure = count;
}

static inline void
set_time_bomb_allocator_realloc_count(rcutils_allocator_t & time_bomb_allocator, int count)
{
  ((__time_bomb_allocator_state *)time_bomb_allocator.state)->realloc_count_until_failure = count;
}

static inline void
set_time_bomb_allocator_free_count(rcutils_allocator_t & time_bomb_allocator, int count)
{
  ((__time_bomb_allocator_state *)time_bomb_allocator.state)->free_count_until_failure = count;
}

static inline void
set_time_bomb_allocator_calloc_count(rcutils_allocator_t & time_bomb_allocator, int count)
{
  ((__time_bomb_allocator_state *)time_bomb_allocator.state)->calloc_count_until_failure = count;
}

#ifdef __cplusplus
}
#endif

#endif  // TIME_BOMB_ALLOCATOR_TESTING_UTILS_H_
