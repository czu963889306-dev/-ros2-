// Copyright 2022 Open Source Robotics Foundation, Inc.
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

#include "rcl/error_handling.h"
#include "rcl/node.h"
#include "rcl/rcl.h"
#include "rcl/subscription.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "test_msgs/msg/basic_types.h"


TEST(TestSubscriptionOptionsContentFilter, subscription_options_failure) {
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();

  const char * filter_expression1 = "filter=1";
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_options_set_content_filter_options(
      nullptr, 0, nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_options_set_content_filter_options(
      filter_expression1, 0, nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_options_set_content_filter_options(
      filter_expression1, 1, nullptr, &subscription_options)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_options_fini(
      nullptr)
  );
  rcl_reset_error();
}

TEST(TestSubscriptionOptionsContentFilter, subscription_options_success)
{
  rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();

  const char * filter_expression1 = "filter=1";

  {
    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_options_set_content_filter_options(
        filter_expression1, 0, nullptr, &subscription_options)
    );

    rmw_subscription_content_filter_options_t * content_filter_options =
      subscription_options.rmw_subscription_options.content_filter_options;
    ASSERT_NE(nullptr, content_filter_options);
    EXPECT_STREQ(filter_expression1, content_filter_options->filter_expression);
    EXPECT_EQ(0u, content_filter_options->expression_parameters.size);
    EXPECT_EQ(nullptr, content_filter_options->expression_parameters.data);
  }

  const char * filter_expression2 = "(filter1=%0 OR filter1=%1) AND filter2=%2";
  const char * expression_parameters2[] = {
    "1", "2", "3",
  };
  size_t expression_parameters_count2 = sizeof(expression_parameters2) / sizeof(char *);

  {
    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_options_set_content_filter_options(
        filter_expression2, expression_parameters_count2,
        expression_parameters2, &subscription_options)
    );

    rmw_subscription_content_filter_options_t * content_filter_options =
      subscription_options.rmw_subscription_options.content_filter_options;
    ASSERT_NE(nullptr, content_filter_options);
    EXPECT_STREQ(filter_expression2, content_filter_options->filter_expression);
    ASSERT_EQ(
      expression_parameters_count2,
      content_filter_options->expression_parameters.size);
    for (size_t i = 0; i < expression_parameters_count2; ++i) {
      EXPECT_STREQ(
        content_filter_options->expression_parameters.data[i],
        expression_parameters2[i]);
    }
  }

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_subscription_options_fini(&subscription_options)
  );
}


