// Copyright 2016-2017 Open Source Robotics Foundation, Inc.
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

#include "functions.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "ament_index_cpp/get_resources.hpp"

#include "rcutils/allocator.h"
#include "rcutils/format_string.h"
#include "rcutils/types/string_array.h"

#include "rcpputils/env.hpp"
#include "rcpputils/shared_library.hpp"

#include "rmw/error_handling.h"
#include "rmw/event.h"
#include "rmw/features.h"
#include "rmw/get_network_flow_endpoints.h"
#include "rmw/get_node_info_and_types.h"
#include "rmw/get_service_names_and_types.h"
#include "rmw/get_topic_endpoint_info.h"
#include "rmw/get_topic_names_and_types.h"
#include "rmw/names_and_types.h"
#include "rmw/rmw.h"

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static std::shared_ptr<rcpputils::SharedLibrary> g_rmw_lib = nullptr;

static std::shared_ptr<rcpputils::SharedLibrary>
attempt_to_load_one_rmw(const std::string & library)
{
  std::string library_name;
  std::shared_ptr<rcpputils::SharedLibrary> ret = nullptr;

  try {
    library_name = rcpputils::get_platform_library_name(library);
  } catch (const std::exception & e) {
    RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to compute shared library name due to %s", e.what());
    return ret;
  }

  try {
    ret = std::make_shared<rcpputils::SharedLibrary>(library_name);
  } catch (const std::exception & e) {
    RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to load shared library '%s' due to %s",
      library_name.c_str(), e.what());
    ret = nullptr;
  }

  return ret;
}

std::shared_ptr<rcpputils::SharedLibrary>
load_library()
{
  // The logic to pick the RMW library to load goes as follows:
  //
  // 1. If the user specified the library to use via the RMW_IMPLEMENTATION
  //    environment variable, try to load only that library.
  // 2. Otherwise, try to load the default RMW implementation.
  // 3. If that fails, try loading all other implementations available in turn
  //    until one succeeds or we run out of options.

  std::string env_var;
  try {
    env_var = rcpputils::get_env_var("RMW_IMPLEMENTATION");
  } catch (const std::exception & e) {
    RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to fetch RMW_IMPLEMENTATION "
      "from environment due to %s", e.what());
    return nullptr;
  }

  // User specified an RMW, attempt to load that one and only that one
  if (!env_var.empty()) {
    return attempt_to_load_one_rmw(env_var);
  }

  // User didn't specify, so next try to load the default RMW
  std::shared_ptr<rcpputils::SharedLibrary> ret;

  ret = attempt_to_load_one_rmw(STRINGIFY(DEFAULT_RMW_IMPLEMENTATION));
  if (ret != nullptr) {
    return ret;
  }

  // OK, we failed to load the default RMW.  Fetch all of the ones we can
  // find and attempt to load them one-by-one.
  rmw_reset_error();
  const std::map<std::string, std::string> packages_with_prefixes = ament_index_cpp::get_resources(
    "rmw_typesupport");
  for (const auto & package_prefix_pair : packages_with_prefixes) {
    if (package_prefix_pair.first != "rmw_implementation") {
      ret = attempt_to_load_one_rmw(package_prefix_pair.first);
      if (ret != nullptr) {
        return ret;
      }
      rmw_reset_error();
    }
  }

  // If we made it here, we couldn't find an rmw to load.

  RMW_SET_ERROR_MSG("failed to load any RMW implementations");

  return nullptr;
}

std::shared_ptr<rcpputils::SharedLibrary>
get_library()
{
  if (!g_rmw_lib) {
    g_rmw_lib = load_library();
  }
  return g_rmw_lib;
}

void *
lookup_symbol(std::shared_ptr<rcpputils::SharedLibrary> lib, const std::string & symbol_name)
{
  if (!lib) {
    if (!rmw_error_is_set()) {
      RMW_SET_ERROR_MSG("no shared library to lookup");
    }  // else assume library loading failed
    return nullptr;
  }

  if (!lib->has_symbol(symbol_name)) {
    try {
      std::string library_path = lib->get_library_path();
      RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "failed to resolve symbol '%s' in shared library '%s'",
        symbol_name.c_str(), library_path.c_str());
    } catch (const std::exception & e) {
      RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "failed to resolve symbol '%s' in shared library due to %s",
        symbol_name.c_str(), e.what());
    }
    return nullptr;
  }
  return lib->get_symbol(symbol_name);
}

