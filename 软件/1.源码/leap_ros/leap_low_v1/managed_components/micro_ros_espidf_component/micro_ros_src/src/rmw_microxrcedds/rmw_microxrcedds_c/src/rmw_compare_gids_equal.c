// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <rmw/rmw.h>
#include <rmw/error_handling.h>

#include <rmw_microxrcedds_c/rmw_c_macros.h>
#include "./rmw_microros_internal/error_handling_internal.h"

rmw_ret_t
rmw_compare_gids_equal(
  const rmw_gid_t * gid1,
  const rmw_gid_t * gid2,
  bool * result)
{
  // Check
  RMW_CHECK_ARGUMENT_FOR_NULL(gid1, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_ARGUMENT_FOR_NULL(gid2, RMW_RET_INVALID_ARGUMENT);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    gid1->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);
  RMW_CHECK_TYPE_IDENTIFIERS_MATCH(
    gid2->implementation_identifier,
    RMW_RET_INCORRECT_RMW_IMPLEMENTATION);

  *result = memcmp(gid1->data, gid2->data, sizeof(rmw_gid_t)) == 0;

  return RMW_RET_OK;
}
