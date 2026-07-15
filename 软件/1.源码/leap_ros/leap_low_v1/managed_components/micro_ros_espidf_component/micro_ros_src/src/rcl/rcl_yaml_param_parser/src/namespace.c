// Copyright 2018 Apex.AI, Inc.
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

#include "./impl/namespace.h"

#include "rcutils/strdup.h"

rcutils_ret_t add_name_to_ns(
  namespace_tracker_t * ns_tracker,
  const char * name,
  const namespace_type_t namespace_type,
  rcutils_allocator_t allocator)
{
  char * cur_ns;
  uint32_t * cur_count;
  char * sep_str;
  size_t name_len;
  size_t ns_len;
  size_t sep_len;
  size_t tot_len;

  switch (namespace_type) {
    case NS_TYPE_NODE:
      cur_ns = ns_tracker->node_ns;
      cur_count = &(ns_tracker->num_node_ns);
      sep_str = NODE_NS_SEPERATOR;
      break;
    case NS_TYPE_PARAM:
      cur_ns = ns_tracker->parameter_ns;
      cur_count = &(ns_tracker->num_parameter_ns);
      sep_str = PARAMETER_NS_SEPERATOR;
      break;
    default:
      return RCUTILS_RET_ERROR;
  }

  /// Add a name to ns
  if (NULL == name) {
    return RCUTILS_RET_INVALID_ARGUMENT;
  }
  if (0U == *cur_count) {
    cur_ns = rcutils_strdup(name, allocator);
    if (NULL == cur_ns) {
      return RCUTILS_RET_BAD_ALLOC;
    }
  } else {
    ns_len = strlen(cur_ns);
    name_len = strlen(name);
    sep_len = strlen(sep_str);
    // Check the last sep_len characters of the current NS against the separator string.
    if (strcmp(cur_ns + ns_len - sep_len, sep_str) == 0) {
      // Current NS already ends with the separator: don't put another separator in.
      sep_len = 0;
      sep_str = "";
    }

    tot_len = ns_len + sep_len + name_len + 1U;

    char * tmp_ns_ptr = allocator.reallocate(cur_ns, tot_len, allocator.state);
    if (NULL == tmp_ns_ptr) {
      return RCUTILS_RET_BAD_ALLOC;
    }
    cur_ns = tmp_ns_ptr;
    memcpy((cur_ns + ns_len), sep_str, sep_len);
    memcpy((cur_ns + ns_len + sep_len), name, name_len);
    cur_ns[tot_len - 1U] = '\0';
  }
  *cur_count = (*cur_count + 1U);

  if (NS_TYPE_NODE == namespace_type) {
    ns_tracker->node_ns = cur_ns;
  } else {
    ns_tracker->parameter_ns = cur_ns;
  }
  return RCUTILS_RET_OK;
}

rcutils_ret_t rem_name_from_ns(
  namespace_tracker_t * ns_tracker,
  const namespace_type_t namespace_type,
  rcutils_allocator_t allocator)
{
  char * cur_ns;
  uint32_t * cur_count;
  char * sep_str;
  size_t ns_len;
  size_t tot_len;

  switch (namespace_type) {
    case NS_TYPE_NODE:
      cur_ns = ns_tracker->node_ns;
      cur_count = &(ns_tracker->num_node_ns);
      sep_str = NODE_NS_SEPERATOR;
      break;
    case NS_TYPE_PARAM:
      cur_ns = ns_tracker->parameter_ns;
      cur_count = &(ns_tracker->num_parameter_ns);
      sep_str = PARAMETER_NS_SEPERATOR;
      break;
    default:
      return RCUTILS_RET_ERROR;
  }

  /// Remove last name from ns
  if (*cur_count > 0U) {
    if (1U == *cur_count) {
      allocator.deallocate(cur_ns, allocator.state);
      cur_ns = NULL;
    } else {
      ns_len = strlen(cur_ns);
      char * last_idx = NULL;
      char * next_str = NULL;
      const char * end_ptr = (cur_ns + ns_len);

      next_str = strstr(cur_ns, sep_str);
      while (NULL != next_str) {
        if (next_str > end_ptr) {
          RCUTILS_SET_ERROR_MSG("Internal error. Crossing array boundary");
          return RCUTILS_RET_ERROR;
        }
        last_idx = next_str;
        next_str = (next_str + strlen(sep_str));
        next_str = strstr(next_str, sep_str);
      }
      if (NULL != last_idx) {
        tot_len = ((size_t)(last_idx - cur_ns) + 1U);
        char * tmp_ns_ptr = allocator.reallocate(cur_ns, tot_len, allocator.state);
        if (NULL == tmp_ns_ptr) {
          return RCUTILS_RET_BAD_ALLOC;
        }
        cur_ns = tmp_ns_ptr;
        cur_ns[tot_len - 1U] = '\0';
      }
    }
    *cur_count = (*cur_count - 1U);
  }
  if (NS_TYPE_NODE == namespace_type) {
    ns_tracker->node_ns = cur_ns;
  } else {
    ns_tracker->parameter_ns = cur_ns;
  }
  return RCUTILS_RET_OK;
}

rcutils_ret_t replace_ns(
  namespace_tracker_t * ns_tracker,
  char * const new_ns,
  const uint32_t new_ns_count,
  const namespace_type_t namespace_type,
  rcutils_allocator_t allocator)
{
  rcutils_ret_t res = RCUTILS_RET_OK;

  /// Remove the old namespace and point to the new namespace
  switch (namespace_type) {
    case NS_TYPE_NODE:
      if (NULL != ns_tracker->node_ns) {
        allocator.deallocate(ns_tracker->node_ns, allocator.state);
      }
      ns_tracker->node_ns = rcutils_strdup(new_ns, allocator);
      if (NULL == ns_tracker->node_ns) {
        return RCUTILS_RET_BAD_ALLOC;
      }
      ns_tracker->num_node_ns = new_ns_count;
      break;
    case NS_TYPE_PARAM:
      if (NULL != ns_tracker->parameter_ns) {
        allocator.deallocate(ns_tracker->parameter_ns, allocator.state);
      }
      ns_tracker->parameter_ns = rcutils_strdup(new_ns, allocator);
      if (NULL == ns_tracker->parameter_ns) {
        return RCUTILS_RET_BAD_ALLOC;
      }
      ns_tracker->num_parameter_ns = new_ns_count;
      break;
    default:
      res = RCUTILS_RET_ERROR;
      break;
  }
  return res;
}