void *
get_symbol(const char * symbol_name)
{
  try {
    return lookup_symbol(get_library(), symbol_name);
  } catch (const std::exception & e) {
    RMW_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to get symbol '%s' due to %s",
      symbol_name, e.what());
    return nullptr;
  }
}

#ifdef __cplusplus
extern "C"
{
#endif

#define EXPAND(x) x

#define ARG_TYPES(...) __VA_ARGS__

#define ARG_VALUES_0(...)
#define ARG_VALUES_1(t1) v1
#define ARG_VALUES_2(t2, ...) v2, EXPAND(ARG_VALUES_1(__VA_ARGS__))
#define ARG_VALUES_3(t3, ...) v3, EXPAND(ARG_VALUES_2(__VA_ARGS__))
#define ARG_VALUES_4(t4, ...) v4, EXPAND(ARG_VALUES_3(__VA_ARGS__))
#define ARG_VALUES_5(t5, ...) v5, EXPAND(ARG_VALUES_4(__VA_ARGS__))
#define ARG_VALUES_6(t6, ...) v6, EXPAND(ARG_VALUES_5(__VA_ARGS__))
#define ARG_VALUES_7(t7, ...) v7, EXPAND(ARG_VALUES_6(__VA_ARGS__))

#define ARGS_0(...) __VA_ARGS__
#define ARGS_1(t1) t1 v1
#define ARGS_2(t2, ...) t2 v2, EXPAND(ARGS_1(__VA_ARGS__))
#define ARGS_3(t3, ...) t3 v3, EXPAND(ARGS_2(__VA_ARGS__))
#define ARGS_4(t4, ...) t4 v4, EXPAND(ARGS_3(__VA_ARGS__))
#define ARGS_5(t5, ...) t5 v5, EXPAND(ARGS_4(__VA_ARGS__))
#define ARGS_6(t6, ...) t6 v6, EXPAND(ARGS_5(__VA_ARGS__))
#define ARGS_7(t7, ...) t7 v7, EXPAND(ARGS_6(__VA_ARGS__))

#define CALL_SYMBOL(symbol_name, ReturnType, error_value, ArgTypes, arg_values) \
  if (!symbol_ ## symbol_name) { \
    /* only necessary for functions called before rmw_init */ \
    symbol_ ## symbol_name = get_symbol(#symbol_name); \
  } \
  if (!symbol_ ## symbol_name) { \
    /* error message set by get_symbol() */ \
    return error_value; \
  } \
  typedef ReturnType (* FunctionSignature)(ArgTypes); \
  FunctionSignature func = reinterpret_cast<FunctionSignature>(symbol_ ## symbol_name); \
  return func(arg_values);

// cppcheck-suppress preprocessorErrorDirective
#define RMW_INTERFACE_FN(name, ReturnType, error_value, _NR, ...) \
  void * symbol_ ## name = nullptr; \
  ReturnType name(EXPAND(ARGS_ ## _NR(__VA_ARGS__))) \
  { \
    CALL_SYMBOL( \
      name, ReturnType, error_value, ARG_TYPES(__VA_ARGS__), \
      EXPAND(ARG_VALUES_ ## _NR(__VA_ARGS__))); \
  }

RMW_INTERFACE_FN(
  rmw_get_implementation_identifier,
  const char *, nullptr,
  0, ARG_TYPES(void))

RMW_INTERFACE_FN(
  rmw_init_options_init,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(rmw_init_options_t *, rcutils_allocator_t))

RMW_INTERFACE_FN(
  rmw_init_options_copy,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_init_options_t *, rmw_init_options_t *))

RMW_INTERFACE_FN(
  rmw_init_options_fini,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_init_options_t *))

RMW_INTERFACE_FN(
  rmw_shutdown,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_context_t *))

RMW_INTERFACE_FN(
  rmw_context_fini,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_context_t *))

RMW_INTERFACE_FN(
  rmw_get_serialization_format,
  const char *, nullptr,
  0, ARG_TYPES(void))

RMW_INTERFACE_FN(
  rmw_create_node,
  rmw_node_t *, nullptr,
  3, ARG_TYPES(
    rmw_context_t *, const char *, const char *))

RMW_INTERFACE_FN(
  rmw_destroy_node,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_node_t *))

RMW_INTERFACE_FN(
  rmw_node_get_graph_guard_condition,
  const rmw_guard_condition_t *, nullptr,
  1, ARG_TYPES(const rmw_node_t *))

RMW_INTERFACE_FN(
  rmw_init_publisher_allocation,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    const rosidl_message_type_support_t *,
    const rosidl_runtime_c__Sequence__bound *,
    rmw_publisher_allocation_t *))

