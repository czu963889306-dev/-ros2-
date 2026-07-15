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

#ifndef RCL__INIT_OPTIONS_IMPL_H_
#define RCL__INIT_OPTIONS_IMPL_H_

#include "rcl/init_options.h"

#include "rmw/init_options.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// \internal
struct rcl_init_options_impl_s
{
  rcl_allocator_t allocator;
  rmw_init_options_t rmw_init_options;
};

#ifdef __cplusplus
}
#endif

#endif  // RCL__INIT_OPTIONS_IMPL_H_
