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
#include <sstream>
#include <string>
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcpputils/filesystem_helper.hpp"

#include "rcl/rcl.h"
#include "rcl/arguments.h"
#include "rcl/error_handling.h"

#include "rcl_yaml_param_parser/parser.h"

#include "rcutils/testing/fault_injection.h"

#include "./allocator_testing_utils.h"
#include "./arguments_impl.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestArgumentsFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
    test_path /= "test_arguments";
  }

  void TearDown()
  {
  }

  rcpputils::fs::path test_path{TEST_RESOURCES_DIRECTORY};
};

#define EXPECT_UNPARSED(parsed_args, ...) \
  do { \
    int expect_unparsed[] = {__VA_ARGS__}; \
    int expect_num_unparsed = sizeof(expect_unparsed) / sizeof(int); \
    rcl_allocator_t alloc = rcl_get_default_allocator(); \
    int actual_num_unparsed = rcl_arguments_get_count_unparsed(&parsed_args); \
    int * actual_unparsed = NULL; \
    if (actual_num_unparsed > 0) { \
      rcl_ret_t ret = rcl_arguments_get_unparsed(&parsed_args, alloc, &actual_unparsed); \
      ASSERT_EQ(RCL_RET_OK, ret); \
      ASSERT_TRUE(NULL != actual_unparsed); \
    } \
    std::stringstream expected; \
    expected << "["; \
    for (int e = 0; e < expect_num_unparsed; ++e) { \
      expected << expect_unparsed[e] << ", "; \
    } \
    expected << "]"; \
    std::stringstream actual; \
    actual << "["; \
    for (int a = 0; a < actual_num_unparsed; ++a) { \
      actual << actual_unparsed[a] << ", "; \
    } \
    actual << "]"; \
    if (NULL != actual_unparsed) { \
      alloc.deallocate(actual_unparsed, alloc.state); \
    } \
    EXPECT_EQ(expected.str(), actual.str()); \
  } while (0)

#define EXPECT_UNPARSED_ROS(parsed_args, ...) \
  do { \
    int expect_unparsed_ros[] = {__VA_ARGS__}; \
    int expect_num_unparsed_ros = sizeof(expect_unparsed_ros) / sizeof(int); \
    rcl_allocator_t alloc = rcl_get_default_allocator(); \
    int actual_num_unparsed_ros = rcl_arguments_get_count_unparsed_ros(&parsed_args); \
    int * actual_unparsed_ros = NULL; \
    if (actual_num_unparsed_ros > 0) { \
      rcl_ret_t ret = rcl_arguments_get_unparsed_ros(&parsed_args, alloc, &actual_unparsed_ros); \
      ASSERT_EQ(RCL_RET_OK, ret); \
      ASSERT_TRUE(NULL != actual_unparsed_ros); \
    } \
    std::stringstream expected; \
    expected << "["; \
    for (int e = 0; e < expect_num_unparsed_ros; ++e) { \
      expected << expect_unparsed_ros[e] << ", "; \
    } \
    expected << "]"; \
    std::stringstream actual; \
    actual << "["; \
    for (int a = 0; a < actual_num_unparsed_ros; ++a) { \
      actual << actual_unparsed_ros[a] << ", "; \
    } \
    actual << "]"; \
    if (NULL != actual_unparsed_ros) { \
      alloc.deallocate(actual_unparsed_ros, alloc.state); \
    } \
    EXPECT_EQ(expected.str(), actual.str()); \
  } while (0)