class TestSubscriptionContentFilterOptions : public ::testing::Test
{
public:
  std::unique_ptr<rcl_context_t> context_;
  std::unique_ptr<rcl_node_t> node_;
  std::unique_ptr<rcl_subscription_t> subscription_;
  void SetUp()
  {
    rcl_ret_t ret;
    {
      rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
      ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
      {
        EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
      });
      context_ = std::make_unique<rcl_context_t>();
      *context_ = rcl_get_zero_initialized_context();
      ret = rcl_init(0, nullptr, &init_options, &*context_);
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    }
    node_ = std::make_unique<rcl_node_t>();
    *node_ = rcl_get_zero_initialized_node();
    constexpr char name[] = "test_subscription_content_filter_options_node";
    rcl_node_options_t node_options = rcl_node_get_default_options();
    ret = rcl_node_init(&*node_, name, "", &*context_, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    constexpr char topic[] = "chatter";

    subscription_ = std::make_unique<rcl_subscription_t>();
    *subscription_ = rcl_get_zero_initialized_subscription();
    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
    ret = rcl_subscription_init(
      &*subscription_, &*node_, ts, topic, &subscription_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown()
  {
    rcl_ret_t ret = rcl_subscription_fini(&*subscription_, &*node_);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_node_fini(&*node_);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_shutdown(&*context_);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(&*context_);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }
};

TEST_F(TestSubscriptionContentFilterOptions, content_filter_options_failure) {
  rcl_subscription_content_filter_options_t content_filter_options =
    rcl_get_zero_initialized_subscription_content_filter_options();

  const char * filter_expression1 = "filter=1";
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_content_filter_options_init(
      nullptr, nullptr, 0, nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_content_filter_options_init(
      &*subscription_, nullptr, 0, nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_content_filter_options_init(
      &*subscription_, filter_expression1, 0, nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_content_filter_options_init(
      &*subscription_, filter_expression1, 1, nullptr, &content_filter_options)
  );
  rcl_reset_error();

  // set
  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_content_filter_options_set(
      nullptr, nullptr, 0, nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_content_filter_options_set(
      &*subscription_, nullptr, 0, nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_content_filter_options_set(
      &*subscription_, filter_expression1, 0, nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_content_filter_options_set(
      &*subscription_, filter_expression1, 1, nullptr, &content_filter_options)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_SUBSCRIPTION_INVALID,
    rcl_subscription_content_filter_options_fini(
      nullptr, nullptr)
  );
  rcl_reset_error();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_subscription_content_filter_options_fini(
      &*subscription_, nullptr)
  );
  rcl_reset_error();
}

TEST_F(TestSubscriptionContentFilterOptions, content_filter_options_success)
{
  rmw_subscription_content_filter_options_t * content_filter_options;
  const char * filter_expression1 = "filter=1";
  const char * filter_expression1_update = "filter=2";

  rcl_subscription_content_filter_options_t subscription_content_filter_options =
    rcl_get_zero_initialized_subscription_content_filter_options();
  {
    // init with filter_expression1
    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_init(
        &*subscription_, filter_expression1, 0, nullptr,
        &subscription_content_filter_options)
    );

    content_filter_options =
      &subscription_content_filter_options.rmw_subscription_content_filter_options;
    EXPECT_STREQ(filter_expression1, content_filter_options->filter_expression);
    EXPECT_EQ(0u, content_filter_options->expression_parameters.size);
    EXPECT_EQ(nullptr, content_filter_options->expression_parameters.data);

    // set with filter_expression1_update
    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_set(
        &*subscription_, filter_expression1_update, 0, nullptr,
        &subscription_content_filter_options)
    );

    content_filter_options =
      &subscription_content_filter_options.rmw_subscription_content_filter_options;
    EXPECT_STREQ(filter_expression1_update, content_filter_options->filter_expression);
    EXPECT_EQ(0u, content_filter_options->expression_parameters.size);
    EXPECT_EQ(nullptr, content_filter_options->expression_parameters.data);
  }

  const char * filter_expression2 = "(filter1=%0 OR filter1=%1) AND filter2=%2";
  const char * expression_parameters2[] = {
    "1", "2", "3",
  };
  size_t expression_parameters_count2 = sizeof(expression_parameters2) / sizeof(char *);

  const char * filter_expression2_update = "(filter1=%0 AND filter1=%1) OR filter2=%2";
  const char * expression_parameters2_update[] = {
    "11", "22", "33",
  };
  size_t expression_parameters_count2_update = sizeof(expression_parameters2) / sizeof(char *);

  rcl_subscription_content_filter_options_t subscription_content_filter_options2 =
    rcl_get_zero_initialized_subscription_content_filter_options();
  {
    // init with filter_expression2 and expression_parameters2
    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_init(
        &*subscription_, filter_expression2, expression_parameters_count2,
        expression_parameters2, &subscription_content_filter_options2)
    );

    content_filter_options =
      &subscription_content_filter_options2.rmw_subscription_content_filter_options;
    ASSERT_NE(nullptr, content_filter_options);
    EXPECT_STREQ(filter_expression2, content_filter_options->filter_expression);
    ASSERT_EQ(
      expression_parameters_count2,
      content_filter_options->expression_parameters.size);
    for (size_t i = 0; i < expression_parameters_count2; ++i) {
      EXPECT_STREQ(
        content_filter_options->expression_parameters.data[i],
        expression_parameters2[i]);
    }

    // set with filter_expression2_update and expression_parameters2_update
    EXPECT_EQ(
      RCL_RET_OK,
      rcl_subscription_content_filter_options_set(
        &*subscription_, filter_expression2_update, expression_parameters_count2_update,
        expression_parameters2_update, &subscription_content_filter_options2)
    );

    content_filter_options =
      &subscription_content_filter_options2.rmw_subscription_content_filter_options;
    ASSERT_NE(nullptr, content_filter_options);
    EXPECT_STREQ(filter_expression2_update, content_filter_options->filter_expression);
    ASSERT_EQ(
      expression_parameters_count2_update,
      content_filter_options->expression_parameters.size);
    for (size_t i = 0; i < expression_parameters_count2_update; ++i) {
      EXPECT_STREQ(
        content_filter_options->expression_parameters.data[i],
        expression_parameters2_update[i]);
    }
  }

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_subscription_content_filter_options_fini(
      &*subscription_, &subscription_content_filter_options)
  );
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_subscription_content_filter_options_fini(
      &*subscription_, &subscription_content_filter_options2)
  );
}