RMW_INTERFACE_FN(
  rmw_fini_publisher_allocation,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_publisher_allocation_t *))

RMW_INTERFACE_FN(
  rmw_create_publisher,
  rmw_publisher_t *, nullptr,
  5, ARG_TYPES(
    const rmw_node_t *, const rosidl_message_type_support_t *, const char *,
    const rmw_qos_profile_t *, const rmw_publisher_options_t *))

RMW_INTERFACE_FN(
  rmw_destroy_publisher,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(rmw_node_t *, rmw_publisher_t *))

RMW_INTERFACE_FN(
  rmw_borrow_loaned_message,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    const rmw_publisher_t *,
    const rosidl_message_type_support_t *,
    void **))

RMW_INTERFACE_FN(
  rmw_return_loaned_message_from_publisher,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_publisher_t *, void *))

RMW_INTERFACE_FN(
  rmw_publish,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_publisher_t *, const void *, rmw_publisher_allocation_t *))

RMW_INTERFACE_FN(
  rmw_publish_loaned_message,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_publisher_t *, void *, rmw_publisher_allocation_t *))

RMW_INTERFACE_FN(
  rmw_publisher_count_matched_subscriptions,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_publisher_t *, size_t *))

RMW_INTERFACE_FN(
  rmw_publisher_get_actual_qos,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_publisher_t *, rmw_qos_profile_t *))

RMW_INTERFACE_FN(
  rmw_publisher_event_init,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(rmw_event_t *, const rmw_publisher_t *, rmw_event_type_t))

RMW_INTERFACE_FN(
  rmw_publish_serialized_message,
  rmw_ret_t, RMW_RET_ERROR,
  3,
  ARG_TYPES(
    const rmw_publisher_t *, const rmw_serialized_message_t *,
    rmw_publisher_allocation_t *))

RMW_INTERFACE_FN(
  rmw_get_serialized_message_size,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    const rosidl_message_type_support_t *,
    const rosidl_runtime_c__Sequence__bound *,
    size_t *))

RMW_INTERFACE_FN(
  rmw_publisher_assert_liveliness,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(const rmw_publisher_t *))

RMW_INTERFACE_FN(
  rmw_publisher_wait_for_all_acked,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_publisher_t *, rmw_time_t))

RMW_INTERFACE_FN(
  rmw_serialize,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const void *, const rosidl_message_type_support_t *, rmw_serialized_message_t *))

RMW_INTERFACE_FN(
  rmw_deserialize,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_serialized_message_t *, const rosidl_message_type_support_t *, void *))

RMW_INTERFACE_FN(
  rmw_init_subscription_allocation,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    const rosidl_message_type_support_t *,
    const rosidl_runtime_c__Sequence__bound *,
    rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_fini_subscription_allocation,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_create_subscription,
  rmw_subscription_t *, nullptr,
  5, ARG_TYPES(
    const rmw_node_t *, const rosidl_message_type_support_t *, const char *,
    const rmw_qos_profile_t *, const rmw_subscription_options_t *))

RMW_INTERFACE_FN(
  rmw_destroy_subscription,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(rmw_node_t *, rmw_subscription_t *))

RMW_INTERFACE_FN(
  rmw_subscription_count_matched_publishers,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_subscription_t *, size_t *))

RMW_INTERFACE_FN(
  rmw_subscription_get_actual_qos,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_subscription_t *, rmw_qos_profile_t *))

RMW_INTERFACE_FN(
  rmw_subscription_event_init,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(rmw_event_t *, const rmw_subscription_t *, rmw_event_type_t))

RMW_INTERFACE_FN(
  rmw_subscription_set_content_filter,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(
    rmw_subscription_t *, const rmw_subscription_content_filter_options_t *))

RMW_INTERFACE_FN(
  rmw_subscription_get_content_filter,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    const rmw_subscription_t *, rcutils_allocator_t *,
    rmw_subscription_content_filter_options_t *))

