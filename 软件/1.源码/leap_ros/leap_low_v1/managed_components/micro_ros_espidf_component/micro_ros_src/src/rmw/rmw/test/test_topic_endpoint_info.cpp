// Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include "gmock/gmock.h"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcutils/allocator.h"

#include "rmw/error_handling.h"
#include "rmw/topic_endpoint_info.h"
#include "rmw/types.h"

char * get_mallocd_string(const char * s)
{
  size_t size = strlen(s) + 1;
  char * c = reinterpret_cast<char *>(malloc(size));
  memcpy(c, s, size);
  return c;
}

TEST(test_topic_endpoint_info, set_topic_type) {
  rmw_topic_endpoint_info_t topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * val = get_mallocd_string("test_topic_type");
  rmw_ret_t ret = rmw_topic_endpoint_info_set_topic_type(&topic_endpoint_info, val, nullptr);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null allocator";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_topic_type(&topic_endpoint_info, nullptr, &allocator);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null topic_type";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_topic_type(nullptr, val, &allocator);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) <<
    "Expected invalid argument for null topic_endpoint_info";
  rmw_reset_error();

  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(const_cast<char *>(topic_endpoint_info.topic_type), allocator.state);
  });
  ret = rmw_topic_endpoint_info_set_topic_type(&topic_endpoint_info, val, &allocator);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid arguments";
  // free the mallocd string and verify that the string was copied by value
  free(val);
  EXPECT_STREQ(
    topic_endpoint_info.topic_type,
    "test_topic_type") << "Topic Type value is not as expected";
}

TEST(test_topic_endpoint_info, set_node_name) {
  rmw_topic_endpoint_info_t topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * val = get_mallocd_string("test_node_name");
  rmw_ret_t ret = rmw_topic_endpoint_info_set_node_name(&topic_endpoint_info, val, nullptr);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null allocator";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_node_name(&topic_endpoint_info, nullptr, &allocator);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null node_name";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_node_name(nullptr, val, &allocator);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) <<
    "Expected invalid argument for null topic_endpoint_info";
  rmw_reset_error();

  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(const_cast<char *>(topic_endpoint_info.node_name), allocator.state);
  });
  ret = rmw_topic_endpoint_info_set_node_name(&topic_endpoint_info, val, &allocator);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid arguments";
  // free the mallocd string and verify that the string was copied by value
  free(val);
  EXPECT_STREQ(topic_endpoint_info.node_name, "test_node_name") <<
    "Node name value is not as expected";
}

TEST(test_topic_endpoint_info, set_node_namespace) {
  rmw_topic_endpoint_info_t topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * val = get_mallocd_string("test_node_namespace");
  rmw_ret_t ret = rmw_topic_endpoint_info_set_node_namespace(&topic_endpoint_info, val, nullptr);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null allocator";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_node_namespace(&topic_endpoint_info, nullptr, &allocator);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null node_namespace";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_node_namespace(nullptr, val, &allocator);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) <<
    "Expected invalid argument for null topic_endpoint_info";
  rmw_reset_error();

  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(const_cast<char *>(topic_endpoint_info.node_namespace), allocator.state);
  });
  ret = rmw_topic_endpoint_info_set_node_namespace(&topic_endpoint_info, val, &allocator);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid arguments";
  // free the mallocd string and verify that the string was copied by value
  free(val);
  EXPECT_STREQ(topic_endpoint_info.node_namespace, "test_node_namespace") <<
    "Node namespace value is not as expected";
}

TEST(test_topic_endpoint_info, set_gid) {
  rmw_topic_endpoint_info_t topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  uint8_t gid[RMW_GID_STORAGE_SIZE];
  for (uint8_t i = 0; i < RMW_GID_STORAGE_SIZE; i++) {
    gid[i] = i;
  }
  rmw_ret_t ret = rmw_topic_endpoint_info_set_gid(nullptr, gid, RMW_GID_STORAGE_SIZE);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) <<
    "Expected invalid argument for null topic_endpoint_info";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_gid(&topic_endpoint_info, gid, RMW_GID_STORAGE_SIZE + 1);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) <<
    "Expected invalid argument for size greater than RMW_GID_STORAGE_SIZE";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_gid(&topic_endpoint_info, gid, RMW_GID_STORAGE_SIZE);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid arguments";
  for (uint8_t i = 0; i < RMW_GID_STORAGE_SIZE; i++) {
    EXPECT_EQ(topic_endpoint_info.endpoint_gid[i], gid[i]) << "Gid value is not as expected";
  }
}

