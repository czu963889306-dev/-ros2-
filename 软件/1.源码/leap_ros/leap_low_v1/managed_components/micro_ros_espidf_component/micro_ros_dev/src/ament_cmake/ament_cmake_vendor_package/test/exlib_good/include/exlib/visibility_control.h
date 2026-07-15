// Copyright 2022 Open Source Robotics Foundation, Inc.
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

#ifndef EXLIB__VISIBILITY_CONTROL_H_
#define EXLIB__VISIBILITY_CONTROL_H_

#ifdef __cplusplus
extern "C"
{
#endif

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define EXLIB_EXPORT __attribute__ ((dllexport))
    #define EXLIB_IMPORT __attribute__ ((dllimport))
  #else
    #define EXLIB_EXPORT __declspec(dllexport)
    #define EXLIB_IMPORT __declspec(dllimport)
  #endif
  #ifdef EXLIB_BUILDING_LIBRARY
    #define EXLIB_PUBLIC EXLIB_EXPORT
  #else
    #define EXLIB_PUBLIC EXLIB_IMPORT
  #endif
  #define EXLIB_PUBLIC_TYPE EXLIB_PUBLIC
  #define EXLIB_LOCAL
#else
  #define EXLIB_EXPORT __attribute__ ((visibility("default")))
  #define EXLIB_IMPORT
  #if __GNUC__ >= 4
    #define EXLIB_PUBLIC __attribute__ ((visibility("default")))
    #define EXLIB_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define EXLIB_PUBLIC
    #define EXLIB_LOCAL
  #endif
  #define EXLIB_PUBLIC_TYPE
#endif

#ifdef __cplusplus
}
#endif

#endif  // EXLIB__VISIBILITY_CONTROL_H_
