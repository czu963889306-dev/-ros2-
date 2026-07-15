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

#include <stddef.h>

#include "rmw/error_handling.h"
#include "rmw/event.h"

#ifdef __cplusplus
extern "C" {
#endif

rmw_event_t
rmw_get_zero_initialized_event(void)
{
  const rmw_event_t event = {
    .implementation_identifier = NULL,
    .data = NULL,
    .event_type = RMW_EVENT_INVALID
  };  // NOLINT(readability/braces): false positive
  return event;
}

rmw_ret_t
rmw_event_fini(rmw_event_t * rmw_event)
{
  RMW_CHECK_ARGUMENT_FOR_NULL(rmw_event, RMW_RET_INVALID_ARGUMENT);
  *rmw_event = rmw_get_zero_initialized_event();
  return RMW_RET_OK;
}

#ifdef __cplusplus
}
#endif
