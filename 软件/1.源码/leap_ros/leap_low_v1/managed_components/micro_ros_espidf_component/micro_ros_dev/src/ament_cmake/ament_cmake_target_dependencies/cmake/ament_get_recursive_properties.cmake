# Copyright 2020 Open Source Robotics Foundation, Inc.
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
# Get the recursive include directories and libraries from interface targets.
#
# :param var_include_dirs: the output variable name
# :type var_include_dirs: string
# :param var_libraries: the output variable name
# :type var_libraries: string
# :param ARGN: a list of interface targets
# :type ARGN: list of strings
#
# @public
#
function(ament_get_recursive_properties var_include_dirs var_libraries)
  set(all_include_dirs "")
  set(all_libraries "")

  if(${ARGC} GREATER 0)
    foreach(target ${ARGN})
      # only use actual targets
      if(NOT TARGET "${target}")
        continue()
      endif()

      get_target_property(include_dirs ${target} INTERFACE_INCLUDE_DIRECTORIES)
      if(include_dirs)
        list_append_unique(all_include_dirs "${include_dirs}")
      endif()

      get_target_property(link_libraries ${target} INTERFACE_LINK_LIBRARIES)
      if(link_libraries)
        foreach(link_library ${link_libraries})
          if(TARGET "${link_library}")
            ament_get_recursive_properties(include_dirs libraries "${link_library}")
            list_append_unique(all_include_dirs "${include_dirs}")
            list_append_unique(all_libraries "${libraries}")
          else()
            list(APPEND all_libraries ${link_library})
          endif()
        endforeach()
      endif()

      get_target_property(imported_configurations ${target} IMPORTED_CONFIGURATIONS)
      if(imported_configurations)
        foreach(imported_config ${imported_configurations})
          get_target_property(imported_implib ${target} IMPORTED_IMPLIB_${imported_config})
          if(imported_implib)
            list(APPEND all_libraries "${imported_implib}")
          else()
            get_target_property(imported_location ${target} IMPORTED_LOCATION_${imported_config})
            if(imported_location)
              list(APPEND all_libraries "${imported_location}")
            endif()
          endif()
        endforeach()
      endif()
    endforeach()

    ament_include_directories_order(ordered_include_dirs ${all_include_dirs})
    ament_libraries_deduplicate(unique_libraries ${all_libraries})
  endif()

  set(${var_include_dirs} ${ordered_include_dirs} PARENT_SCOPE)
  set(${var_libraries} ${unique_libraries} PARENT_SCOPE)
endfunction()