RMW_INTERFACE_FN(
  rmw_take,
  rmw_ret_t, RMW_RET_ERROR,
  4, ARG_TYPES(const rmw_subscription_t *, void *, bool *, rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_take_sequence,
  rmw_ret_t, RMW_RET_ERROR,
  6, ARG_TYPES(
    const rmw_subscription_t *, size_t, rmw_message_sequence_t *,
    rmw_message_info_sequence_t *, size_t *, rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_take_with_info,
  rmw_ret_t, RMW_RET_ERROR,
  5,
  ARG_TYPES(
    const rmw_subscription_t *, void *, bool *, rmw_message_info_t *,
    rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_take_serialized_message,
  rmw_ret_t, RMW_RET_ERROR,
  4,
  ARG_TYPES(
    const rmw_subscription_t *, rmw_serialized_message_t *, bool *,
    rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_take_serialized_message_with_info,
  rmw_ret_t, RMW_RET_ERROR,
  5, ARG_TYPES(
    const rmw_subscription_t *, rmw_serialized_message_t *, bool *, rmw_message_info_t *,
    rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_take_loaned_message,
  rmw_ret_t, RMW_RET_ERROR,
  4, ARG_TYPES(
    const rmw_subscription_t *, void **, bool *, rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_take_loaned_message_with_info,
  rmw_ret_t, RMW_RET_ERROR,
  5, ARG_TYPES(
    const rmw_subscription_t *, void **, bool *, rmw_message_info_t *,
    rmw_subscription_allocation_t *))

RMW_INTERFACE_FN(
  rmw_return_loaned_message_from_subscription,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_subscription_t *, void *))

RMW_INTERFACE_FN(
  rmw_create_client,
  rmw_client_t *, nullptr,
  4, ARG_TYPES(
    const rmw_node_t *, const rosidl_service_type_support_t *, const char *,
    const rmw_qos_profile_t *))

RMW_INTERFACE_FN(
  rmw_destroy_client,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(rmw_node_t *, rmw_client_t *))

RMW_INTERFACE_FN(
  rmw_send_request,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_client_t *, const void *, int64_t *))

RMW_INTERFACE_FN(
  rmw_take_response,
  rmw_ret_t, RMW_RET_ERROR,
  4, ARG_TYPES(const rmw_client_t *, rmw_service_info_t *, void *, bool *))

RMW_INTERFACE_FN(
  rmw_client_request_publisher_get_actual_qos,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_client_t *, rmw_qos_profile_t *))

RMW_INTERFACE_FN(
  rmw_client_response_subscription_get_actual_qos,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_client_t *, rmw_qos_profile_t *))

RMW_INTERFACE_FN(
  rmw_create_service,
  rmw_service_t *, nullptr,
  4, ARG_TYPES(
    const rmw_node_t *, const rosidl_service_type_support_t *, const char *,
    const rmw_qos_profile_t *))

RMW_INTERFACE_FN(
  rmw_destroy_service,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(rmw_node_t *, rmw_service_t *))

RMW_INTERFACE_FN(
  rmw_take_request,
  rmw_ret_t, RMW_RET_ERROR,
  4, ARG_TYPES(const rmw_service_t *, rmw_service_info_t *, void *, bool *))

RMW_INTERFACE_FN(
  rmw_send_response,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_service_t *, rmw_request_id_t *, void *))

RMW_INTERFACE_FN(
  rmw_service_response_publisher_get_actual_qos,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_service_t *, rmw_qos_profile_t *))

RMW_INTERFACE_FN(
  rmw_service_request_subscription_get_actual_qos,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_service_t *, rmw_qos_profile_t *))

RMW_INTERFACE_FN(
  rmw_take_event,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_event_t *, void *, bool *))

RMW_INTERFACE_FN(
  rmw_create_guard_condition,
  rmw_guard_condition_t *, nullptr,
  1, ARG_TYPES(rmw_context_t *))

RMW_INTERFACE_FN(
  rmw_destroy_guard_condition,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_guard_condition_t *))

RMW_INTERFACE_FN(
  rmw_trigger_guard_condition,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(const rmw_guard_condition_t *))

RMW_INTERFACE_FN(
  rmw_create_wait_set,
  rmw_wait_set_t *, nullptr,
  2, ARG_TYPES(rmw_context_t *, size_t))

RMW_INTERFACE_FN(
  rmw_destroy_wait_set,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_wait_set_t *))