TEST(test_topic_endpoint_info, set_qos_profile) {
  rmw_topic_endpoint_info_t topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  rmw_qos_profile_t qos_profile;
  qos_profile.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
  qos_profile.depth = 0;
  qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  qos_profile.durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
  qos_profile.deadline = {1, 0};
  qos_profile.lifespan = {2, 0};
  qos_profile.liveliness = RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC;
  qos_profile.liveliness_lease_duration = {3, 0};
  qos_profile.avoid_ros_namespace_conventions = false;

  rmw_ret_t ret = rmw_topic_endpoint_info_set_qos_profile(nullptr, &qos_profile);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) <<
    "Expected invalid argument for null topic_endpoint_info";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_qos_profile(&topic_endpoint_info, nullptr);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null qos_profile";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_set_qos_profile(&topic_endpoint_info, &qos_profile);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid arguments";

  EXPECT_EQ(topic_endpoint_info.qos_profile.history, RMW_QOS_POLICY_HISTORY_KEEP_LAST) <<
    "Unequal history";
  EXPECT_EQ(topic_endpoint_info.qos_profile.depth, 0u) << "Unequal depth";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.reliability,
    RMW_QOS_POLICY_RELIABILITY_RELIABLE) << "Unequal reliability";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.durability,
    RMW_QOS_POLICY_DURABILITY_VOLATILE) << "Unequal durability";
  EXPECT_EQ(topic_endpoint_info.qos_profile.deadline.sec, 1u) << "Unequal deadline sec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.deadline.nsec, 0u) << "Unequal deadline nsec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.lifespan.sec, 2u) << "Unequal lifespan sec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.lifespan.nsec, 0u) << "Unequal lifespan nsec";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.liveliness,
    RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC) << "Unequal liveliness";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.liveliness_lease_duration.sec,
    3u) << "Unequal liveliness lease duration sec";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.liveliness_lease_duration.nsec,
    0u) << "Unequal liveliness lease duration nsec";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.avoid_ros_namespace_conventions,
    false) << "Unequal avoid namespace conventions";
}

TEST(test_topic_endpoint_info, zero_init) {
  rmw_topic_endpoint_info_t topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  EXPECT_FALSE(topic_endpoint_info.node_name);
  EXPECT_FALSE(topic_endpoint_info.node_namespace);
  EXPECT_FALSE(topic_endpoint_info.topic_type);
  EXPECT_EQ(topic_endpoint_info.endpoint_type, RMW_ENDPOINT_INVALID) <<
    "Endpoint type value should be invalid";
  for (uint8_t i = 0; i < RMW_GID_STORAGE_SIZE; i++) {
    EXPECT_EQ(topic_endpoint_info.endpoint_gid[i], 0) << "Gid value should be 0";
  }
  EXPECT_EQ(topic_endpoint_info.qos_profile.history, 0) << "Non-zero history";
  EXPECT_EQ(topic_endpoint_info.qos_profile.depth, 0u) << "Non-zero depth";
  EXPECT_EQ(topic_endpoint_info.qos_profile.reliability, 0) << "Non-zero reliability";
  EXPECT_EQ(topic_endpoint_info.qos_profile.durability, 0) << "Non-zero durability";
  EXPECT_EQ(topic_endpoint_info.qos_profile.deadline.sec, 0u) << "Non-zero deadline sec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.deadline.nsec, 0u) << "Non-zero deadline nsec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.lifespan.sec, 0u) << "Non-zero lifespan sec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.lifespan.nsec, 0u) << "Non-zero lifespan nsec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.liveliness, 0) << "Non-zero liveliness";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.liveliness_lease_duration.sec,
    0u) << "Non-zero liveliness lease duration sec";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.liveliness_lease_duration.nsec,
    0u) << "Non-zero liveliness lease duration nsec";
  EXPECT_EQ(
    topic_endpoint_info.qos_profile.avoid_ros_namespace_conventions,
    false) << "Non-zero avoid namespace conventions";
}