bool
are_known_ros_args(std::vector<const char *> argv)
{
  const int argc = static_cast<int>(argv.size());
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(
    argc, argv.data(), rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  bool is_valid = (
    0 == rcl_arguments_get_count_unparsed(&parsed_args) &&
    0 == rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  return is_valid;
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), check_known_vs_unknown_args) {
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "__node:=node_name"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "old_name:__node:=node_name"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "old_name:__node:=nodename123"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "__node:=nodename123"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "__ns:=/foo/bar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "__ns:=/"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "_:=kq"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "nodename:__ns:=/foobar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "foo:=bar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "~/foo:=~/bar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "/foo/bar:=bar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "foo:=/bar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "/foo123:=/bar123"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "node:/foo123:=/bar123"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "rostopic:=/foo/bar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "rosservice:=baz"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "rostopic://rostopic:=rosservice"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "rostopic:///rosservice:=rostopic"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-r", "rostopic:///foo/bar:=baz"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-p", "foo:=bar"}));
  // TODO(ivanpauno): Currently, we're accepting `/`, as they're being accepted by qos overrides.
  //                  We might need to revisit qos overrides parameters names if ROS 2 URIs get
  //                  modified.
  EXPECT_TRUE(
    are_known_ros_args(
      {"--ros-args", "-p", "qos_overrides./foo/bar.publisher.history:=keep_last"}));
  // TODO(hidmic): restore tests (and drop the following ones) when parameter names
  //               are standardized to use slashes in lieu of dots.
  // EXPECT_TRUE(are_known_ros_args({"--ros-args", "-p", "~/foo:=~/bar"}));
  // EXPECT_TRUE(are_known_ros_args({"--ros-args", "-p", "foo:=/bar"}));
  // EXPECT_TRUE(are_known_ros_args({"--ros-args", "-p", "/foo123:=/bar123"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-p", "foo.bar:=bar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-p", "node:foo:=bar"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "-p", "fizz123:=buzz456"}));

  const std::string parameters_filepath = (test_path / "test_parameters.1.yaml").string();
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--params-file", parameters_filepath.c_str()}));

  EXPECT_FALSE(are_known_ros_args({"--ros-args", "--custom-ros-arg"}));
  EXPECT_FALSE(are_known_ros_args({"--ros-args", "__node:=node_name"}));
  EXPECT_FALSE(are_known_ros_args({"--ros-args", "old_name:__node:=node_name"}));
  EXPECT_FALSE(are_known_ros_args({"--ros-args", "/foo/bar:=bar"}));
  EXPECT_FALSE(are_known_ros_args({"--ros-args", "foo:=/bar"}));
  EXPECT_FALSE(are_known_ros_args({"--ros-args", "file_name.yaml"}));

  // Setting config logging file
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-config-file", "file.config"}));

  // Setting logger level
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-level", "UNSET"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-level", "DEBUG"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-level", "INFO"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-level", "WARN"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-level", "ERROR"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-level", "FATAL"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-level", "debug"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--log-level", "Info"}));

  EXPECT_FALSE(are_known_ros_args({"--ros-args", "--log", "foo"}));
  EXPECT_FALSE(are_known_ros_args({"--ros-args", "--loglevel", "foo"}));

  // Disabling logging
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--enable-rosout-logs"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--disable-rosout-logs"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--enable-stdout-logs"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--disable-stdout-logs"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--enable-external-lib-logs"}));
  EXPECT_TRUE(are_known_ros_args({"--ros-args", "--disable-external-lib-logs"}));

  EXPECT_FALSE(are_known_ros_args({"--ros-args", "stdout-logs"}));
  EXPECT_FALSE(are_known_ros_args({"--ros-args", "external-lib-logs"}));
  EXPECT_FALSE(are_known_ros_args({"--ros-args", "external-lib-logs"}));
}

