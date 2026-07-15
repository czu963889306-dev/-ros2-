// Copyright 2021 Open Source Robotics Foundation, Inc.
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

#ifndef DYNAMIC_SUPPORT_DISPATCH_HPP_
#define DYNAMIC_SUPPORT_DISPATCH_HPP_

#include <cstddef>
#include <cstdio>
#include <cstring>

#include <memory>
#include <stdexcept>
#include <list>
#include <string>

#include "rcpputils/shared_library.hpp"
#include "rcutils/error_handling.h"
#include "rcutils/snprintf.h"
#include "rosidl_typesupport_c/type_support_map.h"

namespace rosidl_typesupport_c
{

static void *
handle_shared_library_from_name(
  const type_support_map_t * map,
  size_t map_item,
  const char * identifier
)
{
  rcpputils::SharedLibrary * lib = nullptr;

  if (!map->data[map_item]) {
    char library_basename[1024];
    int ret = rcutils_snprintf(
      library_basename, 1023, "%s__%s",
      map->package_name, identifier);
    if (ret < 0) {
      RCUTILS_SET_ERROR_MSG("Failed to format library name");
      return nullptr;
    }

    std::string library_name;
    try {
      library_name = rcpputils::get_platform_library_name(library_basename);
    } catch (const std::exception & e) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Failed to compute library name for '%s' due to %s",
        library_basename, e.what());
      return nullptr;
    }

    try {
      lib = new rcpputils::SharedLibrary(library_name);
    } catch (const std::runtime_error & e) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Could not load library %s: %s", library_name.c_str(), e.what());
      return nullptr;
    } catch (const std::bad_alloc & e) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Could not load library %s: %s", library_name.c_str(), e.what());
      return nullptr;
    }
    map->data[map_item] = lib;
  }

  auto clib = static_cast<const rcpputils::SharedLibrary *>(map->data[map_item]);
  lib = const_cast<rcpputils::SharedLibrary *>(clib);

  void * sym = nullptr;

  try {
    if (!lib->has_symbol(map->symbol_name[map_item])) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Failed to find symbol '%s' in library", map->symbol_name[map_item]);
      return nullptr;
    }
    sym = lib->get_symbol(map->symbol_name[map_item]);
  } catch (const std::exception & e) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Failed to get symbol '%s' in library: %s",
      map->symbol_name[map_item], e.what());
    return nullptr;
  }

  return sym;
}

}  // namespace rosidl_typesupport_c

#endif  // DYNAMIC_SUPPORT_DISPATCH_HPP_