TEST(test_topic_endpoint_info, fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_topic_endpoint_info_t topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();
  rmw_qos_profile_t qos_profile;
  qos_profile.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
  qos_profile.depth = 0;
  qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  qos_profile.durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
  qos_profile.deadline = {1, 0};
  qos_profile.lifespan = {2, 0};
  qos_profile.liveliness = RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC;
  qos_profile.liveliness_lease_duration = {3, 0};
  qos_profile.avoid_ros_namespace_conventions = false;
  rmw_ret_t ret = rmw_topic_endpoint_info_set_qos_profile(&topic_endpoint_info, &qos_profile);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid arguments";
  uint8_t gid[RMW_GID_STORAGE_SIZE];
  for (uint8_t i = 0; i < RMW_GID_STORAGE_SIZE; i++) {
    gid[i] = i * 12;
  }
  ret = rmw_topic_endpoint_info_set_endpoint_type(&topic_endpoint_info, RMW_ENDPOINT_PUBLISHER);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid rmw_endpoint_type_t arguments";
  ret = rmw_topic_endpoint_info_set_endpoint_type(nullptr, RMW_ENDPOINT_PUBLISHER);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null topic endpoint";
  rmw_reset_error();
  ret = rmw_topic_endpoint_info_set_gid(&topic_endpoint_info, gid, RMW_GID_STORAGE_SIZE);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid gid arguments";
  ret = rmw_topic_endpoint_info_set_node_namespace(&topic_endpoint_info, "namespace", &allocator);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid node_namespace arguments";
  ret = rmw_topic_endpoint_info_set_node_name(&topic_endpoint_info, "name", &allocator);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid node_name arguments";
  ret = rmw_topic_endpoint_info_set_topic_type(&topic_endpoint_info, "type", &allocator);
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid topic_type arguments";
  ret = rmw_topic_endpoint_info_fini(&topic_endpoint_info, nullptr);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << "Expected invalid argument for null allocator";
  rmw_reset_error();
  ret = rmw_topic_endpoint_info_fini(nullptr, &allocator);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) <<
    "Expected invalid argument for null topic_endpoint_info";
  rmw_reset_error();

  ret = rmw_topic_endpoint_info_fini(&topic_endpoint_info, &allocator);
  // Verify that the values inside the struct are zero-ed out and finished.
  EXPECT_EQ(ret, RMW_RET_OK) << "Expected OK for valid fini arguments";
  EXPECT_FALSE(topic_endpoint_info.node_name);
  EXPECT_FALSE(topic_endpoint_info.node_namespace);
  EXPECT_FALSE(topic_endpoint_info.topic_type);
  EXPECT_EQ(
    topic_endpoint_info.endpoint_type,
    RMW_ENDPOINT_INVALID) << "Endpoint type value should be invalid";
  for (uint8_t i = 0; i < RMW_GID_STORAGE_SIZE; i++) {
    EXPECT_EQ(topic_endpoint_info.endpoint_gid[i], 0) << "Gid value should be 0";
  }
  EXPECT_EQ(topic_endpoint_info.qos_profile.history, 0) << "Non-zero history";
  EXPECT_EQ(topic_endpoint_info.qos_profile.depth, 0u) << "Non-zero depth";
  EXPECT_EQ(topic_endpoint_info.qos_profile.reliability, 0) << "Non-zero reliability";
  EXPECT_EQ(topic_endpoint_info.qos_profile.durability, 0) << "Non-zero durability";
  EXPECT_EQ(topic_endpoint_info.qos_profile.deadline.sec, 0u) << "Non-zero deadline sec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.deadline.nsec, 0u) << "Non-zero deadline nsec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.lifespan.sec, 0u) << "Non-zero lifespan sec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.lifespan.nsec, 0u) << "Non-zero lifespan nsec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.liveliness, 0) << "Non-zero liveliness";
  EXPECT_EQ(topic_endpoint_info.qos_profile.liveliness_lease_duration.sec, 0u) <<
    "Non-zero liveliness lease duration sec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.liveliness_lease_duration.nsec, 0u) <<
    "Non-zero liveliness lease duration nsec";
  EXPECT_EQ(topic_endpoint_info.qos_profile.avoid_ros_namespace_conventions, false) <<
    "Non-zero avoid namespace conventions";
}