RMW_INTERFACE_FN(
  rmw_wait,
  rmw_ret_t, RMW_RET_ERROR,
  7, ARG_TYPES(
    rmw_subscriptions_t *, rmw_guard_conditions_t *, rmw_services_t *, rmw_clients_t *,
    rmw_events_t *, rmw_wait_set_t *, const rmw_time_t *))

RMW_INTERFACE_FN(
  rmw_get_publisher_names_and_types_by_node,
  rmw_ret_t, RMW_RET_ERROR,
  6, ARG_TYPES(
    const rmw_node_t *, rcutils_allocator_t *, const char *, const char *, bool,
    rmw_names_and_types_t *))

RMW_INTERFACE_FN(
  rmw_get_subscriber_names_and_types_by_node,
  rmw_ret_t, RMW_RET_ERROR,
  6, ARG_TYPES(
    const rmw_node_t *, rcutils_allocator_t *, const char *, const char *, bool,
    rmw_names_and_types_t *))

RMW_INTERFACE_FN(
  rmw_get_service_names_and_types_by_node,
  rmw_ret_t, RMW_RET_ERROR,
  5, ARG_TYPES(
    const rmw_node_t *, rcutils_allocator_t *, const char *, const char *,
    rmw_names_and_types_t *))

RMW_INTERFACE_FN(
  rmw_get_client_names_and_types_by_node,
  rmw_ret_t, RMW_RET_ERROR,
  5, ARG_TYPES(
    const rmw_node_t *, rcutils_allocator_t *, const char *, const char *,
    rmw_names_and_types_t *))

RMW_INTERFACE_FN(
  rmw_get_topic_names_and_types,
  rmw_ret_t, RMW_RET_ERROR,
  4, ARG_TYPES(
    const rmw_node_t *, rcutils_allocator_t *, bool,
    rmw_names_and_types_t *))

RMW_INTERFACE_FN(
  rmw_get_service_names_and_types,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    const rmw_node_t *, rcutils_allocator_t *,
    rmw_names_and_types_t *))

RMW_INTERFACE_FN(
  rmw_get_node_names,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_node_t *, rcutils_string_array_t *, rcutils_string_array_t *))

RMW_INTERFACE_FN(
  rmw_get_node_names_with_enclaves,
  rmw_ret_t, RMW_RET_ERROR,
  4, ARG_TYPES(
    const rmw_node_t *, rcutils_string_array_t *,
    rcutils_string_array_t *, rcutils_string_array_t *))

RMW_INTERFACE_FN(
  rmw_count_publishers,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_node_t *, const char *, size_t *))

RMW_INTERFACE_FN(
  rmw_count_subscribers,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_node_t *, const char *, size_t *))

RMW_INTERFACE_FN(
  rmw_get_gid_for_publisher,
  rmw_ret_t, RMW_RET_ERROR,
  2, ARG_TYPES(const rmw_publisher_t *, rmw_gid_t *))

RMW_INTERFACE_FN(
  rmw_compare_gids_equal,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_gid_t *, const rmw_gid_t *, bool *))

RMW_INTERFACE_FN(
  rmw_service_server_is_available,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(const rmw_node_t *, const rmw_client_t *, bool *))

RMW_INTERFACE_FN(
  rmw_set_log_severity,
  rmw_ret_t, RMW_RET_ERROR,
  1, ARG_TYPES(rmw_log_severity_t))

RMW_INTERFACE_FN(
  rmw_get_publishers_info_by_topic,
  rmw_ret_t, RMW_RET_ERROR,
  5, ARG_TYPES(
    const rmw_node_t *,
    rcutils_allocator_t *,
    const char *,
    bool,
    rmw_topic_endpoint_info_array_t *))

RMW_INTERFACE_FN(
  rmw_get_subscriptions_info_by_topic,
  rmw_ret_t, RMW_RET_ERROR,
  5, ARG_TYPES(
    const rmw_node_t *,
    rcutils_allocator_t *,
    const char *,
    bool,
    rmw_topic_endpoint_info_array_t *))

RMW_INTERFACE_FN(
  rmw_qos_profile_check_compatible,
  rmw_ret_t, RMW_RET_ERROR,
  5, ARG_TYPES(
    const rmw_qos_profile_t,
    const rmw_qos_profile_t,
    rmw_qos_compatibility_type_t *,
    char *,
    size_t))

