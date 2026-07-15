// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#include <gtest/gtest.h>

#include "rcl/arguments.h"
#include "rcl/rcl.h"
#include "rcl/remap.h"
#include "rcl/error_handling.h"

#include "./allocator_testing_utils.h"
#include "./arg_macros.hpp"
#include "./arguments_impl.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestRemapFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
  }

  void TearDown()
  {
  }
};

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), global_namespace_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "__ns:=/foo/bar");

  char * output = NULL;
  ret = rcl_remap_node_namespace(
    NULL, &global_arguments, "NodeName", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("/foo/bar", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), nodename_prefix_namespace_remap) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(
    global_arguments,
    "process_name",
    "--ros-args",
    "-r", "Node1:__ns:=/foo/bar",
    "-r", "Node2:__ns:=/this_one",
    "-r", "Node3:__ns:=/bar/foo");

  {
    char * output = NULL;
    ret = rcl_remap_node_namespace(
      NULL, &global_arguments, "Node1", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/foo/bar", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
  {
    char * output = NULL;
    ret = rcl_remap_node_namespace(
      NULL, &global_arguments, "Node2", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/this_one", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
  {
    char * output = NULL;
    ret = rcl_remap_node_namespace(
      NULL, &global_arguments, "Node3", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/bar/foo", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), no_namespace_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name");

  char * output = NULL;
  ret = rcl_remap_node_namespace(
    NULL, &global_arguments, "NodeName", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), local_namespace_replacement_before_global) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "__ns:=/global_args");
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name", "--ros-args", "-r", "__ns:=/local_args");

  char * output = NULL;
  ret = rcl_remap_node_namespace(
    &local_arguments, &global_arguments, "NodeName", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("/local_args", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), no_use_global_namespace_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name");

  char * output = NULL;
  ret = rcl_remap_node_namespace(
    &local_arguments, NULL, "NodeName", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), other_rules_before_namespace_rule) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(
    global_arguments,
    "process_name",
    "--ros-args",
    "-r", "/foobar:=/foo/bar",
    "-r", "__ns:=/namespace",
    "-r", "__node:=new_name");

  rcl_allocator_t allocator = rcl_get_default_allocator();
  char * output = NULL;
  ret = rcl_remap_node_namespace(NULL, &global_arguments, "NodeName", allocator, &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("/namespace", output);
  allocator.deallocate(output, allocator.state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), global_topic_name_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "/bar/foo:=/foo/bar");

  {
    char * output = NULL;
    ret = rcl_remap_topic_name(
      NULL, &global_arguments, "/bar/foo", "NodeName", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    ASSERT_STREQ("/foo/bar", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
  {
    char * output = NULL;
    ret = rcl_remap_topic_name(
      NULL, &global_arguments, "/foo/bar", "NodeName", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_EQ(NULL, output);
  }
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), topic_and_service_name_not_null) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "/bar/foo:=/foo/bar");

  {
    char * output = NULL;
    ret = rcl_remap_service_name(
      NULL, &global_arguments, NULL, "NodeName", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
    ASSERT_EQ(NULL, output);
  }
  {
    char * output = NULL;
    ret = rcl_remap_topic_name(
      NULL, &global_arguments, NULL, "NodeName", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
    EXPECT_EQ(NULL, output);
  }
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), relative_topic_name_remap) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "foo:=bar");

  char * output = NULL;
  ret = rcl_remap_topic_name(
    NULL, &global_arguments, "/ns/foo", "NodeName", "/ns", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  ASSERT_STREQ("/ns/bar", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), nodename_prefix_topic_remap) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(
    global_arguments,
    "process_name",
    "--ros-args",
    "-r", "Node1:/foo:=/foo/bar",
    "-r", "Node2:/foo:=/this_one",
    "-r", "Node3:/foo:=/bar/foo");

  {
    char * output = NULL;
    ret = rcl_remap_topic_name(
      NULL, &global_arguments, "/foo", "Node1", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/foo/bar", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
  {
    char * output = NULL;
    ret = rcl_remap_topic_name(
      NULL, &global_arguments, "/foo", "Node2", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/this_one", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
  {
    char * output = NULL;
    ret = rcl_remap_topic_name(
      NULL, &global_arguments, "/foo", "Node3", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/bar/foo", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), no_use_global_topic_name_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name");

  char * output = NULL;
  ret = rcl_remap_topic_name(
    &local_arguments, NULL, "/bar/foo", "NodeName", "/", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), no_topic_name_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name");

  char * output = NULL;
  ret = rcl_remap_topic_name(
    NULL, &global_arguments, "/bar/foo", "NodeName", "/", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), local_topic_replacement_before_global) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "/bar/foo:=/foo/global_args");
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name", "--ros-args", "-r", "/bar/foo:=/foo/local_args");

  char * output = NULL;
  ret = rcl_remap_topic_name(
    &local_arguments, &global_arguments, "/bar/foo", "NodeName", "/", rcl_get_default_allocator(),
    &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("/foo/local_args", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), other_rules_before_topic_rule) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(
    global_arguments,
    "process_name",
    "--ros-args",
    "-r", "__ns:=/namespace",
    "-r", "__node:=remap_name",
    "-r", "/foobar:=/foo/bar");

  rcl_allocator_t allocator = rcl_get_default_allocator();
  char * output = NULL;
  ret = rcl_remap_topic_name(
    NULL, &global_arguments, "/foobar", "NodeName", "/", allocator, &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("/foo/bar", output);
  allocator.deallocate(output, allocator.state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), global_service_name_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "/bar/foo:=/foo/bar");

  {
    char * output = NULL;
    ret = rcl_remap_service_name(
      NULL, &global_arguments, "/bar/foo", "NodeName", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    ASSERT_STREQ("/foo/bar", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
  {
    char * output = NULL;
    ret = rcl_remap_service_name(
      NULL, &global_arguments, "/foobar", "NodeName", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_EQ(NULL, output);
  }
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), relative_service_name_remap) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "foo:=bar");

  char * output = NULL;
  ret = rcl_remap_service_name(
    NULL, &global_arguments, "/ns/foo", "NodeName", "/ns", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  ASSERT_STREQ("/ns/bar", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), nodename_prefix_service_remap) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(
    global_arguments,
    "process_name",
    "--ros-args",
    "-r", "Node1:/foo:=/foo/bar",
    "-r", "Node2:/foo:=/this_one",
    "-r", "Node3:/foo:=/bar/foo");

  {
    char * output = NULL;
    ret = rcl_remap_service_name(
      NULL, &global_arguments, "/foo", "Node1", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/foo/bar", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
  {
    char * output = NULL;
    ret = rcl_remap_service_name(
      NULL, &global_arguments, "/foo", "Node2", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/this_one", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
  {
    char * output = NULL;
    ret = rcl_remap_service_name(
      NULL, &global_arguments, "/foo", "Node3", "/", rcl_get_default_allocator(), &output);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_STREQ("/bar/foo", output);
    rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
  }
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), no_use_global_service_name_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name");

  char * output = NULL;
  ret = rcl_remap_service_name(
    &local_arguments, NULL, "/bar/foo", "NodeName", "/", rcl_get_default_allocator(),
    &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), no_service_name_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name");

  char * output = NULL;
  ret = rcl_remap_service_name(
    NULL, &global_arguments, "/bar/foo", "NodeName", "/", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), local_service_replacement_before_global) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "/bar/foo:=/foo/global_args");
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name", "--ros-args", "-r", "/bar/foo:=/foo/local_args");

  char * output = NULL;
  ret = rcl_remap_service_name(
    &local_arguments, &global_arguments, "/bar/foo", "NodeName", "/", rcl_get_default_allocator(),
    &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("/foo/local_args", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), other_rules_before_service_rule) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(
    global_arguments,
    "process_name",
    "--ros-args",
    "-r", "__ns:=/namespace",
    "-r", "__node:=remap_name",
    "-r", "/foobar:=/foo/bar");

  rcl_allocator_t allocator = rcl_get_default_allocator();
  char * output = NULL;
  ret = rcl_remap_service_name(
    NULL, &global_arguments, "/foobar", "NodeName", "/", allocator, &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("/foo/bar", output);
  allocator.deallocate(output, allocator.state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), global_nodename_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "__node:=globalname");

  rcl_allocator_t allocator = rcl_get_default_allocator();
  char * output = NULL;
  ret = rcl_remap_node_name(NULL, &global_arguments, "NodeName", allocator, &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("globalname", output);
  allocator.deallocate(output, allocator.state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), no_nodename_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name");

  char * output = NULL;
  ret = rcl_remap_node_name(
    NULL, &global_arguments, "NodeName", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), local_nodename_replacement_before_global) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "__node:=global_name");
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name", "--ros-args", "-r", "__node:=local_name");

  char * output = NULL;
  ret = rcl_remap_node_name(
    &local_arguments, &global_arguments, "NodeName", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("local_name", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), no_use_global_nodename_replacement) {
  rcl_ret_t ret;
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name");

  char * output = NULL;
  ret = rcl_remap_node_name(
    &local_arguments, NULL, "NodeName", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), use_first_nodename_rule) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(
    global_arguments,
    "process_name",
    "--ros-args",
    "-r", "__node:=firstname",
    "-r", "__node:=secondname");

  rcl_allocator_t allocator = rcl_get_default_allocator();
  char * output = NULL;
  ret = rcl_remap_node_name(NULL, &global_arguments, "NodeName", allocator, &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("firstname", output);
  allocator.deallocate(output, allocator.state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), other_rules_before_nodename_rule) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(
    global_arguments,
    "process_name",
    "--ros-args",
    "-r", "/foobar:=/foo",
    "-r", "__ns:=/namespace",
    "-r", "__node:=remap_name");

  rcl_allocator_t allocator = rcl_get_default_allocator();
  char * output = NULL;
  ret = rcl_remap_node_name(NULL, &global_arguments, "NodeName", allocator, &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("remap_name", output);
  allocator.deallocate(output, allocator.state);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), url_scheme_rosservice) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "rosservice://foo:=bar");

  char * output = NULL;
  ret = rcl_remap_service_name(
    NULL, &global_arguments, "/ns/foo", "NodeName", "/ns", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  ASSERT_STREQ("/ns/bar", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);

  ret = rcl_remap_topic_name(
    NULL, &global_arguments, "/ns/foo", "NodeName", "/ns", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), url_scheme_rostopic) {
  rcl_ret_t ret;
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "rostopic://foo:=bar");

  char * output = NULL;
  ret = rcl_remap_topic_name(
    NULL, &global_arguments, "/ns/foo", "NodeName", "/ns", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  ASSERT_STREQ("/ns/bar", output);
  rcl_get_default_allocator().deallocate(output, rcl_get_default_allocator().state);

  ret = rcl_remap_service_name(
    NULL, &global_arguments, "/ns/foo", "NodeName", "/ns", rcl_get_default_allocator(), &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(NULL, output);
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), _rcl_remap_name_bad_arg) {
  rcl_arguments_t global_arguments;
  SCOPE_ARGS(global_arguments, "process_name", "--ros-args", "-r", "__node:=global_name");
  rcl_arguments_t local_arguments;
  SCOPE_ARGS(local_arguments, "process_name", "--ros-args", "-r", "__node:=local_name");
  rcl_arguments_t zero_init_global_arguments = rcl_get_zero_initialized_arguments();
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_allocator_t bad_allocator = get_failing_allocator();
  char * output = NULL;

  // Expected usage local_args, global not init is OK
  rcl_ret_t ret = rcl_remap_node_name(
    &local_arguments, &zero_init_global_arguments, "NodeName", allocator, &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("local_name", output);
  allocator.deallocate(output, allocator.state);

  // Expected usage global_args, local not null is OK
  ret = rcl_remap_node_name(nullptr, &global_arguments, "NodeName", allocator, &output);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_STREQ("global_name", output);
  allocator.deallocate(output, allocator.state);

  // Both local and global arguments, not valid
  ret = rcl_remap_node_name(nullptr, nullptr, "NodeName", allocator, &output);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();

  // Bad allocator
  ret = rcl_remap_node_name(nullptr, &global_arguments, "NodeName", bad_allocator, &output);
  EXPECT_EQ(RCL_RET_ERROR, ret);
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestRemapFixture, RMW_IMPLEMENTATION), internal_remap_use) {
  // Easiest way to init a rcl_remap is through the arguments API
  const char * argv[] = {
    "process_name", "--ros-args", "-r", "__ns:=/namespace", "random:=arg"
  };
  int argc = sizeof(argv) / sizeof(const char *);
  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();

  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  // Bad alloc
  rcl_remap_t remap_dst = rcl_get_zero_initialized_remap();
  parsed_args.impl->remap_rules->impl->allocator = get_failing_allocator();
  EXPECT_EQ(RCL_RET_BAD_ALLOC, rcl_remap_copy(parsed_args.impl->remap_rules, &remap_dst));
  parsed_args.impl->remap_rules->impl->allocator = alloc;

  // Not valid null ptrs
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_remap_copy(nullptr, &remap_dst));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_remap_copy(parsed_args.impl->remap_rules, nullptr));
  rcl_reset_error();

  // Not valid empty source
  rcl_remap_t remap_empty = rcl_get_zero_initialized_remap();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_remap_copy(&remap_empty, &remap_dst));
  rcl_reset_error();

  // Expected usage
  EXPECT_EQ(RCL_RET_OK, rcl_remap_copy(parsed_args.impl->remap_rules, &remap_dst));

  // Copy twice
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_remap_copy(parsed_args.impl->remap_rules, &remap_dst));
  rcl_reset_error();

  // Fini
  EXPECT_EQ(RCL_RET_OK, rcl_remap_fini(&remap_dst));

  // Fini twice
  EXPECT_EQ(RCL_RET_ERROR, rcl_remap_fini(&remap_dst));
  rcl_reset_error();

  // Bad fini
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_remap_fini(nullptr));
}
