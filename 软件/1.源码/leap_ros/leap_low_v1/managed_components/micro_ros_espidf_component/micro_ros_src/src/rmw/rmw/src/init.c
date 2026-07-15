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

#include <stddef.h>

#include "rmw/domain_id.h"
#include "rmw/init.h"
#include "rmw/init_options.h"

#ifdef __cplusplus
extern "C"
{
#endif

rmw_context_t
rmw_get_zero_initialized_context(void)
{
  return (const rmw_context_t) {
           .instance_id = 0,
           .implementation_identifier = NULL,
           .options = rmw_get_zero_initialized_init_options(),
           .actual_domain_id = 0u,
           .impl = NULL
  };  // NOLINT(readability/braces): false positive
}

#ifdef __cplusplus
}
#endif
