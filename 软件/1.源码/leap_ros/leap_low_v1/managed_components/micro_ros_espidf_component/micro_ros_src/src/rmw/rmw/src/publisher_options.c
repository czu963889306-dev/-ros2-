// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include "rmw/publisher_options.h"

#ifdef __cplusplus
extern "C"
{
#endif

rmw_publisher_options_t
rmw_get_default_publisher_options(void)
{
  rmw_publisher_options_t publisher_options = {
    .rmw_specific_publisher_payload = NULL,
    .require_unique_network_flow_endpoints = RMW_UNIQUE_NETWORK_FLOW_ENDPOINTS_NOT_REQUIRED,
  };
  return publisher_options;
}

#ifdef __cplusplus
}
#endif
