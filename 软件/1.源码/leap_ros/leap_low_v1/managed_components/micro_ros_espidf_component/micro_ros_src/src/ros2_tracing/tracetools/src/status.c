// Copyright 2019 Robert Bosch GmbH
// Copyright 2020 Christophe Bedard
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

#include <stdbool.h>
#include <stdio.h>

#include "tracetools/config.h"
#include "tracetools/status.h"

int tracetools_status(bool trace_compile_status_enabled)
{
#ifndef TRACETOOLS_DISABLED
  printf("Tracing ");
  if (trace_compile_status_enabled) {
    printf("enabled\n");
    return 0;
  } else {
    printf("disabled\n");
    return 1;
  }
#else
  (void)trace_compile_status_enabled;
  printf("Tracing disabled through configuration\n");
  return 1;
#endif
}
