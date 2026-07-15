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

#include "rcl/rcl.h"

int main(int, char **)
{
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  if (ret != RCL_RET_OK) {
    return ret;
  }
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  if (ret != RCL_RET_OK) {
    return ret;
  }
  ret = rcl_init_options_fini(&init_options);
  if (ret != RCL_RET_OK) {
    return ret;
  }
  ret = rcl_shutdown(&context);
  if (ret != RCL_RET_OK) {
    return ret;
  }
  ret = rcl_context_fini(&context);
  if (ret != RCL_RET_OK) {
    return ret;
  }
  return 0;
}
