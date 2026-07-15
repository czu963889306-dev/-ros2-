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
# Get the package names of the available ROS middleware implementations.
#
# The result can be overridden by setting either a CMake or environment
# variable named ``RMW_IMPLEMENTATIONS``.
# The variable can contain RMW implementation names separated by the platform
# specific path separator.
# Including an unavailable RMW implementation results in a fatal error message.
#
# :param var: the output variable name containing the package names
# :type var: list of strings
#
function(get_available_rmw_implementations var)
  ament_index_get_resources(middleware_implementations "rmw_typesupport")
  if(DEFINED middleware_implementations)
    list(REMOVE_ITEM middleware_implementations "rmw_implementation")
    list(SORT middleware_implementations)
  endif()

  # use explicitly provided list if provided
  # option()
  if(NOT "$ENV{RMW_IMPLEMENTATIONS}" STREQUAL "")
    string(REPLACE ":" ";" RMW_IMPLEMENTATIONS "$ENV{RMW_IMPLEMENTATIONS}")
  endif()
  if(NOT "${RMW_IMPLEMENTATIONS}" STREQUAL "")
    # check if given RMW implementations are available
    foreach(rmw_implementation ${RMW_IMPLEMENTATIONS})
      if(NOT "${rmw_implementation}" IN_LIST middleware_implementations)
        message(FATAL_ERROR
          "The RMW implementation '${rmw_implementation}' specified in "
          "'RMW_IMPLEMENTATIONS' is not available ("
          "${middleware_implementations})")
      endif()
    endforeach()
    set(filtered)
    foreach(rmw_implementation ${middleware_implementations})
      if("${rmw_implementation}" IN_LIST RMW_IMPLEMENTATIONS)
        list(APPEND filtered "${rmw_implementation}")
      endif()
      set(middleware_implementations ${filtered})
    endforeach()
    message(STATUS
      "Filtered available RMW implementations: ${middleware_implementations}")
  endif()

  set(${var} ${middleware_implementations} PARENT_SCOPE)
endfunction()
