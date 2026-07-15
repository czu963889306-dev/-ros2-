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

#include "gmock/gmock.h"

#include "rcutils/strdup.h"

#include "./time_bomb_allocator_testing_utils.h"
#include "rmw/error_handling.h"
#include "rmw/subscription_content_filter_options.h"

TEST(rmw_subscription_content_filter_options, get_zero_init)
{
  rmw_subscription_content_filter_options_t options =
    rmw_get_zero_initialized_content_filter_options();
  EXPECT_EQ(options.filter_expression, nullptr);
  EXPECT_EQ(options.expression_parameters.size, 0u);
  EXPECT_EQ(options.expression_parameters.data, nullptr);
}

TEST(rmw_subscription_content_filter_options, options_init)
{
  rmw_subscription_content_filter_options_t options =
    rmw_get_zero_initialized_content_filter_options();
  EXPECT_EQ(options.filter_expression, nullptr);
  EXPECT_EQ(options.expression_parameters.size, 0u);
  EXPECT_EQ(options.expression_parameters.data, nullptr);

  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_init(
      nullptr, 0, nullptr, &allocator, &options));
  rmw_reset_error();

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_init(
      "filter='p'", 1, nullptr, &allocator, &options));
  rmw_reset_error();

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_init(
      "filter='p'", 0, nullptr, &allocator, nullptr));
  rmw_reset_error();

  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_init(
        "filter='p'", 0, nullptr, &allocator, &options));
    EXPECT_STREQ(options.filter_expression, "filter='p'");

    EXPECT_EQ(
      RMW_RET_OK,
      rmw_subscription_content_filter_options_fini(&options, &allocator));
  }

  {
    const char * filter_expression = "(filter1=%0 OR filter1=%1) AND filter2=%2";
    const char * expression_parameters[] = {
      "1", "2", "3",
    };
    size_t expression_parameters_count = sizeof(expression_parameters) / sizeof(char *);
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_init(
        filter_expression, expression_parameters_count, expression_parameters, &allocator,
        &options));

    EXPECT_STREQ(options.filter_expression, filter_expression);
    ASSERT_EQ(expression_parameters_count, options.expression_parameters.size);
    for (size_t i = 0; i < expression_parameters_count; ++i) {
      EXPECT_STREQ(options.expression_parameters.data[i], expression_parameters[i]);
    }

    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_fini(&options, &allocator));
  }
}

TEST(rmw_subscription_content_filter_options, options_set)
{
  rmw_subscription_content_filter_options_t options =
    rmw_get_zero_initialized_content_filter_options();
  EXPECT_EQ(options.filter_expression, nullptr);
  EXPECT_EQ(options.expression_parameters.size, 0u);
  EXPECT_EQ(options.expression_parameters.data, nullptr);

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  EXPECT_EQ(
    RMW_RET_OK, rmw_subscription_content_filter_options_init(
      "filter='p'", 0, nullptr, &allocator, &options));
  EXPECT_STREQ(options.filter_expression, "filter='p'");

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_set(
      nullptr, 0, nullptr, &allocator, &options));
  rmw_reset_error();

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_set(
      "filter='p'", 1, nullptr, &allocator, &options));
  rmw_reset_error();

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_set(
      "filter='p'", 0, nullptr, &allocator, nullptr));
  rmw_reset_error();

  {
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_set(
        "filter='p1'", 0, nullptr, &allocator, &options));
    EXPECT_STREQ(options.filter_expression, "filter='p1'");
  }

  {
    const char * filter_expression = "(filter1=%0 OR filter1=%1) AND filter2=%2";
    const char * expression_parameters[] = {
      "1", "2", "3",
    };
    size_t expression_parameters_count = sizeof(expression_parameters) / sizeof(char *);
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_set(
        filter_expression, expression_parameters_count, expression_parameters, &allocator,
        &options));

    EXPECT_STREQ(options.filter_expression, filter_expression);
    ASSERT_EQ(expression_parameters_count, options.expression_parameters.size);
    for (size_t i = 0; i < expression_parameters_count; ++i) {
      EXPECT_STREQ(options.expression_parameters.data[i], expression_parameters[i]);
    }

    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_fini(&options, &allocator));
  }

  EXPECT_EQ(RMW_RET_OK, rmw_subscription_content_filter_options_fini(&options, &allocator));
}

