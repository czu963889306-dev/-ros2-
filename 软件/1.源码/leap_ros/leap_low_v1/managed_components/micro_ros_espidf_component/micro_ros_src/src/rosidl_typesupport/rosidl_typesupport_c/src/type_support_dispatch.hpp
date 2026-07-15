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

#ifndef TYPE_SUPPORT_DISPATCH_HPP_
#define TYPE_SUPPORT_DISPATCH_HPP_

#ifndef ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT
#include "./dynamic_support_dispatch.hpp"
#endif  // ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT

#include "rcutils/error_handling.h"
#include "rcutils/snprintf.h"
#include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/type_support_map.h"

namespace rosidl_typesupport_c
{

extern const char * typesupport_identifier;

template<typename TypeSupport>
const TypeSupport *
get_typesupport_handle_function(
  const TypeSupport * handle, const char * identifier)
{
  if (strcmp(handle->typesupport_identifier, identifier) == 0) {
    return handle;
  }

  if (handle->typesupport_identifier == rosidl_typesupport_c__typesupport_identifier) {
    const type_support_map_t * map = \
      static_cast<const type_support_map_t *>(handle->data);
    for (size_t i = 0; i < map->size; ++i) {
      if (strcmp(map->typesupport_identifier[i], identifier) != 0) {
        continue;
      }
      typedef const TypeSupport * (* funcSignature)(void);
#ifndef ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT
      void * sym = handle_shared_library_from_name(map, i, identifier);
      if (nullptr == sym) {
        continue;
      }
      funcSignature func = reinterpret_cast<funcSignature>(sym);
#else
      funcSignature func = reinterpret_cast<funcSignature>(map->data[i]);
#endif  // ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT
      const TypeSupport * ts = func();
      return ts;
    }
  }
  RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
    "Handle's typesupport identifier (%s) is not supported by this library",
    handle->typesupport_identifier);
  return nullptr;
}

}  // namespace rosidl_typesupport_c

#endif  // TYPE_SUPPORT_DISPATCH_HPP_
