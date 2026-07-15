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
#include <stdio.h>
#include <stdlib.h>

#include "rcutils/configuration_flags.h"

#ifndef RCUTILS_NO_FILESYSTEM

#ifndef _WIN32
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#elif defined (_GNU_SOURCE) && !defined (__QNXNTO__)
#include <link.h>
#elif defined (__QNXNTO__)
#include <sys/link.h>
#endif
#include <dlfcn.h>
#else
// When building with MSVC 19.28.29333.0 on Windows 10 (as of 2020-11-11),
// there appears to be a problem with winbase.h (which is included by
// Windows.h).  In particular, warnings of the form:
//
// warning C5105: macro expansion producing 'defined' has undefined behavior
//
// See https://developercommunity.visualstudio.com/content/problem/695656/wdk-and-sdk-are-not-compatible-with-experimentalpr.html
// for more information.  For now disable that warning when including windows.h
#pragma warning(push)
#pragma warning(disable : 5105)
#include <windows.h>
#pragma warning(pop)
C_ASSERT(sizeof(void *) == sizeof(HINSTANCE));
#ifdef UNICODE
#error "rcutils does not support Unicode paths"
#endif
C_ASSERT(sizeof(char) == sizeof(TCHAR));
#endif  // _WIN32

#endif //RCUTILS_NO_FILESYSTEM

#include "rcutils/error_handling.h"
#include "rcutils/macros.h"
#include "rcutils/shared_library.h"
#include "rcutils/strdup.h"

rcutils_shared_library_t
rcutils_get_zero_initialized_shared_library(void)
{
  rcutils_shared_library_t zero_initialized_shared_library;
  zero_initialized_shared_library.library_path = NULL;
  zero_initialized_shared_library.lib_pointer = NULL;
  zero_initialized_shared_library.allocator = rcutils_get_zero_initialized_allocator();
  return zero_initialized_shared_library;
}

rcutils_ret_t
rcutils_load_shared_library(
  rcutils_shared_library_t * lib,
  const char * library_path,
  rcutils_allocator_t allocator)
{
#ifndef RCUTILS_NO_FILESYSTEM
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCUTILS_RET_BAD_ALLOC);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCUTILS_RET_ERROR);

  RCUTILS_CHECK_ARGUMENT_FOR_NULL(lib, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(library_path, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(&allocator, return RCUTILS_RET_INVALID_ARGUMENT);
  if (NULL != lib->lib_pointer) {
    RCUTILS_SET_ERROR_MSG("lib argument is not zero-initialized");
    return RCUTILS_RET_INVALID_ARGUMENT;
  }

  rcutils_ret_t ret = RCUTILS_RET_OK;
  lib->allocator = allocator;

  // Delegate library path resolution to dynamic linker.

  // Since the given library path might be relative, let the dynamic
  // linker search for the binary object in all standard locations and
  // current process space (if already loaded) first.
  // Then lookup its full path with the handle obtained.
  // See POSIX dlopen() API and Windows' LoadLibrary() API documentation
  // for further reference.

#ifndef _WIN32
  lib->lib_pointer = dlopen(library_path, RTLD_LAZY);
  if (NULL == lib->lib_pointer) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING("dlopen error: %s", dlerror());
    return RCUTILS_RET_ERROR;
  }

#if defined(__APPLE__)
  const char * image_name = NULL;
  uint32_t image_count = _dyld_image_count();
  for (uint32_t i = 0; NULL == image_name && i < image_count; ++i) {
    // Iterate in reverse as the library is likely near the end of the list.
    const char * candidate_name = _dyld_get_image_name(image_count - i - 1);
    if (NULL == candidate_name) {
      RCUTILS_SET_ERROR_MSG("dyld image index out of range");
      ret = RCUTILS_RET_ERROR;
      goto fail;
    }
    void * handle = dlopen(candidate_name, RTLD_LAZY | RTLD_NOLOAD);
    if (handle == lib->lib_pointer) {
      image_name = candidate_name;
    }
    if (dlclose(handle) != 0) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING("dlclose error: %s", dlerror());
      ret = RCUTILS_RET_ERROR;
      goto fail;
    }
  }
  if (NULL == image_name) {
    RCUTILS_SET_ERROR_MSG("dyld image name could not be found");
    ret = RCUTILS_RET_ERROR;
    goto fail;
  }
  lib->library_path = rcutils_strdup(image_name, lib->allocator);