TEST(rmw_subscription_content_filter_options, options_copy) {
  rmw_subscription_content_filter_options_t source =
    rmw_get_zero_initialized_content_filter_options();
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  const char * filter_expression = "(filter1=%0 OR filter1=%1) AND filter2=%2";
  const char * expression_parameters[] = {
    "1", "2", "3",
  };
  size_t expression_parameters_count = sizeof(expression_parameters) / sizeof(char *);

  EXPECT_EQ(
    RMW_RET_OK, rmw_subscription_content_filter_options_init(
      filter_expression, expression_parameters_count, expression_parameters, &allocator, &source));

  rmw_subscription_content_filter_options_t destination =
    rmw_get_zero_initialized_content_filter_options();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_copy(nullptr, &allocator, &destination));
  rcutils_reset_error();

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_copy(&source, nullptr, &destination));
  rcutils_reset_error();

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_copy(&source, &allocator, nullptr));
  rcutils_reset_error();

  rcutils_allocator_t failing_allocator = get_time_bomb_allocator();
  constexpr int expected_num_malloc_calls = 4;
  for (int i = 0; i < expected_num_malloc_calls; ++i) {
    set_time_bomb_allocator_malloc_count(failing_allocator, i);
    EXPECT_EQ(
      RMW_RET_BAD_ALLOC,
      rmw_subscription_content_filter_options_copy(
        &source, &failing_allocator, &destination));
    rcutils_reset_error();
  }

  EXPECT_EQ(
    RMW_RET_OK,
    rmw_subscription_content_filter_options_copy(&source, &allocator, &destination));

  EXPECT_STREQ(source.filter_expression, destination.filter_expression);
  int res = 0;
  EXPECT_EQ(
    RCUTILS_RET_OK,
    rcutils_string_array_cmp(
      &source.expression_parameters, &destination.expression_parameters, &res));
  EXPECT_EQ(res, 0);

  {
    // second copy operation
    rmw_subscription_content_filter_options_t source2 =
      rmw_get_zero_initialized_content_filter_options();
    const char * filter_expression = "filter='p1'";
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_init(
        filter_expression, 0, nullptr, &allocator, &source2));
    EXPECT_STREQ(source2.filter_expression, filter_expression);
    EXPECT_EQ(
      RMW_RET_OK,
      rmw_subscription_content_filter_options_copy(&source2, &allocator, &destination));
    EXPECT_STREQ(source2.filter_expression, destination.filter_expression);
    EXPECT_EQ(source2.expression_parameters.size, 0u);
    EXPECT_EQ(source2.expression_parameters.data, nullptr);
    EXPECT_EQ(
      RMW_RET_OK, rmw_subscription_content_filter_options_fini(&source2, &allocator));
  }

  EXPECT_EQ(RMW_RET_OK, rmw_subscription_content_filter_options_fini(&source, &allocator));
  EXPECT_EQ(
    RMW_RET_OK,
    rmw_subscription_content_filter_options_fini(&destination, &allocator));
}

TEST(rmw_subscription_content_filter_options, content_filter_options_fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_fini(nullptr, &allocator));
  rcutils_reset_error();

  rmw_subscription_content_filter_options_t options =
    rmw_get_zero_initialized_content_filter_options();
  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rmw_subscription_content_filter_options_fini(&options, nullptr));
  rcutils_reset_error();

  EXPECT_EQ(RMW_RET_OK, rmw_subscription_content_filter_options_fini(&options, &allocator));
}
