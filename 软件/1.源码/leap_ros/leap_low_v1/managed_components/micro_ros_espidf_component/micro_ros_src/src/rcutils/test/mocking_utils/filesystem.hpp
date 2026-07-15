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

#ifndef MOCKING_UTILS__FILESYSTEM_HPP_
#define MOCKING_UTILS__FILESYSTEM_HPP_

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#else
#include <windows.h>
#endif

#ifdef _GNU_SOURCE
#include <features.h>
#if __GLIBC__ <= 2 && __GLIBC_MINOR__ < 33
#define _GLIBC_LESS_2_33_
#endif
#endif

#include <map>
#include <string>
#include <type_traits>

#include "rcutils/macros.h"

#include "patch.hpp"

namespace mocking_utils
{
namespace filesystem
{

/// Platform-independent set of file type constants.
namespace file_types
{
constexpr auto REGULAR_FILE = S_IFREG;
constexpr auto DIRECTORY = S_IFDIR;
}  // namespace file_types

/// Platform-independent set of file permission constants.
namespace permissions
{
#ifndef _WIN32
constexpr auto USER_READABLE = S_IRUSR;
constexpr auto USER_WRITABLE = S_IWUSR;
#else
constexpr auto USER_READABLE = _S_IREAD;
constexpr auto USER_WRITABLE = _S_IWRITE;
#endif
}  // namespace permissions

// Deal with binary API quirks in 64 bit MacOS.
#if defined(__MACH__) && defined(_DARWIN_FEATURE_64_BIT_INODE)
#define MOCKING_UTILS_FILESYSTEM_PATCH_TARGET(scope, function) \
  (std::string(RCUTILS_STRINGIFY(function) "$INODE64") + "@" + (scope))
#else
#define MOCKING_UTILS_FILESYSTEM_PATCH_TARGET MOCKING_UTILS_PATCH_TARGET
#endif

#if !defined(_WIN32)

/// Helper class for patching the filesystem API.
/**
 * \tparam ID Numerical identifier for this patches. Ought to be unique.
 */
template<size_t ID>
class FileSystem
{
public:
  /// Construct mocked filesystem.
  /**
   * \param[in] scope Scope target string, using Mimick syntax.
   *   \see mocking_utils::Patch documentation for further reference.
   */
  explicit FileSystem(const std::string & scope)
  : opendir_mock_(
      MOCKING_UTILS_FILESYSTEM_PATCH_TARGET(scope, opendir),
      MOCKING_UTILS_PATCH_PROXY(opendir)),
#if defined(_GNU_SOURCE) && defined(_GLIBC_LESS_2_33_)
// Deal with binary API less than 2.33 quirks in GNU Linux.
    __xstat_mock_(
      MOCKING_UTILS_FILESYSTEM_PATCH_TARGET(scope, __xstat),
      MOCKING_UTILS_PATCH_PROXY(__xstat))
  {
    __xstat_mock_.then_call(
      std::bind(
        &FileSystem::do___xstat, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3));
#else
    stat_mock_(
      MOCKING_UTILS_FILESYSTEM_PATCH_TARGET(scope, stat),
      MOCKING_UTILS_PATCH_PROXY(stat))
  {
    stat_mock_.then_call(
      std::bind(
        &FileSystem::do_stat, this,
        std::placeholders::_1, std::placeholders::_2));
#endif
    opendir_mock_.then_call(std::bind(&FileSystem::do_opendir, this, std::placeholders::_1));
  }

  /// Force APIs that return file descriptors or handles to fail as if these had been exhausted.
  void exhaust_file_descriptors()
  {
    forced_errno_ = EMFILE;
  }

  /// Get information from file in the mocked filesystem.
  /**
   * \param[in] path Path to the file whose information is to be retrieved.
   *   If file is not found, one will be added.
   * \return mutable reference to file information.
   */
  struct stat & file_info(const std::string & path)
  {
    return files_info_[path];
  }

private:
  DIR * do_opendir(const char *)
  {
    if (forced_errno_ != 0) {
      errno = forced_errno_;
      return NULL;
    }
    errno = ENOENT;
    return NULL;
  }
  MOCKING_UTILS_PATCH_TYPE(ID, opendir) opendir_mock_;

#if defined(_GNU_SOURCE) && defined(_GLIBC_LESS_2_33_)
  int do___xstat(int, const char * path, struct stat * info)
  {
#else
  int do_stat(const char * path, struct stat * info)
  {
#endif
    if (files_info_.count(path) == 0) {
      errno = ENOENT;
      return -1;
    }
    *info = files_info_[path];
    return 0;
  }

#if defined(_GNU_SOURCE) && defined(_GLIBC_LESS_2_33_)
  MOCKING_UTILS_PATCH_TYPE(ID, __xstat) __xstat_mock_;
#else
  MOCKING_UTILS_PATCH_TYPE(ID, stat) stat_mock_;
#endif

  int forced_errno_{0};
  std::map<std::string, struct stat> files_info_;
};

#else  // !defined(_WIN32)

/// Helper class for patching the filesystem API.
/**
 * \tparam ID Numerical identifier for this patches. Ought to be unique.
 */
template<size_t ID>
class FileSystem
{
public:
  /// Construct mocked filesystem.
  /**
   * \param[in] scope Scope target string, using Mimick syntax.
   *   \see mocking_utils::Patch documentation for further reference.
   */
  explicit FileSystem(const std::string & scope)
  : find_first_file_mock_(MOCKING_UTILS_FILESYSTEM_PATCH_TARGET(scope, FindFirstFileA),
      MOCKING_UTILS_PATCH_PROXY(FindFirstFileA)),
    _stat_mock_(MOCKING_UTILS_FILESYSTEM_PATCH_TARGET(scope, _stat),
      MOCKING_UTILS_PATCH_PROXY(_stat))
  {
    find_first_file_mock_.then_call(
      std::bind(
        &FileSystem::do_FindFirstFileA, this,
        std::placeholders::_1, std::placeholders::_2));
    _stat_mock_.then_call(
      std::bind(
        &FileSystem::do__stat, this,
        std::placeholders::_1, std::placeholders::_2));
  }

  /// Force APIs that return file descriptors or handles to fail as if these had been exhausted.
  void exhaust_file_descriptors()
  {
    forced_errno_ = ERROR_NO_MORE_SEARCH_HANDLES;
  }

  /// Get information from file in the mocked filesystem.
  /**
   * \param[in] path Path to the file whose information is to be retrieved.
   *   If file is not found, one will be added.
   * \return mutable reference to file information.
   */
  struct _stat & file_info(const std::string & path)
  {
    return files_info_[path];
  }

private:
  HANDLE do_FindFirstFileA(LPCSTR, LPWIN32_FIND_DATAA)
  {
    if (forced_errno_ != 0) {
      SetLastError(forced_errno_);
      return INVALID_HANDLE_VALUE;
    }
    SetLastError(ERROR_FILE_NOT_FOUND);
    return INVALID_HANDLE_VALUE;
  }

  MOCKING_UTILS_PATCH_TYPE(ID, FindFirstFileA) find_first_file_mock_;

  int do__stat(const char * path, struct _stat * info)
  {
    if (files_info_.count(path) == 0) {
      errno = ENOENT;
      return -1;
    }
    *info = files_info_[path];
    return 0;
  }

  MOCKING_UTILS_PATCH_TYPE(ID, _stat) _stat_mock_;

  int forced_errno_{0};
  std::map<std::string, struct _stat> files_info_;
};

#endif  // else !defined(_WIN32)

}  // namespace filesystem

/// Patch filesystem API in a given `scope`.
#define patch_filesystem(scope) filesystem::FileSystem<__COUNTER__>(scope)

}  // namespace mocking_utils

#endif  // MOCKING_UTILS__FILESYSTEM_HPP_
