// Copyright 2020 Robert Bosch GmbH. All Rights Reserved.
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

#include "rmw/types.h"

RMW_PUBLIC
RMW_WARN_UNUSED
rmw_message_info_t
rmw_get_zero_initialized_message_info(void)
{
  rmw_message_info_t zero_initialized_message_info = {0};
  return zero_initialized_message_info;
}
