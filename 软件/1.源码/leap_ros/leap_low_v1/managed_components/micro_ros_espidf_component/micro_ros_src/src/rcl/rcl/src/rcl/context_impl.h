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

#ifndef RCL__CONTEXT_IMPL_H_
#define RCL__CONTEXT_IMPL_H_

#include "rcl/context.h"
#include "rcl/error_handling.h"

#include "./init_options_impl.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// \internal
struct rcl_context_impl_s
{
  /// Allocator used during init and shutdown.
  rcl_allocator_t allocator;
  /// Copy of init options given during init.
  rcl_init_options_t init_options;
  /// Length of argv (may be `0`).
  int64_t argc;
  /// Copy of argv used during init (may be `NULL`).
  char ** argv;
  /// rmw context.
  rmw_context_t rmw_context;
};

RCL_LOCAL
rcl_ret_t
__cleanup_context(rcl_context_t * context);

#ifdef __cplusplus
}
#endif

#endif  // RCL__CONTEXT_IMPL_H_
