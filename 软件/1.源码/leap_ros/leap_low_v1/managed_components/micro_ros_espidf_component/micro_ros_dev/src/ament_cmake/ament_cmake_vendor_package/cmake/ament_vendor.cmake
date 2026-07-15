# Copyright 2022 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# Build and install an external project as a vendor package.
#
# :param TARGET_NAME: the name to give this vendor package target.
# :type TARGET_NAME: string
# :param SATISFIED: a boolean flag indicating whether there is a system
#   package which already satisfies the requirement that this vendor package
#   aims to provide.
# :type SATISFIED: option
# :param VCS_TYPE: the mechanism used to fetch the project source code
#   (i.e. git, tar, zip, svn, path). Defaults to 'git' if not specified.
# :type VCS_TYPE: string
# :param VCS_URL: the VCS-specific URL to fetch the project source code from.
# :type VCS_URL: string
# :param VCS_VERSION: the VCS-specific version to fetch the projcect source
#   code at.
# :type VCS_VERSION: string
# :param PATCHES: paths to patch files to apply to downloaded source code,
#   either absolute or relative to the current calling directory. If given a
#   directory, all patch files in the directory (non-recursive) will be applied
#   in alphabetical order.
# :type PATCHES: list of strings
# :param CMAKE_ARGS: extra arguments to pass to the CMake invocation of the
#   external project.
# :type CMAKE_ARGS: list of strings
# :param SOURCE_SUBDIR: subdirectory within the external project to be built
#   and installed. Defaults to the root directory.
# :type SOURCE_SUBDIR: string
# :param SKIP_INSTALL: when specified, do not install the external project or
#   any associated ament support files.
# :type SKIP_INSTALL: option
# :param GLOBAL_HOOK: rather than requiring consumers of the external project
#   to ``find_package`` the vendor package prior to looking for the external
#   project, expose the external project globally to any downstream CMake
#   projects.
# :type GLOBAL_HOOK: option
#
# @public
#
macro(ament_vendor TARGET_NAME)
  if(NOT PROJECT_NAME)
    message(FATAL_ERROR "ament_vendor() must be called after project()")
  endif()

  if(_${PROJECT_NAME}_AMENT_PACKAGE)
    message(FATAL_ERROR "ament_vendor() must be called before ament_package()")
  endif()

  cmake_parse_arguments(_ARG "GLOBAL_HOOK;SKIP_INSTALL" "SOURCE_SUBDIR;VCS_TYPE;VCS_URL;VCS_VERSION;SATISFIED" "CMAKE_ARGS;PATCHES" ${ARGN})
  if(_ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_vendor() called with unused arguments: "
      "${_ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT _ARG_VCS_URL)
    message(FATAL_ERROR "ament_vendor() must be called with the VCS_URL argument")
  endif()
  if(_ARG_VCS_TYPE STREQUAL "path")
    if(_ARG_SOURCE_SUBDIR)
      message(FATAL_ERROR "ament_vendor() cannot use VCS_TYPE 'path' with SOURCE_SUBDIR argument")
    endif()
    if(_ARG_PATCHES)
      message(FATAL_ERROR "ament_vendor() cannot use VCS_TYPE 'path' with PATCHES argument")
    endif()
  endif()

  if(_ARG_SKIP_INSTALL AND _ARG_GLOBAL_HOOK)
    message(FATAL_ERROR "ament_vendor() cannot use GLOBALHOOK with SKIP_INSTALL argument")
  endif()

  if(NOT _ARG_VCS_TYPE)
    set(_ARG_VCS_TYPE "git")
  endif()
  if(NOT _ARG_VCS_VERSION)
    set(_ARG_VCS_VERSION "''")
  endif()

  if(NOT _ARG_SATISFIED)
    set(_ARG_SATISFIED FALSE)
  endif()

  option(FORCE_BUILD_VENDOR_PKG
    "Build vendor packages from source, even if system-installed packages are available"
    OFF)

  if(NOT _ARG_SATISFIED OR FORCE_BUILD_VENDOR_PKG)
    if(_ARG_SATISFIED)
      message(STATUS "Forcing vendor package build for '${TARGET_NAME}', which is already satisfied")
    endif()

    list_append_unique(_AMENT_CMAKE_VENDOR_PACKAGE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-prefix/install")

    _ament_vendor(
      "${TARGET_NAME}"
      "${_ARG_VCS_TYPE}"
      "${_ARG_VCS_URL}"
      "${_ARG_VCS_VERSION}"
      "${_ARG_PATCHES}"
      "${_ARG_CMAKE_ARGS}"
      "${_ARG_SOURCE_SUBDIR}"
      "${_ARG_SKIP_INSTALL}"
    )

    if(NOT _ament_vendor_called AND NOT _ARG_SKIP_INSTALL)
      # Hooks for CMAKE_PREFIX_PATH
      if(_ARG_GLOBAL_HOOK)
        ament_environment_hooks(${ament_cmake_vendor_package_DIR}/templates/vendor_package_cmake_prefix.bat.in)
        ament_environment_hooks(${ament_cmake_vendor_package_DIR}/templates/vendor_package_cmake_prefix.dsv.in)
        ament_environment_hooks(${ament_cmake_vendor_package_DIR}/templates/vendor_package_cmake_prefix.sh.in)
      else()
        list(APPEND ${PROJECT_NAME}_CONFIG_EXTRAS ${ament_cmake_vendor_package_DIR}/templates/vendor_package_cmake_prefix.cmake.in)
      endif()

      # Hooks for PATH and the system's library path
      if(APPLE)
        set(_LIBDIR_VAR_NAME "DYLD_LIBRARY_PATH")
      elseif(WIN32)
        set(_LIBDIR_VAR_NAME "PATH")
      else()
        set(_LIBDIR_VAR_NAME "LD_LIBRARY_PATH")
      endif()
      ament_environment_hooks(${ament_cmake_vendor_package_DIR}/templates/vendor_package.bat.in)
      ament_environment_hooks(${ament_cmake_vendor_package_DIR}/templates/vendor_package.dsv.in)
      ament_environment_hooks(${ament_cmake_vendor_package_DIR}/templates/vendor_package.sh.in)

      # Resource index marker
      ament_index_register_resource("vendor_packages" CONTENT "opt/${PROJECT_NAME}")

      set(_ament_vendor_called TRUE)
    endif()
  else()
    message(STATUS "Skipping vendor package build for '${TARGET_NAME}', which is already satisfied")
  endif()
endmacro()

function(_ament_vendor TARGET_NAME VCS_TYPE VCS_URL VCS_VERSION PATCHES CMAKE_ARGS SOURCE_SUBDIR SKIP_INSTALL)
  set(PATCH_FILES)
  foreach(PATCH ${PATCHES})
    if(NOT IS_ABSOLUTE ${PATCH})
      set(PATCH "${CMAKE_CURRENT_LIST_DIR}/${PATCH}")
    endif()
    if(NOT EXISTS ${PATCH})
      message(FATAL_ERROR "ament_vendor() could not find patch file: ${PATCH}")
    endif()
    if(IS_DIRECTORY ${PATCH})
      file(GLOB PATCH LIST_DIRECTORIES FALSE "${PATCH}/*.patch" "${PATCH}/*.diff")
      list(SORT PATCH)
    endif()
    list(APPEND PATCH_FILES ${PATCH})
  endforeach()

  if(PATCH_FILES)
    set(PATCH_COMMAND)
    foreach(PATCH ${PATCH_FILES})
      list(APPEND PATCH_COMMAND COMMAND echo "Applying patch: ${PATCH}")
      list(APPEND PATCH_COMMAND COMMAND ${CMAKE_COMMAND} -E chdir <SOURCE_DIR> git apply --whitespace=nowarn -p1 ${PATCH})
    endforeach()
    list(POP_FRONT PATCH_COMMAND)
  endif()

  list(PREPEND CMAKE_ARGS
    -DCMAKE_STAGING_PREFIX=<INSTALL_DIR>
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/opt/${PROJECT_NAME}"
    -Wno-dev
  )

  if(DEFINED CMAKE_GENERATOR)
    list(PREPEND CMAKE_ARGS -G "${CMAKE_GENERATOR}")
  endif()

  set(CMAKE_ARGS_CONTENT "# CMake configuration for ${TARGET_NAME}")
  set(CMAKE_ARGS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-config.cmake")

  if(DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(CMAKE_TOOLCHAIN_FILE [=[${CMAKE_TOOLCHAIN_FILE}]=] CACHE INTERNAL \"\")")
    if(ANDROID)
      if(DEFINED ANDROID_ABI)
        set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(ANDROID_ABI [=[${ANDROID_ABI}]=] CACHE INTERNAL \"\")")
      endif()
      if(DEFINED ANDROID_CPP_FEATURES)
        set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(ANDROID_CPP_FEATURES [=[${ANDROID_CPP_FEATURES}]=] CACHE INTERNAL \"\")")
      endif()
      if(DEFINED ANDROID_FUNCTION_LEVEL_LINKING)
        set(CMAKE_ARGS_CONTENT
          "${CMAKE_ARGS_CONTENT}\nset(ANDROID_FUNCTION_LEVEL_LINKING [=[${ANDROID_FUNCTION_LEVEL_LINKING}]=] CACHE INTERNAL \"\")")
      endif()
      if(DEFINED ANDROID_NATIVE_API_LEVEL)
        set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(ANDROID_NATIVE_API_LEVEL [=[${ANDROID_NATIVE_API_LEVEL}]=] CACHE INTERNAL \"\")")
      endif()
      if(DEFINED ANDROID_NDK)
        set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(ANDROID_NDK [=[${ANDROID_NDK}]=] CACHE INTERNAL \"\")")
      endif()
      if(DEFINED ANDROID_STL)
        set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(ANDROID_STL [=[${ANDROID_STL}]=] CACHE INTERNAL \"\")")
      endif()
      if(DEFINED ANDROID_TOOLCHAIN_NAME)
        set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(ANDROID_TOOLCHAIN_NAME [=[${ANDROID_TOOLCHAIN_NAME}]=] CACHE INTERNAL \"\")")
      endif()
    endif()
  else()
    if(DEFINED CMAKE_C_COMPILER)
      set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(CMAKE_C_COMPILER [=[${CMAKE_C_COMPILER}]=] CACHE INTERNAL \"\")")
    endif()

    if(DEFINED CMAKE_CXX_COMPILER)
      set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(CMAKE_CXX_COMPILER [=[${CMAKE_CXX_COMPILER}]=] CACHE INTERNAL \"\")")
    endif()
  endif()

  if(DEFINED CMAKE_C_FLAGS)
    set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(CMAKE_C_FLAGS [=[${CMAKE_C_FLAGS}]=] CACHE INTERNAL \"\")")
  endif()

  if(DEFINED CMAKE_CXX_FLAGS)
    set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(CMAKE_CXX_FLAGS [=[${CMAKE_CXX_FLAGS}]=] CACHE INTERNAL \"\")")
  endif()

  if(DEFINED CMAKE_VERBOSE_MAKEFILE)
    set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(CMAKE_VERBOSE_MAKEFILE [=[${CMAKE_VERBOSE_MAKEFILE}]=] CACHE INTERNAL \"\")")
  endif()

  if(DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(CMAKE_BUILD_TYPE [=[${CMAKE_BUILD_TYPE}]=] CACHE INTERNAL \"\")")
  endif()

  list(PREPEND CMAKE_PREFIX_PATH ${_AMENT_CMAKE_VENDOR_PACKAGE_PREFIX_PATH})
  set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(CMAKE_PREFIX_PATH [=[${CMAKE_PREFIX_PATH}]=] CACHE INTERNAL \"\")")

  set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(BUILD_TESTING \"OFF\" CACHE INTERNAL \"\")")

  if(DEFINED BUILD_SHARED_LIBS)
    set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(BUILD_SHARED_LIBS [=[${BUILD_SHARED_LIBS}]=] CACHE INTERNAL \"\")")
  else()
    set(CMAKE_ARGS_CONTENT "${CMAKE_ARGS_CONTENT}\nset(BUILD_SHARED_LIBS ON CACHE BOOL \"\")")
  endif()

  file(GENERATE OUTPUT "${CMAKE_ARGS_FILE}" CONTENT "${CMAKE_ARGS_CONTENT}")
  list(PREPEND CMAKE_ARGS "-C${CMAKE_ARGS_FILE}")

  set(EXTERNALPROJECT_ARGS "")
  if(VCS_TYPE STREQUAL "path")
    if(NOT IS_ABSOLUTE ${VCS_URL})
      set(VCS_URL "${CMAKE_CURRENT_LIST_DIR}/${VCS_URL}")
    endif()
    if(NOT EXISTS ${VCS_URL})
      message(FATAL_ERROR "ament_vendor() could not find sources path: ${VCS_URL}")
    endif()
    list(
      APPEND EXTERNALPROJECT_ARGS
      SOURCE_DIR ${VCS_URL}
    )
  else()
    set(REPOS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.repos")
    configure_file("${ament_cmake_vendor_package_DIR}/templates/target.repos.in" ${REPOS_FILE} @ONLY)
    find_program(vcs_EXECUTABLE vcs REQUIRED)
    list(
      APPEND EXTERNALPROJECT_ARGS
      DOWNLOAD_COMMAND "${CMAKE_COMMAND}" -E rm -rf <SOURCE_DIR> &&
      DOWNLOAD_COMMAND "${CMAKE_COMMAND}" -E make_directory <SOURCE_DIR> &&
      DOWNLOAD_COMMAND "${vcs_EXECUTABLE}" import . --input "${REPOS_FILE}" --shallow --recursive --force
      SOURCE_SUBDIR ${SOURCE_SUBDIR}
    )
  endif()

  include(ExternalProject)

  set(INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-prefix/install")
  externalproject_add(
    ${TARGET_NAME}
    INSTALL_DIR ${INSTALL_DIR}
    UPDATE_DISCONNECTED TRUE
    PATCH_COMMAND ${PATCH_COMMAND}
    CMAKE_ARGS ${CMAKE_ARGS}
    ${EXTERNALPROJECT_ARGS}
  )

  externalproject_add_stepdependencies(${TARGET_NAME} download ${REPOS_FILE})
  if(PATCH_FILES)
    externalproject_add_stepdependencies(${TARGET_NAME} download ${PATCH_FILES})
  endif()
  if(VCS_TYPE STREQUAL "path")
    file(GLOB_RECURSE SOURCE_FILES
      LIST_DIRECTORIES FALSE
      "${VCS_URL}/*")
    externalproject_add_stepdependencies(${TARGET_NAME} configure ${SOURCE_FILES})
  endif()
  externalproject_add_stepdependencies(${TARGET_NAME} configure ${CMAKE_ARGS_FILE})

  if(NOT SKIP_INSTALL)
    install(
      DIRECTORY "${INSTALL_DIR}/"
      DESTINATION "opt/${PROJECT_NAME}"
      USE_SOURCE_PERMISSIONS
    )
  endif()
endfunction()
