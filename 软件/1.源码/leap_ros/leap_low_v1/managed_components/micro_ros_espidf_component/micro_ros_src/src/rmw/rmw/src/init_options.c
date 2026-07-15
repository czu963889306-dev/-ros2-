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

#include "rmw/init_options.h"
#include "rmw/localhost.h"

#ifdef __cplusplus
extern "C"
{
#endif

rmw_init_options_t
rmw_get_zero_initialized_init_options(void)
{
  return (const rmw_init_options_t) {
           .domain_id = RMW_DEFAULT_DOMAIN_ID,
           .localhost_only = RMW_LOCALHOST_ONLY_DEFAULT,
           .implementation_identifier = NULL,
           .impl = NULL,
           .instance_id = 0,
           .enclave = NULL,
           .security_options = rmw_get_default_security_options(),
  };  // NOLINT(readability/braces): false positive
}

#ifdef __cplusplus
}
#endif