#elif defined(_GNU_SOURCE) && !defined(__QNXNTO__) && !defined(__ANDROID__)
  struct link_map * map = NULL;
  if (dlinfo(lib->lib_pointer, RTLD_DI_LINKMAP, &map) != 0) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING("dlinfo error: %s", dlerror());
    ret = RCUTILS_RET_ERROR;
    goto fail;
  }
  lib->library_path = rcutils_strdup(map->l_name, lib->allocator);
#else
  lib->library_path = rcutils_strdup(library_path, lib->allocator);
#endif
  if (NULL == lib->library_path) {
    RCUTILS_SET_ERROR_MSG("unable to allocate memory");
    ret = RCUTILS_RET_BAD_ALLOC;
    goto fail;
  }

  return RCUTILS_RET_OK;
fail:
  if (dlclose(lib->lib_pointer) != 0) {
    RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(
      "dlclose error: %s\n", dlerror());
  }
  lib->lib_pointer = NULL;
  return ret;
#else
  HMODULE module = LoadLibrary(library_path);
  if (!module) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "LoadLibrary error: %lu", GetLastError());
    return RCUTILS_RET_ERROR;
  }

  for (DWORD buffer_capacity = MAX_PATH; ; buffer_capacity *= 2) {
    LPSTR buffer = lib->allocator.allocate(buffer_capacity, lib->allocator.state);
    if (NULL == buffer) {
      RCUTILS_SET_ERROR_MSG("unable to allocate memory");
      ret = RCUTILS_RET_BAD_ALLOC;
      goto fail;
    }
    DWORD buffer_size = GetModuleFileName(module, buffer, buffer_capacity);
    if (0 == buffer_size) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "GetModuleFileName error: %lu", GetLastError());
      ret = RCUTILS_RET_ERROR;
      goto fail;
    }
    if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
      lib->allocator.deallocate(buffer, lib->allocator.state);
      continue;
    }
    lib->library_path = lib->allocator.reallocate(
      buffer, buffer_size + 1, lib->allocator.state);
    if (NULL == lib->library_path) {
      lib->library_path = buffer;
    }
    break;
  }
  lib->lib_pointer = (void *)module;

  return RCUTILS_RET_OK;
fail:
  if (!FreeLibrary(module)) {
    RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(
      "FreeLibrary error: %lu\n", GetLastError());
  }
  return ret;
#endif  // _WIN32

#else
  (void) lib;
  (void) library_path;
  (void) allocator;
  return RCUTILS_RET_ERROR;
#endif //RCUTILS_NO_FILESYSTEM

}

void *
rcutils_get_symbol(const rcutils_shared_library_t * lib, const char * symbol_name)
{
#ifndef RCUTILS_NO_FILESYSTEM
  if (!lib || !lib->lib_pointer || (symbol_name == NULL)) {
    RCUTILS_SET_ERROR_MSG("invalid inputs arguments");
    return NULL;
  }

#ifndef _WIN32
  void * lib_symbol = dlsym(lib->lib_pointer, symbol_name);
  char * error = dlerror();
  if (error != NULL) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Error getting the symbol '%s'. Error '%s'",
      symbol_name, error);
    return NULL;
  }
#else
  void * lib_symbol = GetProcAddress((HINSTANCE)(lib->lib_pointer), symbol_name);
  if (lib_symbol == NULL) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Error getting the symbol '%s'. Error '%d'",
      symbol_name, GetLastError());
    return NULL;
  }
#endif  // _WIN32
  if (!lib_symbol) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "symbol '%s' does not exist in the library '%s'",
      symbol_name, lib->library_path);
    return NULL;
  }
  return lib_symbol;
#else
  (void) lib;
  (void) symbol_name;
  return NULL;
#endif //RCUTILS_NO_FILESYSTEM
}

