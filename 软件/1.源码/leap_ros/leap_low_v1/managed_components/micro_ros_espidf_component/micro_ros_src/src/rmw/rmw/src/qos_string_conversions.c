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

#include "rmw/error_handling.h"
#include "rmw/qos_string_conversions.h"

const char *
rmw_qos_policy_kind_to_str(rmw_qos_policy_kind_t kind)
{
  switch (kind) {
    case RMW_QOS_POLICY_DURABILITY:
      return "durability";
    case RMW_QOS_POLICY_DEADLINE:
      return "deadline";
    case RMW_QOS_POLICY_LIVELINESS:
      return "liveliness";
    case RMW_QOS_POLICY_RELIABILITY:
      return "reliability";
    case RMW_QOS_POLICY_HISTORY:
      return "history";
    case RMW_QOS_POLICY_LIFESPAN:
      return "lifespan";
    case RMW_QOS_POLICY_DEPTH:
      return "depth";
    case RMW_QOS_POLICY_LIVELINESS_LEASE_DURATION:
      return "liveliness_lease_duration";
    case RMW_QOS_POLICY_AVOID_ROS_NAMESPACE_CONVENTIONS:
      return "avoid_ros_namespace_conventions";
    case RMW_QOS_POLICY_INVALID:  // fallthrough
    default:
      return NULL;
  }
}

const char *
rmw_qos_durability_policy_to_str(enum rmw_qos_durability_policy_e value)
{
  switch (value) {
    case RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT:
      return "system_default";
    case RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL:
      return "transient_local";
    case RMW_QOS_POLICY_DURABILITY_VOLATILE:
      return "volatile";
    case RMW_QOS_POLICY_DURABILITY_UNKNOWN:  // fallthrough
    default:
      return NULL;
  }
}

const char *
rmw_qos_history_policy_to_str(enum rmw_qos_history_policy_e value)
{
  switch (value) {
    case RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT:
      return "system_default";
    case RMW_QOS_POLICY_HISTORY_KEEP_LAST:
      return "keep_last";
    case RMW_QOS_POLICY_HISTORY_KEEP_ALL:
      return "keep_all";
    case RMW_QOS_POLICY_HISTORY_UNKNOWN:  // fallthrough
    default:
      return NULL;
  }
}

const char *
rmw_qos_liveliness_policy_to_str(enum rmw_qos_liveliness_policy_e value)
{
  switch (value) {
    case RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT:
      return "system_default";
    case RMW_QOS_POLICY_LIVELINESS_AUTOMATIC:
      return "automatic";
    case RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC:
      return "manual_by_topic";
    case RMW_QOS_POLICY_LIVELINESS_UNKNOWN:  // fallthrough
    default:
      return NULL;
  }
}

const char *
rmw_qos_reliability_policy_to_str(enum rmw_qos_reliability_policy_e value)
{
  switch (value) {
    case RMW_QOS_POLICY_RELIABILITY_SYSTEM_DEFAULT:
      return "system_default";
    case RMW_QOS_POLICY_RELIABILITY_RELIABLE:
      return "reliable";
    case RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT:
      return "best_effort";
    case RMW_QOS_POLICY_RELIABILITY_UNKNOWN:  // fallthrough
    default:
      return NULL;
  }
}

#define RMW_QOS_STREQ_WITH_LITERAL(string_literal, string) \
  (0 == strncmp(string_literal, str, sizeof(string_literal)))

rmw_qos_policy_kind_t
rmw_qos_policy_kind_from_str(const char * str)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(str, RMW_QOS_POLICY_INVALID);
  if (RMW_QOS_STREQ_WITH_LITERAL("durability", str)) {
    return RMW_QOS_POLICY_DURABILITY;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("deadline", str)) {
    return RMW_QOS_POLICY_DEADLINE;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("liveliness", str)) {
    return RMW_QOS_POLICY_LIVELINESS;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("reliability", str)) {
    return RMW_QOS_POLICY_RELIABILITY;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("history", str)) {
    return RMW_QOS_POLICY_HISTORY;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("lifespan", str)) {
    return RMW_QOS_POLICY_LIFESPAN;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("depth", str)) {
    return RMW_QOS_POLICY_DEPTH;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("liveliness_lease_duration", str)) {
    return RMW_QOS_POLICY_LIVELINESS_LEASE_DURATION;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("avoid_ros_namespace_conventions", str)) {
    return RMW_QOS_POLICY_AVOID_ROS_NAMESPACE_CONVENTIONS;
  }
  return RMW_QOS_POLICY_INVALID;
}

enum rmw_qos_durability_policy_e
rmw_qos_durability_policy_from_str(const char * str)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(str, RMW_QOS_POLICY_DURABILITY_UNKNOWN);
  if (RMW_QOS_STREQ_WITH_LITERAL("system_default", str)) {
    return RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("transient_local", str)) {
    return RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("volatile", str)) {
    return RMW_QOS_POLICY_DURABILITY_VOLATILE;
  }
  return RMW_QOS_POLICY_DURABILITY_UNKNOWN;
}

enum rmw_qos_history_policy_e
rmw_qos_history_policy_from_str(const char * str)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(str, RMW_QOS_POLICY_HISTORY_UNKNOWN);
  if (RMW_QOS_STREQ_WITH_LITERAL("system_default", str)) {
    return RMW_QOS_POLICY_HISTORY_SYSTEM_DEFAULT;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("keep_last", str)) {
    return RMW_QOS_POLICY_HISTORY_KEEP_LAST;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("keep_all", str)) {
    return RMW_QOS_POLICY_HISTORY_KEEP_ALL;
  }
  return RMW_QOS_POLICY_HISTORY_UNKNOWN;
}

enum rmw_qos_liveliness_policy_e
rmw_qos_liveliness_policy_from_str(const char * str)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(str, RMW_QOS_POLICY_LIVELINESS_UNKNOWN);
  if (RMW_QOS_STREQ_WITH_LITERAL("system_default", str)) {
    return RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("automatic", str)) {
    return RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("manual_by_topic", str)) {
    return RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC;
  }
  return RMW_QOS_POLICY_LIVELINESS_UNKNOWN;
}

enum rmw_qos_reliability_policy_e
rmw_qos_reliability_policy_from_str(const char * str)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(str, RMW_QOS_POLICY_RELIABILITY_UNKNOWN);
  if (RMW_QOS_STREQ_WITH_LITERAL("system_default", str)) {
    return RMW_QOS_POLICY_RELIABILITY_SYSTEM_DEFAULT;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("reliable", str)) {
    return RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  }
  if (RMW_QOS_STREQ_WITH_LITERAL("best_effort", str)) {
    return RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  }
  return RMW_QOS_POLICY_RELIABILITY_UNKNOWN;
}
