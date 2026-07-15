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

#ifdef __cplusplus
extern "C"
{
#endif
#include "rcutils/filesystem.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef RCUTILS_NO_FILESYSTEM
#include <sys/stat.h>
#endif
#ifndef _WIN32
#ifndef RCUTILS_NO_FILESYSTEM
#include <dirent.h>
#endif
#include <unistd.h>
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
#include <direct.h>
#endif  // _WIN32

#include "rcutils/env.h"
#include "rcutils/error_handling.h"
#include "rcutils/format_string.h"
#include "rcutils/repl_str.h"
#include "rcutils/strdup.h"

#ifdef _WIN32
# define RCUTILS_PATH_DELIMITER "\\"
#else
# define RCUTILS_PATH_DELIMITER "/"
#endif  // _WIN32

#ifdef RCUTILS_NO_FILESYSTEM
typedef int DIR;
#endif  // _RCUTILS_NO_FILESYSTEM
typedef struct rcutils_dir_iter_state_t
{
#ifdef _WIN32
  HANDLE handle;
  WIN32_FIND_DATA data;
#else
  DIR * dir;
#endif
} rcutils_dir_iter_state_t;

bool
rcutils_get_cwd(char * buffer, size_t max_length)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) buffer;
  (void) max_length;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  if (NULL == buffer || max_length == 0) {
    return false;
  }
#ifdef _WIN32
  if (NULL == _getcwd(buffer, (int)max_length)) {
    return false;
  }
#else
  if (NULL == getcwd(buffer, max_length)) {
    return false;
  }
#endif  // _WIN32
  return true;
#endif  // _RCUTILS_NO_FILESYSTEM
}

bool
rcutils_is_directory(const char * abs_path)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) abs_path;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  struct stat buf;
  if (stat(abs_path, &buf) < 0) {
    return false;
  }
#ifdef _WIN32
  return (buf.st_mode & S_IFDIR) == S_IFDIR;
#else
  return S_ISDIR(buf.st_mode);
#endif  // _WIN32
#endif  // _RCUTILS_NO_FILESYSTEM
}

bool
rcutils_is_file(const char * abs_path)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) abs_path;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  struct stat buf;
  if (stat(abs_path, &buf) < 0) {
    return false;
  }
#ifdef _WIN32
  return (buf.st_mode & S_IFREG) == S_IFREG;
#else
  return S_ISREG(buf.st_mode);
#endif  // _WIN32
#endif  // _RCUTILS_NO_FILESYSTEM
}

bool
rcutils_exists(const char * abs_path)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) abs_path;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  struct stat buf;
  if (stat(abs_path, &buf) < 0) {
    return false;
  }
  return true;
#endif  // _RCUTILS_NO_FILESYSTEM
}

bool
rcutils_is_readable(const char * abs_path)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) abs_path;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  struct stat buf;
  if (stat(abs_path, &buf) < 0) {
    return false;
  }
#ifdef _WIN32
  if (!(buf.st_mode & _S_IREAD)) {
#else
  if (!(buf.st_mode & S_IRUSR)) {
#endif  // _WIN32
    return false;
  }
  return true;
#endif  // _RCUTILS_NO_FILESYSTEM
}

bool
rcutils_is_writable(const char * abs_path)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) abs_path;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  struct stat buf;
  if (stat(abs_path, &buf) < 0) {
    return false;
  }
#ifdef _WIN32
  if (!(buf.st_mode & _S_IWRITE)) {
#else
  if (!(buf.st_mode & S_IWUSR)) {
#endif  // _WIN32
    return false;
  }
  return true;
#endif  // _RCUTILS_NO_FILESYSTEM
}

bool
rcutils_is_readable_and_writable(const char * abs_path)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) abs_path;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  struct stat buf;
  if (stat(abs_path, &buf) < 0) {
    return false;
  }
