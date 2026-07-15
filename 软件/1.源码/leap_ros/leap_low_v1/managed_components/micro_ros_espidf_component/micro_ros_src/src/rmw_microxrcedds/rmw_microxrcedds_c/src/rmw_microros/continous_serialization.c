// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <rmw_microxrcedds_c/config.h>
#include <rmw/rmw.h>
#include <rmw/allocators.h>
#include <rmw/ret_types.h>

#include "../rmw_microros_internal/types.h"

void rmw_uros_set_continous_serialization_callbacks(
  rmw_publisher_t * publisher,
  rmw_uros_continous_serialization_size size_cb,
  rmw_uros_continous_serialization serialization_cb)
{
  rmw_uxrce_publisher_t * custom_publisher = (rmw_uxrce_publisher_t *)publisher->data;

  custom_publisher->cs_cb_size = size_cb;
  custom_publisher->cs_cb_serialization = serialization_cb;
}
