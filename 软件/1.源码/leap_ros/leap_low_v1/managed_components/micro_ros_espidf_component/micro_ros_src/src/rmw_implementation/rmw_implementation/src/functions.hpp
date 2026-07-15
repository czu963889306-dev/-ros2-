// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#ifndef FUNCTIONS_HPP_
#define FUNCTIONS_HPP_

#include <memory>
#include <string>

#include "rcpputils/shared_library.hpp"

#include "./visibility_control.h"

RMW_IMPLEMENTATION_DEFAULT_VISIBILITY
std::shared_ptr<rcpputils::SharedLibrary> load_library();

RMW_IMPLEMENTATION_DEFAULT_VISIBILITY
void * lookup_symbol(
  std::shared_ptr<rcpputils::SharedLibrary> lib,
  const std::string & symbol_name);

#ifdef __cplusplus
extern "C"
{
#endif

RMW_IMPLEMENTATION_DEFAULT_VISIBILITY
void prefetch_symbols(void);

#ifdef __cplusplus
}
#endif

RMW_IMPLEMENTATION_DEFAULT_VISIBILITY
void unload_library();

#endif  // FUNCTIONS_HPP_