#ifdef _WIN32
  // NOTE(marguedas) on windows all writable files are readable
  // hence the following check is equivalent to "& _S_IWRITE"
  if (!((buf.st_mode & _S_IWRITE) && (buf.st_mode & _S_IREAD))) {
#else
  if (!((buf.st_mode & S_IWUSR) && (buf.st_mode & S_IRUSR))) {
#endif  // _WIN32
    return false;
  }
  return true;
#endif  // _RCUTILS_NO_FILESYSTEM
}

char *
rcutils_join_path(
  const char * left_hand_path,
  const char * right_hand_path,
  rcutils_allocator_t allocator)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) left_hand_path;
  (void) right_hand_path;
  (void) allocator;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return NULL;
#else
  if (NULL == left_hand_path) {
    return NULL;
  }
  if (NULL == right_hand_path) {
    return NULL;
  }

  return rcutils_format_string(
    allocator,
    "%s%s%s",
    left_hand_path, RCUTILS_PATH_DELIMITER, right_hand_path);
#endif  // _RCUTILS_NO_FILESYSTEM
}

char *
rcutils_to_native_path(
  const char * path,
  rcutils_allocator_t allocator)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) path;
  (void) allocator;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return NULL;
#else
  if (NULL == path) {
    return NULL;
  }

  return rcutils_repl_str(path, "/", RCUTILS_PATH_DELIMITER, &allocator);
#endif  // _RCUTILS_NO_FILESYSTEM
}

char *
rcutils_expand_user(const char * path, rcutils_allocator_t allocator)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) path;
  (void) allocator;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return NULL;
#else
  if (NULL == path) {
    return NULL;
  }

  if ('~' != path[0]) {
    return rcutils_strdup(path, allocator);
  }

  const char * homedir = rcutils_get_home_dir();
  if (NULL == homedir) {
    return NULL;
  }
  return rcutils_format_string_limit(
    allocator,
    strlen(homedir) + strlen(path),
    "%s%s",
    homedir,
    path + 1);
#endif  // _RCUTILS_NO_FILESYSTEM
}

bool
rcutils_mkdir(const char * abs_path)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) abs_path;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  if (NULL == abs_path) {
    return false;
  }

  if (abs_path[0] == '\0') {
    return false;
  }

  bool success = false;
#ifdef _WIN32
  // TODO(clalancette): Check to ensure that the path is absolute on Windows.
  // In theory we can use PathRelativeA to do this, but I was unable to make
  // it work.  Needs further investigation.

  int ret = _mkdir(abs_path);
#else
  if (abs_path[0] != '/') {
    return false;
  }

  int ret = mkdir(abs_path, 0775);
#endif
  if (ret == 0 || (errno == EEXIST && rcutils_is_directory(abs_path))) {
    success = true;
  }

  return success;
#endif  // _RCUTILS_NO_FILESYSTEM
}

rcutils_ret_t
rcutils_calculate_directory_size(
  const char * directory_path,
  uint64_t * size,
  rcutils_allocator_t allocator)
{
  return rcutils_calculate_directory_size_with_recursion(directory_path, 1, size, allocator);
}

typedef struct dir_list_t
{
  char * path;
  uint32_t depth;  // Compare with base path
  struct dir_list_t * next;
} dir_list_t;

#ifndef RCUTILS_NO_FILESYSTEM
static void free_dir_list(dir_list_t * dir_list, rcutils_allocator_t allocator)
{
  dir_list_t * next_dir;
  do {
    next_dir = dir_list->next;
    allocator.deallocate(dir_list->path, allocator.state);
    allocator.deallocate(dir_list, allocator.state);
    dir_list = next_dir;
  } while (dir_list);
}

static void remove_first_dir_from_list(dir_list_t ** dir_list, rcutils_allocator_t allocator)
{
  dir_list_t * next_dir = (*dir_list)->next;
  allocator.deallocate((*dir_list)->path, allocator.state);
  allocator.deallocate(*dir_list, allocator.state);
  *dir_list = next_dir;
}

