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

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#define FLAGS_LEN	23

static bool * get_memory_lock(void *address)
{
  static bool memory_locks[FLAGS_LEN] = { 0 };
  uintptr_t a = (uintptr_t)(address);

  // Public domain hash function taken from http://burtleburtle.net/bob/hash/integer.html
  a = (a ^ 61) ^ (a >> 16);
  a = a + (a << 3);
  a = a ^ (a >> 4);
  a = a * 0x27d4eb2d;
  a = a ^ (a >> 15);

  a = a % FLAGS_LEN;
  return memory_locks + a;
}

void lock_memory(uint64_t *address){
  bool * memory_lock = get_memory_lock(address);

  while (__atomic_test_and_set(memory_lock, __ATOMIC_ACQUIRE) == 1);
}

void unlock_memory(uint64_t *address){
  bool * memory_lock = get_memory_lock(address);

  __atomic_clear(memory_lock, __ATOMIC_RELEASE);
}

uint64_t __atomic_load_8(uint64_t *mem, int model) { 
  (void) model;

  lock_memory(mem); 
  uint64_t ret = *mem; 
  unlock_memory(mem); 
  return ret; 
}

void __atomic_store_8(uint64_t *mem, uint64_t val, int model) { 
  (void) model;

  lock_memory(mem); 
  *mem = val; 
  unlock_memory(mem); 
}

uint64_t __atomic_exchange_8(uint64_t *mem, uint64_t val, int model) { 
  (void) model;

  lock_memory(mem); 
  uint64_t ret = *mem; 
  *mem = val; 
  unlock_memory(mem); 
  return ret; 
}

uint64_t __atomic_fetch_add_8(uint64_t *mem, uint64_t val, int model) { 
  (void) model;

  lock_memory(mem); 
  uint64_t ret = *mem; 
  *mem += val; 
  unlock_memory(mem); 
  return ret; 
}

#ifdef __cplusplus
}
#endif