RMW_INTERFACE_FN(
  rmw_publisher_get_network_flow_endpoints,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    const rmw_publisher_t *,
    rcutils_allocator_t *,
    rmw_network_flow_endpoint_array_t *))

RMW_INTERFACE_FN(
  rmw_subscription_get_network_flow_endpoints,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    const rmw_subscription_t *,
    rcutils_allocator_t *,
    rmw_network_flow_endpoint_array_t *))

RMW_INTERFACE_FN(
  rmw_subscription_set_on_new_message_callback,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    rmw_subscription_t *, rmw_event_callback_t, const void *))

RMW_INTERFACE_FN(
  rmw_service_set_on_new_request_callback,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    rmw_service_t *, rmw_event_callback_t, const void *))

RMW_INTERFACE_FN(
  rmw_client_set_on_new_response_callback,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    rmw_client_t *, rmw_event_callback_t, const void *))

RMW_INTERFACE_FN(
  rmw_event_set_callback,
  rmw_ret_t, RMW_RET_ERROR,
  3, ARG_TYPES(
    rmw_event_t *, rmw_event_callback_t, const void *))

RMW_INTERFACE_FN(
  rmw_feature_supported,
  bool, false,
  1, ARG_TYPES(
    rmw_feature_t))

#define GET_SYMBOL(x) symbol_ ## x = get_symbol(#x);

void prefetch_symbols(void)
{
  // get all symbols to avoid race conditions later since the passed
  // symbol name is expected to be a std::string which requires allocation
  GET_SYMBOL(rmw_get_implementation_identifier)
  GET_SYMBOL(rmw_init_options_init)
  GET_SYMBOL(rmw_init_options_copy)
  GET_SYMBOL(rmw_init_options_fini)
  GET_SYMBOL(rmw_shutdown)
  GET_SYMBOL(rmw_context_fini)
  GET_SYMBOL(rmw_get_serialization_format)
  GET_SYMBOL(rmw_create_node)
  GET_SYMBOL(rmw_destroy_node)
  GET_SYMBOL(rmw_node_get_graph_guard_condition)
  GET_SYMBOL(rmw_init_publisher_allocation)
  GET_SYMBOL(rmw_fini_publisher_allocation)
  GET_SYMBOL(rmw_create_publisher)
  GET_SYMBOL(rmw_destroy_publisher)
  GET_SYMBOL(rmw_borrow_loaned_message)
  GET_SYMBOL(rmw_return_loaned_message_from_publisher)
  GET_SYMBOL(rmw_publish)
  GET_SYMBOL(rmw_publish_loaned_message)
  GET_SYMBOL(rmw_publisher_count_matched_subscriptions)
  GET_SYMBOL(rmw_publisher_get_actual_qos)
  GET_SYMBOL(rmw_publisher_event_init)
  GET_SYMBOL(rmw_publish_serialized_message)
  GET_SYMBOL(rmw_publisher_assert_liveliness)
  GET_SYMBOL(rmw_publisher_wait_for_all_acked)
  GET_SYMBOL(rmw_get_serialized_message_size)
  GET_SYMBOL(rmw_serialize)
  GET_SYMBOL(rmw_deserialize)
  GET_SYMBOL(rmw_init_subscription_allocation)
  GET_SYMBOL(rmw_fini_subscription_allocation)
  GET_SYMBOL(rmw_create_subscription)
  GET_SYMBOL(rmw_destroy_subscription)
  GET_SYMBOL(rmw_subscription_count_matched_publishers)
  GET_SYMBOL(rmw_subscription_get_actual_qos)
  GET_SYMBOL(rmw_subscription_event_init)
  GET_SYMBOL(rmw_subscription_set_content_filter)
  GET_SYMBOL(rmw_subscription_get_content_filter)
  GET_SYMBOL(rmw_take)
  GET_SYMBOL(rmw_take_with_info)
  GET_SYMBOL(rmw_take_serialized_message)
  GET_SYMBOL(rmw_take_serialized_message_with_info)
  GET_SYMBOL(rmw_take_loaned_message)
  GET_SYMBOL(rmw_take_loaned_message_with_info)
  GET_SYMBOL(rmw_return_loaned_message_from_subscription)
  GET_SYMBOL(rmw_create_client)
  GET_SYMBOL(rmw_destroy_client)
  GET_SYMBOL(rmw_send_request)
  GET_SYMBOL(rmw_take_response)
  GET_SYMBOL(rmw_create_service)
  GET_SYMBOL(rmw_destroy_service)
  GET_SYMBOL(rmw_take_request)
  GET_SYMBOL(rmw_send_response)
  GET_SYMBOL(rmw_take_event)
  GET_SYMBOL(rmw_create_guard_condition)
  GET_SYMBOL(rmw_destroy_guard_condition)
  GET_SYMBOL(rmw_trigger_guard_condition)
  GET_SYMBOL(rmw_create_wait_set)
  GET_SYMBOL(rmw_destroy_wait_set)
  GET_SYMBOL(rmw_wait)
  GET_SYMBOL(rmw_get_publisher_names_and_types_by_node)
  GET_SYMBOL(rmw_get_subscriber_names_and_types_by_node)
  GET_SYMBOL(rmw_get_service_names_and_types_by_node)
  GET_SYMBOL(rmw_get_client_names_and_types_by_node)
  GET_SYMBOL(rmw_get_topic_names_and_types)
  GET_SYMBOL(rmw_get_service_names_and_types)
  GET_SYMBOL(rmw_get_node_names)
  GET_SYMBOL(rmw_get_node_names_with_enclaves)
  GET_SYMBOL(rmw_count_publishers)
  GET_SYMBOL(rmw_count_subscribers)
  GET_SYMBOL(rmw_get_gid_for_publisher)
  GET_SYMBOL(rmw_compare_gids_equal)
  GET_SYMBOL(rmw_service_response_publisher_get_actual_qos)
  GET_SYMBOL(rmw_service_request_subscription_get_actual_qos)
  GET_SYMBOL(rmw_service_server_is_available)
  GET_SYMBOL(rmw_set_log_severity)
  GET_SYMBOL(rmw_get_publishers_info_by_topic)
  GET_SYMBOL(rmw_get_subscriptions_info_by_topic)
  GET_SYMBOL(rmw_qos_profile_check_compatible)
  GET_SYMBOL(rmw_publisher_get_network_flow_endpoints)
  GET_SYMBOL(rmw_subscription_get_network_flow_endpoints)
  GET_SYMBOL(rmw_client_request_publisher_get_actual_qos)
  GET_SYMBOL(rmw_client_response_subscription_get_actual_qos)
  GET_SYMBOL(rmw_subscription_set_on_new_message_callback)
  GET_SYMBOL(rmw_service_set_on_new_request_callback)
  GET_SYMBOL(rmw_client_set_on_new_response_callback)
  GET_SYMBOL(rmw_event_set_callback)
  GET_SYMBOL(rmw_feature_supported)
}

