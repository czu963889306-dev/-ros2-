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

#ifdef __cplusplus
extern "C"
{
#endif

#include <errno.h>
#include <string.h>

#include "rcutils/strcasecmp.h"

int
rcutils_strcasecmp(
  const char * s1,
  const char * s2,
  int * value)
{
  if (s1 == NULL || s2 == NULL || value == NULL) {
    return -1;
  }
#ifndef _WIN32
  *value = strcasecmp(s1, s2);
#else
  *value = _stricmp(s1, s2);
#endif
  return 0;
}

int
rcutils_strncasecmp(
  const char * s1,
  const char * s2,
  size_t n,
  int * value)
{
  if (s1 == NULL || s2 == NULL || value == NULL) {
    return -1;
  }
#ifndef _WIN32
  *value = strncasecmp(s1, s2, n);
#else
  *value = _strnicmp(s1, s2, n);
#endif
  return 0;
}

#ifdef __cplusplus
}
#endif