bool
are_valid_ros_args(std::vector<const char *> argv)
{
  const int argc = static_cast<int>(argv.size());
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(
    argc, argv.data(), rcl_get_default_allocator(), &parsed_args);
  if (RCL_RET_OK != ret) {
    EXPECT_EQ(ret, RCL_RET_INVALID_ROS_ARGS) << rcl_get_error_string().str;
    rcl_reset_error();
    return false;
  }
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  return true;
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), check_valid_vs_invalid_args) {
  const std::string parameters_filepath = (test_path / "test_parameters.1.yaml").string();
  EXPECT_TRUE(
    are_valid_ros_args(
  {
    "--ros-args", "-p", "foo:=bar", "-r", "__node:=node_name",
    "--params-file", parameters_filepath.c_str(), "--log-level", "INFO",
    "--log-config-file", "file.config"
  }));

  // ROS args unknown to rcl are not (necessarily) invalid
  EXPECT_TRUE(are_valid_ros_args({"--ros-args", "--custom-ros-arg"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "--remap"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", ":"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "1"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "~"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", ":="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "foo:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", ":=bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "::="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "1:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "~:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "__node:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "__node:=/foo/bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "__ns:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "__ns:=foo"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", ":__node:=nodename"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "~:__node:=nodename"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "}foo:=/bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "f oo:=/bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "foo:=/b ar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "f{oo:=/bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "foo:=/b}ar"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "rostopic://:=rosservice"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-r", "rostopic::=rosservice"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", ":="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "foo:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", ":=bar"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", ":"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "1"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "~"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "::="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "1:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "~:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "__node:="}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "__node:=/foo/bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "__ns:=foo"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", ":__node:=nodename"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "~:__node:=nodename"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "}foo:=/bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "--param", "}foo:=/bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-p", "f oo:=/bar"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "--param", "f oo:=/bar"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "-e"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "--enclave"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "--params-file"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "--log-config-file"}));

  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "--log-level"}));
  EXPECT_FALSE(are_valid_ros_args({"--ros-args", "--log-level", "foo"}));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_no_args) {
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(0, NULL, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed(&parsed_args));
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_null_args) {
  const int argc = 1;
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, NULL, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_negative_args) {
  const int argc = -1;
  const char * const argv[] = {"process_name"};
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_bad_alloc_parse_args) {
  const char * const argv[] = {"process_name"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_allocator_t bad_alloc = get_failing_allocator();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, bad_alloc, &parsed_args);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_bad_alloc_unparse_args) {
  const char * const argv[] = {
    "process_name", "--ros-args", "/foo/bar:=", "-r", "bar:=/fiz/buz", "}bar:=fiz", "--", "arg"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_allocator_t bad_alloc = get_failing_allocator();
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(2, rcl_arguments_get_count_unparsed(&parsed_args));

  int * actual_unparsed = NULL;
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC, rcl_arguments_get_unparsed(&parsed_args, bad_alloc, &actual_unparsed));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_BAD_ALLOC, rcl_arguments_get_unparsed_ros(&parsed_args, bad_alloc, &actual_unparsed));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_arguments_get_unparsed_ros(nullptr, allocator, &actual_unparsed));
  rcl_reset_error();

  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_empty_unparsed) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_arguments_t empty_parsed_args = rcl_get_zero_initialized_arguments();
  int * actual_unparsed = NULL;
  int * actual_unparsed_ros = NULL;

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_arguments_get_unparsed(&empty_parsed_args, allocator, &actual_unparsed));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_arguments_get_unparsed_ros(&empty_parsed_args, allocator, &actual_unparsed_ros));
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_bad_params_get_counts) {
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  EXPECT_EQ(-1, rcl_arguments_get_count_unparsed(nullptr));
  rcl_reset_error();
  EXPECT_EQ(-1, rcl_arguments_get_count_unparsed_ros(nullptr));
  rcl_reset_error();
  EXPECT_EQ(-1, rcl_arguments_get_param_files_count(nullptr));
  rcl_reset_error();
  EXPECT_EQ(-1, rcl_arguments_get_count_unparsed(&parsed_args));
  rcl_reset_error();
  EXPECT_EQ(-1, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  rcl_reset_error();
  EXPECT_EQ(-1, rcl_arguments_get_param_files_count(&parsed_args));
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_null_args_output) {
  const char * const argv[] = {"process_name"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_no_ros_args) {
  const char * const argv[] = {"process_name"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_zero_ros_args) {
  const char * const argv[] = {"process_name", "--ros-args"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_zero_ros_args_w_trailing_dashes) {
  const char * const argv[] = {"process_name", "--ros-args", "--"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_remap) {
  const char * const argv[] = {
    "process_name", "--ros-args", "-r", "/foo/bar:=/fiz/buz", "--remap", "foo:=/baz"
  };
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;
  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_one_remap_two_ros_args) {
  const char * const argv[] =
  {"process_name", "--ros-args", "--ros-args", "-r", "/foo/bar:=/fiz/buz"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;
  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_one_remap_w_trailing_dashes) {
  const char * const argv[] = {"process_name", "--ros-args", "-r", "/foo/bar:=/fiz/buz", "--"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;
  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_one_remap_w_two_trailing_dashes) {
  const char * const argv[] =
  {"process_name", "--ros-args", "-r", "/foo/bar:=/fiz/buz", "--", "--"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;
  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0, 5);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_mix_valid_invalid_rules) {
  const char * const argv[] = {
    "process_name", "--ros-args", "/foo/bar:=", "-r", "bar:=/fiz/buz", "}bar:=fiz", "--", "arg"
  };
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;
  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0, 7);
  EXPECT_UNPARSED_ROS(parsed_args, 2, 5);
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_copy) {
  const char * const argv[] = {
    "process_name", "--ros-args", "/foo/bar:=", "-r", "bar:=/fiz/buz", "-r", "__ns:=/foo", "--",
    "arg"
  };
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;

  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_arguments_t copied_args = rcl_get_zero_initialized_arguments();
  ret = rcl_arguments_copy(&parsed_args, &copied_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Can't copy to non empty
  ret = rcl_arguments_copy(&parsed_args, &copied_args);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  EXPECT_UNPARSED(parsed_args, 0, 8);
  EXPECT_UNPARSED_ROS(parsed_args, 2);
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));

  EXPECT_UNPARSED(copied_args, 0, 8);
  EXPECT_UNPARSED_ROS(copied_args, 2);
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&copied_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_copy_bad_alloc) {
  const char * const argv[] = {"process_name", "--ros-args", "/foo/bar:="};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;

  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_arguments_t copied_args = rcl_get_zero_initialized_arguments();
  rcl_allocator_t bad_alloc = get_failing_allocator();
  rcl_allocator_t saved_alloc = parsed_args.impl->allocator;
  parsed_args.impl->allocator = bad_alloc;
  ret = rcl_arguments_copy(&parsed_args, &copied_args);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  parsed_args.impl->allocator = saved_alloc;

  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args)) << rcl_get_error_string().str;
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_copy_no_ros_args) {
  const char * const argv[] = {"process_name", "--ros-args", "--", "arg", "--ros-args"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;

  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_arguments_t copied_args = rcl_get_zero_initialized_arguments();
  ret = rcl_arguments_copy(&parsed_args, &copied_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  EXPECT_UNPARSED(parsed_args, 0, 3);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));

  EXPECT_UNPARSED(copied_args, 0, 3);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&copied_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&copied_args));
}

// Similar to the default allocator, but returns NULL when size is zero.
// This is useful for emulating systems where `malloc(0)` return NULL.
// TODO(jacobperron): Consider using this allocate function in other tests
static void *
__return_null_on_zero_allocate(size_t size, void * state)
{
  RCUTILS_UNUSED(state);
  if (size == 0) {
    return NULL;
  }
  return malloc(size);
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_copy_no_args) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  allocator.allocate = __return_null_on_zero_allocate;
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(0, NULL, allocator, &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed(&parsed_args));
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));

  rcl_arguments_t copied_args = rcl_get_zero_initialized_arguments();
  ret = rcl_arguments_copy(&parsed_args, &copied_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed(&copied_args));
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&copied_args));

  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&copied_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_two_namespace) {
  const char * const argv[] = {
    "process_name", "--ros-args", "-r", "__ns:=/foo/bar", "-r", "__ns:=/fiz/buz"
  };
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret;
  ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_UNPARSED(parsed_args, 0);
  EXPECT_EQ(0, rcl_arguments_get_count_unparsed_ros(&parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_uninitialized_parsed_args) {
  const char * const argv[] = {"process_name"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args;
  int not_null = 1;
  parsed_args.impl = reinterpret_cast<rcl_arguments_impl_t *>(&not_null);
  ASSERT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args));
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_double_parse) {
  const char * const argv[] = {
    "process_name", "--ros-args", "-r", "__ns:=/foo/bar", "-r", "__ns:=/fiz/buz"
  };
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  ASSERT_EQ(
    RCL_RET_OK,
    rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });
  ASSERT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args));
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_fini_null) {
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_arguments_fini(NULL));
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_fini_impl_null) {
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  parsed_args.impl = NULL;
  EXPECT_EQ(RCL_RET_ERROR, rcl_arguments_fini(&parsed_args));
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_fini_twice) {
  const char * const argv[] = {"process_name"};
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  ASSERT_EQ(RCL_RET_OK, rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  EXPECT_EQ(RCL_RET_ERROR, rcl_arguments_fini(&parsed_args));
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_bad_remove_ros_args) {
  const char * const argv[] = {"process_name"};
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_allocator_t default_allocator = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, default_allocator, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  const char ** nonros_argv = NULL;
  int nonros_argc = 0;

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_remove_ros_arguments(
      NULL, &parsed_args, default_allocator, &nonros_argc, &nonros_argv));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_remove_ros_arguments(
      argv, NULL, default_allocator, &nonros_argc, &nonros_argv));
  rcl_reset_error();

  rcl_allocator_t zero_initialized_allocator =
    (rcl_allocator_t)rcutils_get_zero_initialized_allocator();
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_remove_ros_arguments(
      argv, &parsed_args, zero_initialized_allocator, &nonros_argc, &nonros_argv));
  rcl_reset_error();

  rcl_allocator_t bad_alloc = get_failing_allocator();
  EXPECT_EQ(
    RCL_RET_BAD_ALLOC, rcl_remove_ros_arguments(
      argv, &parsed_args, bad_alloc, &nonros_argc, &nonros_argv));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_remove_ros_arguments(
      argv, &parsed_args, default_allocator, NULL, &nonros_argv));
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_remove_ros_arguments(
      argv, &parsed_args, default_allocator, &nonros_argc, NULL));
  rcl_reset_error();

  rcl_arguments_t zero_initialized_parsed_args = rcl_get_zero_initialized_arguments();
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_remove_ros_arguments(
      argv, &zero_initialized_parsed_args, default_allocator, &nonros_argc, &nonros_argv));
  rcl_reset_error();

  const char * stack_allocated_nonros_argv[] = {"--foo", "--bar"};
  const char ** initialized_nonros_argv = stack_allocated_nonros_argv;
  int initialized_nonros_argc = sizeof(stack_allocated_nonros_argv) / sizeof(const char *);

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_remove_ros_arguments(
      argv, &parsed_args, default_allocator, &initialized_nonros_argc, &initialized_nonros_argv));
  rcl_reset_error();

  rcl_arguments_t no_parsed_args = rcl_get_zero_initialized_arguments();
  ret = rcl_parse_arguments(0, NULL, default_allocator, &no_parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&no_parsed_args));
  });

  ret = rcl_remove_ros_arguments(
    NULL, &no_parsed_args, default_allocator, &nonros_argc, &nonros_argv);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(NULL == nonros_argv);
  EXPECT_EQ(0, nonros_argc);
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_bad_alloc_remove_ros_args) {
  const char * const argv[] = {
    "process_name", "-d", "--ros-args", "-r", "__ns:=/foo/bar", "-r", "__ns:=/fiz/buz", "--",
    "--foo=bar", "--baz", "--ros-args", "--ros-args", "-p", "bar:=baz", "--", "--", "arg",
  };
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int nonros_argc = 0;
  const char ** nonros_argv = NULL;
  rcl_allocator_t bomb_alloc = get_time_bombed_allocator();
  set_time_bombed_allocator_count(bomb_alloc, 1);
  ret = rcl_remove_ros_arguments(
    argv,
    &parsed_args,
    bomb_alloc,
    &nonros_argc,
    &nonros_argv);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret);
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_remove_ros_args) {
  const char * const argv[] = {
    "process_name", "-d", "--ros-args", "-r", "__ns:=/foo/bar", "-r", "__ns:=/fiz/buz", "--",
    "--foo=bar", "--baz", "--ros-args", "--ros-args", "-p", "bar:=baz", "--", "--", "arg",
  };
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int nonros_argc = 0;
  const char ** nonros_argv = NULL;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    alloc.deallocate(nonros_argv, alloc.state);
  });

  ret = rcl_remove_ros_arguments(
    argv,
    &parsed_args,
    alloc,
    &nonros_argc,
    &nonros_argv);

  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ASSERT_EQ(nonros_argc, 6);
  EXPECT_STREQ(nonros_argv[0], "process_name");
  EXPECT_STREQ(nonros_argv[1], "-d");
  EXPECT_STREQ(nonros_argv[2], "--foo=bar");
  EXPECT_STREQ(nonros_argv[3], "--baz");
  EXPECT_STREQ(nonros_argv[4], "--");
  EXPECT_STREQ(nonros_argv[5], "arg");
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_remove_ros_args_if_ros_only) {
  const char * const argv[] = {"--ros-args", "--disable-rosout-logs"};
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int nonros_argc = 0;
  const char ** nonros_argv = NULL;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    alloc.deallocate(nonros_argv, alloc.state);
  });

  ret = rcl_remove_ros_arguments(
    argv,
    &parsed_args,
    alloc,
    &nonros_argc,
    &nonros_argv);

  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  EXPECT_EQ(0, nonros_argc);
  EXPECT_TRUE(NULL == nonros_argv);
}


TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_remove_ros_args_if_no_args) {
  const char ** argv = NULL;
  const int argc = 0;

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int nonros_argc = 0;
  const char ** nonros_argv = NULL;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    alloc.deallocate(nonros_argv, alloc.state);
  });

  ret = rcl_remove_ros_arguments(
    argv,
    &parsed_args,
    alloc,
    &nonros_argc,
    &nonros_argv);

  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  EXPECT_EQ(0, nonros_argc);
  EXPECT_TRUE(NULL == nonros_argv);
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_param_argument_zero) {
  const char * const argv[] =
  {"process_name", "--ros-args", "-r", "__ns:=/namespace", "random:=arg"};
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();

  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int parameter_filecount = rcl_arguments_get_param_files_count(&parsed_args);
  EXPECT_EQ(0, parameter_filecount);
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_param_argument_single) {
  const std::string parameters_filepath = (test_path / "test_parameters.1.yaml").string();
  const char * const argv[] = {
    "process_name", "--ros-args", "-r", "__ns:=/namespace", "random:=arg",
    "--params-file", parameters_filepath.c_str()
  };
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();

  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int parameter_filecount = rcl_arguments_get_param_files_count(&parsed_args);
  EXPECT_EQ(1, parameter_filecount);
  char ** parameter_files = NULL;
  ret = rcl_arguments_get_param_files(&parsed_args, alloc, &parameter_files);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_STREQ(parameters_filepath.c_str(), parameter_files[0]);

  for (int i = 0; i < parameter_filecount; ++i) {
    alloc.deallocate(parameter_files[i], alloc.state);
  }
  alloc.deallocate(parameter_files, alloc.state);

  // Test bad alloc
  rcl_allocator_t bad_alloc = get_failing_allocator();
  rcl_params_t * params_test = NULL;
  rcl_allocator_t saved_alloc = parsed_args.impl->allocator;
  parsed_args.impl->parameter_overrides->allocator = bad_alloc;
  ret = rcl_arguments_get_param_overrides(&parsed_args, &params_test);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;
  EXPECT_EQ(NULL, params_test);
  parsed_args.impl->parameter_overrides->allocator = saved_alloc;

  // Expected usage
  rcl_params_t * params = NULL;
  ret = rcl_arguments_get_param_overrides(&parsed_args, &params);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params);
  });
  EXPECT_EQ(1U, params->num_nodes);

  rcl_variant_t * param_value =
    rcl_yaml_node_struct_get("some_node", "param_group.string_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->string_value);
  EXPECT_STREQ("foo", param_value->string_value);

  param_value = rcl_yaml_node_struct_get("some_node", "int_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->integer_value);
  EXPECT_EQ(1, *(param_value->integer_value));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_param_argument_multiple) {
  const std::string parameters_filepath1 = (test_path / "test_parameters.1.yaml").string();
  const std::string parameters_filepath2 = (test_path / "test_parameters.2.yaml").string();
  const char * const argv[] = {
    "process_name", "--ros-args", "--params-file", parameters_filepath1.c_str(),
    "-r", "__ns:=/namespace", "random:=arg", "--params-file", parameters_filepath2.c_str()
  };
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_ret_t ret;

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();

  ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int parameter_filecount = rcl_arguments_get_param_files_count(&parsed_args);
  EXPECT_EQ(2, parameter_filecount);
  char ** parameter_files = NULL;
  ret = rcl_arguments_get_param_files(&parsed_args, alloc, &parameter_files);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_STREQ(parameters_filepath1.c_str(), parameter_files[0]);
  EXPECT_STREQ(parameters_filepath2.c_str(), parameter_files[1]);
  for (int i = 0; i < parameter_filecount; ++i) {
    alloc.deallocate(parameter_files[i], alloc.state);
  }

  alloc.deallocate(parameter_files, alloc.state);

  rcl_params_t * params = NULL;
  ret = rcl_arguments_get_param_overrides(&parsed_args, &params);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params);
  });
  EXPECT_EQ(2U, params->num_nodes);

  rcl_variant_t * param_value =
    rcl_yaml_node_struct_get("some_node", "int_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->integer_value);
  EXPECT_EQ(3, *(param_value->integer_value));

  param_value = rcl_yaml_node_struct_get("some_node", "param_group.string_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->string_value);
  EXPECT_STREQ("foo", param_value->string_value);

  param_value = rcl_yaml_node_struct_get("another_node", "double_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->double_value);
  EXPECT_DOUBLE_EQ(1.0, *(param_value->double_value));

  param_value = rcl_yaml_node_struct_get("another_node", "param_group.bool_array_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->bool_array_value);
  ASSERT_TRUE(NULL != param_value->bool_array_value->values);
  ASSERT_EQ(3U, param_value->bool_array_value->size);
  bool bool_value = param_value->bool_array_value->values[0];
  EXPECT_TRUE(bool_value);
  bool_value = param_value->bool_array_value->values[1];
  EXPECT_FALSE(bool_value);
  bool_value = param_value->bool_array_value->values[2];
  EXPECT_FALSE(bool_value);
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_param_arguments_copy) {
  const std::string parameters_filepath1 = (test_path / "test_parameters.1.yaml").string();
  const std::string parameters_filepath2 = (test_path / "test_parameters.2.yaml").string();
  const char * const argv[] = {
    "process_name", "--ros-args", "--params-file", parameters_filepath1.c_str(),
    "-r", "__ns:=/namespace", "random:=arg", "--params-file", parameters_filepath2.c_str()
  };
  const int argc = sizeof(argv) / sizeof(const char *);
  rcl_ret_t ret;

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();

  ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int parameter_filecount = rcl_arguments_get_param_files_count(&parsed_args);
  EXPECT_EQ(2, parameter_filecount);

  rcl_arguments_t copied_args = rcl_get_zero_initialized_arguments();
  ret = rcl_arguments_copy(&parsed_args, &copied_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(2, rcl_arguments_get_param_files_count(&copied_args));
  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&copied_args));
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_no_param_overrides) {
  const char * const argv[] = {"process_name"};
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();

  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  ret = rcl_arguments_get_param_overrides(&parsed_args, NULL);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  rcl_params_t * params = NULL;
  ret = rcl_arguments_get_param_overrides(NULL, &params);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  rcl_arguments_t empty_parsed_arg = rcl_get_zero_initialized_arguments();
  ret = rcl_arguments_get_param_overrides(&empty_parsed_arg, &params);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  rcl_params_t preallocated_params;
  params = &preallocated_params;
  ret = rcl_arguments_get_param_overrides(&parsed_args, &params);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  params = NULL;
  ret = rcl_arguments_get_param_overrides(&parsed_args, &params);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(NULL == params);
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_param_overrides) {
  const std::string parameters_filepath = (test_path / "test_parameters.1.yaml").string();
  const char * const argv[] = {
    "process_name", "--ros-args",
    "--params-file", parameters_filepath.c_str(),
    "--param", "string_param:=bar",
    "-p", "some.bool_param:=false",
    "-p", "some_node:int_param:=4"
  };
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();

  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  rcl_params_t * params = NULL;
  ret = rcl_arguments_get_param_overrides(&parsed_args, &params);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params);
  });
  EXPECT_EQ(2U, params->num_nodes);

  rcl_variant_t * param_value =
    rcl_yaml_node_struct_get("/**", "string_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->string_value);
  EXPECT_STREQ("bar", param_value->string_value);

  param_value = rcl_yaml_node_struct_get("/**", "some.bool_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->bool_value);
  bool bool_value = *param_value->bool_value;
  EXPECT_FALSE(bool_value);

  param_value = rcl_yaml_node_struct_get("some_node", "int_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->integer_value);
  EXPECT_EQ(4, *(param_value->integer_value));

  param_value = rcl_yaml_node_struct_get("some_node", "param_group.string_param", params);
  ASSERT_TRUE(NULL != param_value);
  ASSERT_TRUE(NULL != param_value->string_value);
  EXPECT_STREQ("foo", param_value->string_value);
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_bad_alloc_get_param_files) {
  const std::string parameters_filepath1 = (test_path / "test_parameters.1.yaml").string();
  const std::string parameters_filepath2 = (test_path / "test_parameters.2.yaml").string();
  const char * const argv[] = {
    "process_name", "--ros-args", "--params-file", parameters_filepath1.c_str(),
    "-r", "__ns:=/namespace", "random:=arg", "--params-file", parameters_filepath2.c_str()
  };
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  int parameter_filecount = rcl_arguments_get_param_files_count(&parsed_args);
  EXPECT_EQ(2, parameter_filecount);

  // Configure allocator to fail at different points of the code
  rcl_allocator_t bomb_alloc = get_time_bombed_allocator();
  set_time_bombed_allocator_count(bomb_alloc, 0);
  char ** parameter_files = NULL;
  ret = rcl_arguments_get_param_files(&parsed_args, bomb_alloc, &parameter_files);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;

  set_time_bombed_allocator_count(bomb_alloc, 1);
  ret = rcl_arguments_get_param_files(&parsed_args, bomb_alloc, &parameter_files);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;

  set_time_bombed_allocator_count(bomb_alloc, 2);
  ret = rcl_arguments_get_param_files(&parsed_args, bomb_alloc, &parameter_files);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_null_get_param_files) {
  const std::string parameters_filepath1 = (test_path / "test_parameters.1.yaml").string();
  const char * const argv[] = {
    "process_name", "--ros-args", "--params-file", parameters_filepath1.c_str()
  };
  const int argc = sizeof(argv) / sizeof(const char *);

  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  char ** parameter_files = NULL;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret = rcl_parse_arguments(argc, argv, allocator, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  ret = rcl_arguments_get_param_files(nullptr, allocator, &parameter_files);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  ret = rcl_arguments_get_param_files(&parsed_args, allocator, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  rcl_arguments_t empty_parsed_args = rcl_get_zero_initialized_arguments();
  ret = rcl_arguments_get_param_files(&empty_parsed_args, allocator, &parameter_files);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_parse_with_internal_errors) {
  const std::string parameters_filepath1 =
    (test_path / "test_parameters.1.yaml").string();
  const std::string parameters_filepath2 =
    (test_path / "test_parameters.2.yaml").string();
  const char * const argv[] = {
    "process_name", RCL_ROS_ARGS_FLAG,
    RCL_PARAM_FILE_FLAG, parameters_filepath1.c_str(),
    RCL_REMAP_FLAG, "that_node:foo:=baz",
    RCL_REMAP_FLAG, "foo:=bar",
    RCL_PARAM_FILE_FLAG, parameters_filepath2.c_str(),
    RCL_REMAP_FLAG, "__name:=my_node",
    RCL_REMAP_FLAG, "__ns:=/my_ns",
    RCL_PARAM_FLAG, "testing:=true",
    RCL_PARAM_FLAG, "this_node:constant:=42",
    RCL_ENCLAVE_FLAG, "fizz",
    RCL_ENCLAVE_FLAG, "buzz",  // override
    RCL_LOG_LEVEL_FLAG, "rcl:=debug",
    RCL_EXTERNAL_LOG_CONFIG_FLAG, "flip.txt",
    RCL_EXTERNAL_LOG_CONFIG_FLAG, "flop.txt",  // override
    "--enable-" RCL_LOG_STDOUT_FLAG_SUFFIX,
    "--enable-" RCL_LOG_ROSOUT_FLAG_SUFFIX,
    "--disable-" RCL_LOG_EXT_LIB_FLAG_SUFFIX,
    "--not-a-real-ros-flag", "not-a-real-ros-arg",
    RCL_ROS_ARGS_EXPLICIT_END_TOKEN,
    "--some-non-ros-flag", "some-non-ros-flag"
  };
  const int argc = sizeof(argv) / sizeof(argv[0]);
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_ret_t ret = rcl_parse_arguments(argc, argv, allocator, &parsed_args);
    if (RCL_RET_OK == ret) {
      int64_t count = rcutils_fault_injection_get_count();
      rcutils_fault_injection_set_count(RCUTILS_FAULT_INJECTION_NEVER_FAIL);
      ret = rcl_arguments_fini(&parsed_args);
      rcutils_fault_injection_set_count(count);
      EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    } else {
      rcl_reset_error();
    }
  });
}

TEST_F(CLASSNAME(TestArgumentsFixture, RMW_IMPLEMENTATION), test_copy_with_internal_errors) {
  const std::string parameters_filepath1 =
    (test_path / "test_parameters.1.yaml").string();
  const std::string parameters_filepath2 =
    (test_path / "test_parameters.2.yaml").string();
  const char * const argv[] = {
    "process_name", RCL_ROS_ARGS_FLAG,
    RCL_PARAM_FILE_FLAG, parameters_filepath1.c_str(),
    RCL_REMAP_FLAG, "that_node:foo:=baz",
    RCL_REMAP_FLAG, "foo:=bar",
    RCL_PARAM_FILE_FLAG, parameters_filepath2.c_str(),
    RCL_REMAP_FLAG, "__name:=my_node",
    RCL_REMAP_FLAG, "__ns:=/my_ns",
    RCL_PARAM_FLAG, "testing:=true",
    RCL_PARAM_FLAG, "this_node:constant:=42",
    RCL_ENCLAVE_FLAG, "fizz",
    RCL_ENCLAVE_FLAG, "buzz",  // override
    RCL_LOG_LEVEL_FLAG, "rcl:=debug",
    RCL_EXTERNAL_LOG_CONFIG_FLAG, "flip.txt",
    RCL_EXTERNAL_LOG_CONFIG_FLAG, "flop.txt",  // override
    "--enable-" RCL_LOG_STDOUT_FLAG_SUFFIX,
    "--enable-" RCL_LOG_ROSOUT_FLAG_SUFFIX,
    "--disable-" RCL_LOG_EXT_LIB_FLAG_SUFFIX,
    "--not-a-real-ros-flag", "not-a-real-ros-arg",
    RCL_ROS_ARGS_EXPLICIT_END_TOKEN,
    "--some-non-ros-flag", "some-non-ros-flag"
  };
  const int argc = sizeof(argv) / sizeof(argv[0]);

  rcl_allocator_t alloc = rcl_get_default_allocator();
  rcl_arguments_t parsed_args = rcl_get_zero_initialized_arguments();

  rcl_ret_t ret = rcl_parse_arguments(argc, argv, alloc, &parsed_args);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&parsed_args));
  });

  rcl_arguments_t copied_args = rcl_get_zero_initialized_arguments();
  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_ret_t ret = rcl_arguments_copy(&parsed_args, &copied_args);
    if (RCL_RET_OK == ret) {
      int64_t count = rcutils_fault_injection_get_count();
      rcutils_fault_injection_set_count(RCUTILS_FAULT_INJECTION_NEVER_FAIL);
      ret = rcl_arguments_fini(&copied_args);
      rcutils_fault_injection_set_count(count);
      EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    } else {
      rcl_reset_error();
    }
  });
}
