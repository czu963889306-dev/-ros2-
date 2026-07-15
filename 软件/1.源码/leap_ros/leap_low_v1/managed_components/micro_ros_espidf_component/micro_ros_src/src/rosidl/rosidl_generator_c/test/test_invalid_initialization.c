// Copyright 2017 Open Source Robotics Foundation, Inc.
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


#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <stdint.h>
#include <string.h>

#include "rosidl_runtime_c/string_functions.h"
#include "rosidl_generator_c/msg/strings.h"

#define EXPECT_EQ(arg1, arg2) if ((arg1) != (arg2)) return 1
#define EXPECT_NE(arg1, arg2) if ((arg1) == (arg2)) return 1

int main(void);
int test_init_null(void);
int test_partial_fini_cleanup(void);

int main(void)
{
  int rc = 0;

  printf("Testing rosidl_generator_c invalid initialization...\n");
  printf("Testing call to init with NULL...\n");
  if (test_init_null()) {
    fprintf(stderr, "test_init_null() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c destroy called on partial message...\n");
  if (test_partial_fini_cleanup()) {
    fprintf(stderr, "test_partial_fini_cleanup() FAILED\n");
    rc++;
  }
  return rc;
}

/**
 * Test call to init with null fails
 */
int test_init_null(void)
{
  rosidl_generator_c__msg__Strings * msg = NULL;
  EXPECT_EQ(false, rosidl_generator_c__msg__Strings__init(msg));
  return 0;
}

/**
 * Test cleanup on a partially fini'd msg
 */
int test_partial_fini_cleanup(void)
{
  /* create a message, properly initialized. */
  rosidl_generator_c__msg__Strings * msg = rosidl_generator_c__msg__Strings__create();
  EXPECT_NE(msg, NULL);
  /* call fini on a few fields */
  rosidl_runtime_c__String__fini(&msg->string_value);
  rosidl_runtime_c__String__fini(&msg->string_value_default1);
  /* destroy the message */
  rosidl_generator_c__msg__Strings__destroy(msg);
  return 0;
}
