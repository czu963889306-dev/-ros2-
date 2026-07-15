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

#include "rcpputils/shared_library.hpp"
#include "rosidl_typesupport_c/identifier.h"
#include "rosidl_typesupport_c/message_type_support_dispatch.h"
#include "rosidl_typesupport_c/service_type_support_dispatch.h"
#include "rosidl_typesupport_c/type_support_map.h"

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

constexpr size_t map_size = 4u;
constexpr const char package_name[] = "rosidl_typesupport_c";
constexpr const char * identifiers[map_size] = {
  "test_type_support1", "test_type_support2", "test_type_support3", "test_type_support4"
};

constexpr const char * symbols[map_size] = {
  "test_message_type_support",
  "test_message_type_support2",
  "test_message_type_support3",
  "test_message_type_support4"
};

rosidl_message_type_support_t get_rosidl_message_type_support(const char * identifier)
{
  return {identifier, nullptr, nullptr};
}

rosidl_service_type_support_t get_rosidl_service_type_support(const char * identifier)
{
  return {identifier, nullptr, nullptr};
}

type_support_map_t get_typesupport_map(void ** library_array)
{
  return type_support_map_t{
    map_size,
    package_name,
    identifiers,
    symbols,
    library_array,
  };
}

BENCHMARK_F(PerformanceTest, message_typesupport_handle_function)(benchmark::State & st)
{
  rosidl_message_type_support_t type_support_c_identifier =
    get_rosidl_message_type_support(rosidl_typesupport_c__typesupport_identifier);
  rcpputils::SharedLibrary * library_array[map_size] = {nullptr, nullptr, nullptr};
  type_support_map_t support_map = get_typesupport_map(reinterpret_cast<void **>(&library_array));
  type_support_c_identifier.data = &support_map;

  reset_heap_counters();

  for (auto _ : st) {
    // Successfully load library and find symbols
    auto * result = rosidl_typesupport_c__get_message_typesupport_handle_function(
      &type_support_c_identifier,
      "test_type_support1");
    if (nullptr == result) {
      st.SkipWithError("rosidl_typesupport_c__get_message_typesupport_handle_function failed");
    }
    // Unload for the next iteration
    for (size_t i = 0; i < map_size; i++) {
      if (library_array[i] != nullptr) {
        delete library_array[i];
        library_array[i] = nullptr;
      }
    }
  }
}

BENCHMARK_F(PerformanceTest, service_typesupport_handle_function)(benchmark::State & st)
{
  rosidl_service_type_support_t type_support_c_identifier =
    get_rosidl_service_type_support(rosidl_typesupport_c__typesupport_identifier);
  rcpputils::SharedLibrary * library_array[map_size] = {nullptr, nullptr, nullptr};
  type_support_map_t support_map = get_typesupport_map(reinterpret_cast<void **>(&library_array));
  type_support_c_identifier.data = &support_map;

  reset_heap_counters();

  for (auto _ : st) {
    // Successfully load library and find symbols
    auto * result = rosidl_typesupport_c__get_service_typesupport_handle_function(
      &type_support_c_identifier,
      "test_type_support1");
    if (nullptr == result) {
      st.SkipWithError("rosidl_typesupport_c__get_service_typesupport_handle_function failed");
    }
    // Unload for the next iteration
    for (size_t i = 0; i < map_size; i++) {
      if (library_array[i] != nullptr) {
        delete library_array[i];
        library_array[i] = nullptr;
      }
    }
  }
}
