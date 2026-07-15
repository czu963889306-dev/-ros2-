// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#include <iostream>

#include "rcutils/error_handling.h"
#include "rcutils/logging.h"
#include "rcutils/types/rcutils_ret.h"

int main(int, char **)
{
  rcutils_ret_t ret = rcutils_logging_initialize();
  if (ret != RCUTILS_RET_OK) {
    fprintf(stderr, "error initializing logging: %s\n", rcutils_get_error_string().str);
    return -1;
  }

  // check all attributes for a debug log message
  rcutils_log_location_t location = {"func", "file", 42u};
  char message[2048];
  message[0] = 'X';
  for (size_t i = 1; i < sizeof(message) - 2; ++i) {
    message[i] = 'x';
  }
  message[sizeof(message) - 2] = 'X';
  message[sizeof(message) - 1] = '\0';
  rcutils_log(&location, RCUTILS_LOG_SEVERITY_INFO, "name1", "%s", message);

  message[1] = '%';
  message[2] = 'd';
  rcutils_log(&location, RCUTILS_LOG_SEVERITY_INFO, "name2", message, 42);

  std::cout.flush();

  return 0;
}