static rcutils_ret_t check_and_calculate_size(
  const char * filename,
  uint64_t * dir_size,
  const size_t max_depth,
  dir_list_t * dir_list,
  rcutils_allocator_t allocator)
{
  // Skip over local folder handle (`.`) and parent folder (`..`)
  if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
    return RCUTILS_RET_OK;
  }

  char * file_path = rcutils_join_path(dir_list->path, filename, allocator);
  if (NULL == file_path) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("rcutils_join_path return NULL !\n");
    return RCUTILS_RET_BAD_ALLOC;
  }

  if (rcutils_is_directory(file_path)) {
    if ((max_depth == 0) || ((dir_list->depth + 1) <= max_depth)) {
      // Add new directory to dir_list
      dir_list_t * found_new_dir =
        allocator.allocate(sizeof(dir_list_t), allocator.state);
      if (NULL == found_new_dir) {
        RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(
          "Failed to allocate memory for path %s !\n", file_path);
        allocator.deallocate(file_path, allocator.state);
        return RCUTILS_RET_BAD_ALLOC;
      }
      found_new_dir->path = file_path;
      found_new_dir->depth = dir_list->depth + 1;
      found_new_dir->next = dir_list->next;
      dir_list->next = found_new_dir;
      return RCUTILS_RET_OK;
    }
  } else {
    *dir_size += rcutils_get_file_size(file_path);
  }

  allocator.deallocate(file_path, allocator.state);

  return RCUTILS_RET_OK;
}
#endif  // _RCUTILS_NO_FILESYSTEM

rcutils_ret_t
rcutils_calculate_directory_size_with_recursion(
  const char * directory_path,
  const size_t max_depth,
  uint64_t * size,
  rcutils_allocator_t allocator)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) directory_path;
  (void) max_depth;
  (void) size;
  (void) allocator;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return RCUTILS_RET_ERROR;
#else
  dir_list_t * dir_list = NULL;
  rcutils_ret_t ret = RCUTILS_RET_OK;
  rcutils_dir_iter_t * iter = NULL;

  if (NULL == directory_path) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("directory_path is NULL !");
    return RCUTILS_RET_INVALID_ARGUMENT;
  }

  if (NULL == size) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("size pointer is NULL !");
    return RCUTILS_RET_INVALID_ARGUMENT;
  }

  size_t dir_size = 0;

  if (!rcutils_is_directory(directory_path)) {
    RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(
      "Path is not a directory: %s\n", directory_path);
    return RCUTILS_RET_ERROR;
  }

  dir_list = allocator.zero_allocate(1, sizeof(dir_list_t), allocator.state);
  if (NULL == dir_list) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to allocate memory !\n");
    return RCUTILS_RET_BAD_ALLOC;
  }

  dir_list->depth = 1;

  dir_list->path = rcutils_strdup(directory_path, allocator);
  if (NULL == dir_list->path) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to duplicate directory path !\n");
    allocator.deallocate(dir_list, allocator.state);
    return RCUTILS_RET_BAD_ALLOC;
  }

  *size = 0;

  do {
    iter = rcutils_dir_iter_start(dir_list->path, allocator);
    if (NULL == iter) {
      ret = RCUTILS_RET_ERROR;
      goto fail;
    }

    do {
      ret = check_and_calculate_size(iter->entry_name, size, max_depth, dir_list, allocator);
      if (RCUTILS_RET_OK != ret) {
        goto fail;
      }
    } while (rcutils_dir_iter_next(iter));

    rcutils_dir_iter_end(iter);

    remove_first_dir_from_list(&dir_list, allocator);
  } while (dir_list);

  return ret;

fail:
  rcutils_dir_iter_end(iter);
  free_dir_list(dir_list, allocator);
  return ret;
#endif  // _RCUTILS_NO_FILESYSTEM
}

rcutils_dir_iter_t *
rcutils_dir_iter_start(const char * directory_path, const rcutils_allocator_t allocator)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) directory_path;
  (void) allocator;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return NULL;
#else
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(directory_path, NULL);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &allocator, "allocator is invalid", return NULL);

  rcutils_dir_iter_t * iter = (rcutils_dir_iter_t *)allocator.zero_allocate(
    1, sizeof(rcutils_dir_iter_t), allocator.state);
  if (NULL == iter) {
    return NULL;
  }
  iter->allocator = allocator;

  rcutils_dir_iter_state_t * state = (rcutils_dir_iter_state_t *)allocator.zero_allocate(
    1, sizeof(rcutils_dir_iter_state_t), allocator.state);
  if (NULL == state) {
    RCUTILS_SET_ERROR_MSG(
      "Failed to allocate memory.\n");
    goto rcutils_dir_iter_start_fail;
  }
  iter->state = (void *)state;

