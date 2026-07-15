// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include "./separate_compilation.h"  // NOLINT

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "rosidl_generator_c/msg/detail/defaults__struct.h"
#include "rosidl_generator_c/msg/detail/defaults__functions.h"

int func()
{
  rosidl_generator_c__msg__Defaults * msg = rosidl_generator_c__msg__Defaults__create();
  if (!msg) {
    fprintf(stderr, "failed to allocate message\n");
    return 1;
  }

  if (msg->bool_value != true) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->byte_value != 50) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->float32_value != 1.125) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->float64_value != 1.125) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->int8_value != -50) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->uint8_value != 200) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->int16_value != -1000) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->uint16_value != 2000) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->int32_value != -30000) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->uint32_value != 60000) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->int64_value != -40000000) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  if (msg->uint64_value != 50000000) {
    fprintf(stderr, "wrong default value!\n");
    return 1;
  }

  rosidl_generator_c__msg__Defaults__destroy(msg);

  return 0;
}
