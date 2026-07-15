// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__FAILING_ALLOCATOR_FUNCTIONS_HPP_
#define RCL__FAILING_ALLOCATOR_FUNCTIONS_HPP_

#include <cstdlib>

extern "C"
{
inline
void *
failing_malloc(size_t size, void * state)
{
  (void)size;
  (void)state;
  return nullptr;
}

inline
void *
failing_realloc(void * memory_in, size_t size, void * state)
{
  (void)memory_in;
  (void)size;
  (void)state;
  // this is the right fail case, i.e. if failed, the memory_in is not free'd
  // see reallocf() for this behavior fix
  return nullptr;
}

inline
void *
failing_calloc(size_t count, size_t size, void * state)
{
  (void)count;
  (void)size;
  (void)state;
  return nullptr;
}
}  // extern "C"

#endif  // RCL__FAILING_ALLOCATOR_FUNCTIONS_HPP_