#ifdef _WIN32
  char * search_path = rcutils_join_path(directory_path, "*", allocator);
  if (NULL == search_path) {
    goto rcutils_dir_iter_start_fail;
  }
  state->handle = FindFirstFile(search_path, &state->data);
  allocator.deallocate(search_path, allocator.state);
  if (INVALID_HANDLE_VALUE == state->handle) {
    DWORD error = GetLastError();
    if (ERROR_FILE_NOT_FOUND != error || !rcutils_is_directory(directory_path)) {
      RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Can't open directory %s. Error code: %d\n", directory_path, error);
      goto rcutils_dir_iter_start_fail;
    }
  } else {
    iter->entry_name = state->data.cFileName;
  }
#else
  state->dir = opendir(directory_path);
  if (NULL == state->dir) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Can't open directory %s. Error code: %d\n", directory_path, errno);
    goto rcutils_dir_iter_start_fail;
  }

  errno = 0;
  struct dirent * entry = readdir(state->dir);
  if (NULL != entry) {
    iter->entry_name = entry->d_name;
  } else if (0 != errno) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Can't iterate directory %s. Error code: %d\n", directory_path, errno);
    goto rcutils_dir_iter_start_fail;
  }
#endif

  return iter;

rcutils_dir_iter_start_fail:
  rcutils_dir_iter_end(iter);
  return NULL;
#endif  // _RCUTILS_NO_FILESYSTEM
}

bool
rcutils_dir_iter_next(rcutils_dir_iter_t * iter)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) iter;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return false;
#else
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(iter, false);
  rcutils_dir_iter_state_t * state = (rcutils_dir_iter_state_t *)iter->state;
  RCUTILS_CHECK_FOR_NULL_WITH_MSG(state, "iter is invalid", false);

#ifdef _WIN32
  if (FindNextFile(state->handle, &state->data)) {
    iter->entry_name = state->data.cFileName;
    return true;
  }
  FindClose(state->handle);
#else
  struct dirent * entry = readdir(state->dir);
  if (NULL != entry) {
    iter->entry_name = entry->d_name;
    return true;
  }
#endif

  iter->entry_name = NULL;
  return false;
#endif  // _RCUTILS_NO_FILESYSTEM
}

void
rcutils_dir_iter_end(rcutils_dir_iter_t * iter)
{
#ifdef RCUTILS_NO_FILESYSTEM
  (void) iter;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
#else
  if (NULL == iter) {
    return;
  }

  rcutils_allocator_t allocator = iter->allocator;
  rcutils_dir_iter_state_t * state = (rcutils_dir_iter_state_t *)iter->state;
  if (NULL != state) {
#ifdef _WIN32
    FindClose(state->handle);
#else
    if (NULL != state->dir) {
      closedir(state->dir);
    }
#endif

    allocator.deallocate(state, allocator.state);
  }

  allocator.deallocate(iter, allocator.state);
#endif  // _RCUTILS_NO_FILESYSTEM
}

size_t
rcutils_get_file_size(const char * file_path)
{

#ifdef RCUTILS_NO_FILESYSTEM
  (void) file_path;
  RCUTILS_SET_ERROR_MSG("not available filesystem");
  return 0;
#else
  if (!rcutils_is_file(file_path)) {
    RCUTILS_SAFE_FWRITE_TO_STDERR_WITH_FORMAT_STRING(
      "Path is not a file: %s\n", file_path);
    return 0;
  }

  struct stat stat_buffer;
  int rc = stat(file_path, &stat_buffer);
  return rc == 0 ? (size_t)(stat_buffer.st_size) : 0;
#endif  // _RCUTILS_NO_FILESYSTEM
}

#ifdef __cplusplus
}
#endif