bool
rcutils_has_symbol(const rcutils_shared_library_t * lib, const char * symbol_name)
{
#ifndef RCUTILS_NO_FILESYSTEM
  if (!lib || !lib->lib_pointer || symbol_name == NULL) {
    return false;
  }

#ifndef _WIN32
  // the correct way to test for an error is to call dlerror() to clear any old error conditions,
  // then call dlsym(), and then call dlerror() again, saving its return value into a variable,
  // and check whether this saved value is not NULL.
  dlerror(); /* Clear any existing error */
  void * lib_symbol = dlsym(lib->lib_pointer, symbol_name);
  return dlerror() == NULL && lib_symbol != 0;
#else
  void * lib_symbol = GetProcAddress((HINSTANCE)(lib->lib_pointer), symbol_name);
  return lib_symbol != NULL;
#endif  // _WIN32
#else
  (void) lib;
  (void) symbol_name;
  return false;
#endif //RCUTILS_NO_FILESYSTEM
}

rcutils_ret_t
rcutils_unload_shared_library(rcutils_shared_library_t * lib)
{
#ifndef RCUTILS_NO_FILESYSTEM
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(lib, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(lib->lib_pointer, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(lib->library_path, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR(&lib->allocator, return RCUTILS_RET_INVALID_ARGUMENT);

  rcutils_ret_t ret = RCUTILS_RET_OK;
#ifndef _WIN32
  // The function dlclose() returns 0 on success, and nonzero on error.
  int error_code = dlclose(lib->lib_pointer);
  if (error_code) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING("dlclose error: %s", dlerror());
#else
  // If the function succeeds, the return value is nonzero.
  int error_code = FreeLibrary((HINSTANCE)(lib->lib_pointer));
  if (!error_code) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING("FreeLibrary error: %lu", GetLastError());
#endif  // _WIN32
    ret = RCUTILS_RET_ERROR;
  }

  lib->allocator.deallocate(lib->library_path, lib->allocator.state);
  lib->library_path = NULL;
  lib->lib_pointer = NULL;
  lib->allocator = rcutils_get_zero_initialized_allocator();
  return ret;
#else
  (void) lib;
  return RCUTILS_RET_ERROR;
#endif //RCUTILS_NO_FILESYSTEM
}

rcutils_ret_t
rcutils_get_platform_library_name(
  const char * library_name,
  char * library_name_platform,
  unsigned int buffer_size,
  bool debug)
{
#ifndef RCUTILS_NO_FILESYSTEM
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(library_name, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(library_name_platform, RCUTILS_RET_INVALID_ARGUMENT);

  int written = 0;

#if defined(__linux__) || defined(__QNXNTO__)
  if (debug) {
    if (buffer_size >= (strlen(library_name) + 8)) {
      written = rcutils_snprintf(
        library_name_platform, strlen(library_name) + 8, "lib%sd.so", library_name);
    }
  } else {
    if (buffer_size >= (strlen(library_name) + 7)) {
      written = rcutils_snprintf(
        library_name_platform, strlen(library_name) + 7, "lib%s.so", library_name);
    }
  }
#elif __APPLE__
  if (debug) {
    if (buffer_size >= (strlen(library_name) + 11)) {
      written = rcutils_snprintf(
        library_name_platform, strlen(library_name) + 11, "lib%sd.dylib", library_name);
    }
  } else {
    if (buffer_size >= (strlen(library_name) + 10)) {
      written = rcutils_snprintf(
        library_name_platform, strlen(library_name) + 10, "lib%s.dylib", library_name);
    }
  }
#elif _WIN32
  if (debug) {
    if (buffer_size >= (strlen(library_name) + 6)) {
      written = rcutils_snprintf(
        library_name_platform, strlen(library_name) + 6, "%sd.dll", library_name);
    }
  } else {
    if (buffer_size >= (strlen(library_name) + 5)) {
      written = rcutils_snprintf(
        library_name_platform, strlen(library_name) + 5, "%s.dll", library_name);
    }
  }
#endif
  if (written <= 0) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "failed to format library name: '%s'\n",
      library_name);
    return RCUTILS_RET_ERROR;
  }
  return RCUTILS_RET_OK;
#else
  (void) library_name;
  (void) library_name_platform;
  (void) buffer_size;
  (void) debug;
  return RCUTILS_RET_ERROR;
#endif //RCUTILS_NO_FILESYSTEM
}

bool
rcutils_is_shared_library_loaded(rcutils_shared_library_t * lib)
{
  return lib->lib_pointer != NULL;
}

#ifdef __cplusplus
}
#endif