void * symbol_rmw_init = nullptr;

rmw_ret_t
rmw_init(const rmw_init_options_t * options, rmw_context_t * context)
{
  prefetch_symbols();
  if (!symbol_rmw_init) {
    symbol_rmw_init = get_symbol("rmw_init");
  }
  if (!symbol_rmw_init) {
    return RMW_RET_ERROR;
  }

  typedef rmw_ret_t (* FunctionSignature)(const rmw_init_options_t *, rmw_context_t *);
  FunctionSignature func = reinterpret_cast<FunctionSignature>(symbol_rmw_init);
  return func(options, context);
}

#ifdef __cplusplus
}
#endif

void
unload_library()
{
  symbol_rmw_get_implementation_identifier = nullptr;
  symbol_rmw_init_options_init = nullptr;
  symbol_rmw_init_options_copy = nullptr;
  symbol_rmw_init_options_fini = nullptr;
  symbol_rmw_shutdown = nullptr;
  symbol_rmw_context_fini = nullptr;
  symbol_rmw_get_serialization_format = nullptr;
  symbol_rmw_create_node = nullptr;
  symbol_rmw_destroy_node = nullptr;
  symbol_rmw_node_get_graph_guard_condition = nullptr;
  symbol_rmw_init_publisher_allocation = nullptr;
  symbol_rmw_fini_publisher_allocation = nullptr;
  symbol_rmw_create_publisher = nullptr;
  symbol_rmw_destroy_publisher = nullptr;
  symbol_rmw_borrow_loaned_message = nullptr;
  symbol_rmw_return_loaned_message_from_publisher = nullptr;
  symbol_rmw_publish = nullptr;
  symbol_rmw_publish_loaned_message = nullptr;
  symbol_rmw_publisher_count_matched_subscriptions = nullptr;
  symbol_rmw_publisher_get_actual_qos = nullptr;
  symbol_rmw_publisher_event_init = nullptr;
  symbol_rmw_publish_serialized_message = nullptr;
  symbol_rmw_get_serialized_message_size = nullptr;
  symbol_rmw_publisher_assert_liveliness = nullptr;
  symbol_rmw_publisher_wait_for_all_acked = nullptr;
  symbol_rmw_serialize = nullptr;
  symbol_rmw_deserialize = nullptr;
  symbol_rmw_init_subscription_allocation = nullptr;
  symbol_rmw_fini_subscription_allocation = nullptr;
  symbol_rmw_create_subscription = nullptr;
  symbol_rmw_destroy_subscription = nullptr;
  symbol_rmw_subscription_count_matched_publishers = nullptr;
  symbol_rmw_subscription_get_actual_qos = nullptr;
  symbol_rmw_subscription_event_init = nullptr;
  symbol_rmw_subscription_set_content_filter = nullptr;
  symbol_rmw_subscription_get_content_filter = nullptr;
  symbol_rmw_take = nullptr;
  symbol_rmw_take_sequence = nullptr;
  symbol_rmw_take_with_info = nullptr;
  symbol_rmw_take_serialized_message = nullptr;
  symbol_rmw_take_serialized_message_with_info = nullptr;
  symbol_rmw_take_loaned_message = nullptr;
  symbol_rmw_take_loaned_message_with_info = nullptr;
  symbol_rmw_return_loaned_message_from_subscription = nullptr;
  symbol_rmw_create_client = nullptr;
  symbol_rmw_destroy_client = nullptr;
  symbol_rmw_client_request_publisher_get_actual_qos = nullptr;
  symbol_rmw_client_response_subscription_get_actual_qos = nullptr;
  symbol_rmw_send_request = nullptr;
  symbol_rmw_take_response = nullptr;
  symbol_rmw_create_service = nullptr;
  symbol_rmw_destroy_service = nullptr;
  symbol_rmw_service_response_publisher_get_actual_qos = nullptr;
  symbol_rmw_service_request_subscription_get_actual_qos = nullptr;
  symbol_rmw_take_request = nullptr;
  symbol_rmw_send_response = nullptr;
  symbol_rmw_take_event = nullptr;
  symbol_rmw_create_guard_condition = nullptr;
  symbol_rmw_destroy_guard_condition = nullptr;
  symbol_rmw_trigger_guard_condition = nullptr;
  symbol_rmw_create_wait_set = nullptr;
  symbol_rmw_destroy_wait_set = nullptr;
  symbol_rmw_wait = nullptr;
  symbol_rmw_get_publisher_names_and_types_by_node = nullptr;
  symbol_rmw_get_subscriber_names_and_types_by_node = nullptr;
  symbol_rmw_get_service_names_and_types_by_node = nullptr;
  symbol_rmw_get_client_names_and_types_by_node = nullptr;
  symbol_rmw_get_topic_names_and_types = nullptr;
  symbol_rmw_get_service_names_and_types = nullptr;
  symbol_rmw_get_node_names = nullptr;
  symbol_rmw_get_node_names_with_enclaves = nullptr;
  symbol_rmw_count_publishers = nullptr;
  symbol_rmw_count_subscribers = nullptr;
  symbol_rmw_get_gid_for_publisher = nullptr;
  symbol_rmw_compare_gids_equal = nullptr;
  symbol_rmw_service_server_is_available = nullptr;
  symbol_rmw_set_log_severity = nullptr;
  symbol_rmw_get_publishers_info_by_topic = nullptr;
  symbol_rmw_get_subscriptions_info_by_topic = nullptr;
  symbol_rmw_qos_profile_check_compatible = nullptr;
  symbol_rmw_publisher_get_network_flow_endpoints = nullptr;
  symbol_rmw_subscription_get_network_flow_endpoints = nullptr;
  symbol_rmw_init = nullptr;
  symbol_rmw_subscription_set_on_new_message_callback = nullptr;
  symbol_rmw_service_set_on_new_request_callback = nullptr;
  symbol_rmw_client_set_on_new_response_callback = nullptr;
  symbol_rmw_event_set_callback = nullptr;
  symbol_rmw_feature_supported = nullptr;
  g_rmw_lib.reset();
}
