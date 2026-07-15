// Copyright (c) 2021 - for information on the respective copyright owner
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

#include <micro_ros_utilities/string_utilities.h>

#include <stddef.h>
#include <string.h>

#include <rcutils/allocator.h>

rosidl_runtime_c__String micro_ros_string_utilities_init(const char * data)
{
  const rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rosidl_runtime_c__String ret;

  ret.size = strlen(data);
  ret.capacity = ret.size + 1;
  ret.data = allocator.allocate(ret.capacity, allocator.state);
  memset(ret.data, 0, ret.capacity);

  memcpy(ret.data, data, ret.size);

  return ret;
}

rosidl_runtime_c__String micro_ros_string_utilities_init_with_size(const size_t size)
{
  const rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rosidl_runtime_c__String ret;

  ret.size = 0;
  ret.capacity = size + 1;
  ret.data = allocator.allocate(ret.capacity, allocator.state);
  memset(ret.data, 0, ret.capacity);

  return ret;
}

rosidl_runtime_c__String micro_ros_string_utilities_set(
  const rosidl_runtime_c__String str,
  const char * data)
{
  const rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rosidl_runtime_c__String ret = str;
  ret.size = strlen(data);

  if (ret.size > ret.capacity) {
    ret.data = allocator.reallocate(ret.data, ret.size + 1, allocator.state);
    ret.capacity = ret.size + 1;
  }

  memcpy(ret.data, data, ret.size + 1);

  return ret;
}

const char * micro_ros_string_utilities_get_c_str(const rosidl_runtime_c__String str)
{
  return (const char *) str.data;
}


rosidl_runtime_c__String micro_ros_string_utilities_append(
  const rosidl_runtime_c__String str,
  const char * data)
{
  const rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rosidl_runtime_c__String ret = str;
  size_t append_size = strlen(data);

  if (append_size + ret.size >= ret.capacity) {
    ret.data = allocator.reallocate(ret.data, ret.size + append_size + 1, allocator.state);
    ret.capacity = ret.size + append_size + 1;
  }

  memcpy(&ret.data[ret.size], data, append_size + 1);

  ret.size += append_size;

  return ret;
}

rosidl_runtime_c__String micro_ros_string_utilities_remove_tail_chars(
  const rosidl_runtime_c__String str,
  const size_t n)
{
  rosidl_runtime_c__String ret = str;

  ret.size -= n;
  ret.data[ret.size] = '\0';

  return ret;
}


void micro_ros_string_utilities_destroy(rosidl_runtime_c__String * const str)
{
  const rcutils_allocator_t allocator = rcutils_get_default_allocator();

  allocator.deallocate(str->data, allocator.state);
  str->data = NULL;
  str->size = 0;
  str->capacity = 0;
}
