# Copyright 2014 Open Source Robotics Foundation, Inc.
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
# Export libraries to downstream packages.
#
# :param ARGN: a list of libraries.
#   Each element might either be an absolute path to a library, a
#   CMake library target, or a CMake imported libary target.
#   Note that this macro is not needed for interface library targets.
#   If a plain library name is passed it will be redirected to
#   ament_export_library_names().
# :type ARGN: list of strings
#
# @public
#
macro(ament_export_libraries)
  if(_${PROJECT_NAME}_AMENT_PACKAGE)
    message(FATAL_ERROR
      "ament_export_libraries() must be called before ament_package()")
  endif()

  if(${ARGC} GREATER 0)
    _ament_cmake_export_libraries_register_environment_hook()
    _ament_cmake_export_libraries_register_package_hook()
    # loop over libraries
    # but remember related build configuration keyword if available
    set(_argn ${ARGN})
    set(_i 0)
    while(_i LESS ${ARGC})
      list(GET _argn ${_i} _arg)

      # pass linker flags along
      if("${_arg}" MATCHES "^-" AND NOT "${_arg}" MATCHES "^-[l|framework]")
        list(APPEND _AMENT_EXPORT_LIBRARIES "${_arg}")
        math(EXPR _i "${_i} + 1")
        continue()
      endif()

      if("${_arg}" MATCHES "^(debug|optimized|general)$")
        # remember build configuration keyword
        # and get following library
        set(_cfg "${_arg}")
        math(EXPR _i "${_i} + 1")
        if(_i EQUAL ${ARGC})
          message(FATAL_ERROR "ament_export_libraries() package "
            "'${PROJECT_NAME}' passes the build configuration keyword "
            "'${_cfg}' as the last exported library")
        endif()
        list(GET _argn ${_i} _lib)
      else()
        # the value is a library without a build configuration keyword
        set(_cfg "")
        set(_lib "${_arg}")
      endif()
      math(EXPR _i "${_i} + 1")

      if(IS_ABSOLUTE "${_lib}")
        # keep absolute libraries as-is
        if(NOT EXISTS "${_lib}")
          message(FATAL_ERROR
            "ament_export_libraries() package '${PROJECT_NAME}' exports the "
            "library '${_lib}' which doesn't exist")
        endif()
        list(APPEND _AMENT_EXPORT_LIBRARIES ${_cfg} "${_lib}")
      elseif(TARGET "${_lib}")
        # sometimes cmake dependencies define imported targets
        # in which case the imported library information is not the target name
        # but the information is embedded in properties of the imported target
        get_target_property(_is_imported "${_lib}" IMPORTED)
        if(_is_imported)
          set(_imported_libraries "")
          get_target_property(_imported_location "${_lib}" IMPORTED_LOCATION)
          if(_imported_location)
            list(APPEND _imported_libraries ${_imported_location})
          else()
            get_target_property(_imported_configurations "${_lib}"
              IMPORTED_CONFIGURATIONS)
            foreach(_cfg ${_imported_configurations})
              get_target_property(_imported_location_${_cfg} "${_lib}"
                IMPORTED_LOCATION_${_cfg})
              if(_imported_location_${_cfg})
                list(APPEND _imported_libraries ${_imported_location_${_cfg}})
              endif()
            endforeach()
          endif()
          foreach(_imported_library ${_imported_libraries})
            # verify that imported library is an existing absolute path
            if(NOT IS_ABSOLUTE "${_imported_library}")
              message(FATAL_ERROR "ament_export_libraries() package "
                "'${PROJECT_NAME}' exports the imported library "
                "'${_imported_library}' which is not an absolute path")
            endif()
            if(NOT EXISTS "${_imported_library}")
              message(FATAL_ERROR "ament_export_libraries() package "
                "'${PROJECT_NAME}' exports the imported library "
                "'${_imported_library}' which doesn't exist")
            endif()
            list(APPEND _AMENT_EXPORT_LIBRARIES
              ${_cfg} "${_imported_library}")
          endforeach()
        else()
          # keep plain target names as-is
          # they will be resolved via find_library()
          # relative to the CMAKE_INSTALL_PREFIX
          # when find_package() is invoked from a downstream package
          get_target_property(_output_name ${_lib} OUTPUT_NAME)
          if(_output_name)
            list(APPEND _AMENT_EXPORT_LIBRARIES ${_cfg} "${_output_name}")
          else()
            list(APPEND _AMENT_EXPORT_LIBRARIES ${_cfg} "${_lib}")
          endif()
        endif()
      else()
        # keep plain library names as-is
        # they will be resolved via find_library()
        # when find_package() is invoked from a downstream package
        ament_export_library_names(${_cfg} "${_lib}")
      endif()
    endwhile()
  endif()
endmacro()
