# Copyright 2015 Open Source Robotics Foundation, Inc.
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
# Generate a JSON / YAML file containing the rosidl generator arguments.
#
#
# @public
#
function(rosidl_write_generator_arguments output_file)
  set(REQUIRED_ONE_VALUE_KEYWORDS
    "PACKAGE_NAME")
  set(OPTIONAL_ONE_VALUE_KEYWORDS
    "OUTPUT_DIR"
    "TEMPLATE_DIR")

  set(REQUIRED_MULTI_VALUE_KEYWORDS  # only require one of them
    "IDL_TUPLES"
    "NON_IDL_TUPLES"
    "ROS_INTERFACE_FILES")
  set(OPTIONAL_MULTI_VALUE_KEYWORDS
    "ROS_INTERFACE_DEPENDENCIES"  # since the dependencies can be empty
    "TARGET_DEPENDENCIES"
    "ADDITIONAL_FILES")

  cmake_parse_arguments(
    ARG
    ""
    "${REQUIRED_ONE_VALUE_KEYWORDS};${OPTIONAL_ONE_VALUE_KEYWORDS}"
    "${REQUIRED_MULTI_VALUE_KEYWORDS};${OPTIONAL_MULTI_VALUE_KEYWORDS}"
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "rosidl_write_generator_arguments() called with unused "
      "arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()
  foreach(required_argument ${REQUIRED_ONE_VALUE_KEYWORDS})
    if(NOT ARG_${required_argument})
      message(FATAL_ERROR
        "rosidl_write_generator_arguments() must be invoked with the "
        "${required_argument} argument")
    endif()
  endforeach()
  set(has_a_required_multi_value_argument FALSE)
  foreach(required_argument ${REQUIRED_MULTI_VALUE_KEYWORDS})
    if(ARG_${required_argument})
      set(has_a_required_multi_value_argument TRUE)
    endif()
  endforeach()
  if(NOT has_a_required_multi_value_argument)
    message(FATAL_ERROR
      "rosidl_write_generator_arguments() must be invoked with at least one of "
      "the ${REQUIRED_MULTI_VALUE_KEYWORDS} arguments")
  endif()

  # create folder
  get_filename_component(output_path "${output_file}" PATH)
  file(MAKE_DIRECTORY "${output_path}")

  # open object
  file(WRITE "${output_file}"
    "{")

  set(first_element TRUE)

  # write string values
  foreach(one_value_argument ${REQUIRED_ONE_VALUE_KEYWORDS} ${OPTIONAL_ONE_VALUE_KEYWORDS})
    if(ARG_${one_value_argument})
      # write conditional comma and mandatory newline
      if(NOT first_element)
        file(APPEND "${output_file}" ",")
      else()
        set(first_element FALSE)
      endif()
      file(APPEND "${output_file}" "\n")

      string(TOLOWER "${one_value_argument}" key)
      string(REPLACE "\\" "\\\\" value "${ARG_${one_value_argument}}")
      file(APPEND "${output_file}"
        "  \"${key}\": \"${value}\"")
    endif()
  endforeach()

  # write array values
  foreach(multi_value_argument ${REQUIRED_MULTI_VALUE_KEYWORDS} ${OPTIONAL_MULTI_VALUE_KEYWORDS})
    if(ARG_${multi_value_argument})
      # write conditional comma and mandatory newline and indentation
      if(NOT first_element)
        file(APPEND "${output_file}" ",")
      else()
        set(first_element FALSE)
      endif()
      file(APPEND "${output_file}" "\n")

      # write key, open array
      string(TOLOWER "${multi_value_argument}" key)
      file(APPEND "${output_file}"
        "  \"${key}\": [\n")

      # write array values, last without trailing colon
      list(GET ARG_${multi_value_argument} -1 last_value)
      list(REMOVE_AT ARG_${multi_value_argument} -1)
      foreach(value ${ARG_${multi_value_argument}})
        string(REPLACE "\\" "\\\\" value "${value}")
        file(APPEND "${output_file}"
          "    \"${value}\",\n")
      endforeach()
      string(REPLACE "\\" "\\\\" last_value "${last_value}")
      file(APPEND "${output_file}"
        "    \"${last_value}\"\n")

      # close array
      file(APPEND "${output_file}"
        "  ]")
    endif()
  endforeach()

  # close object
  file(APPEND "${output_file}"
    "\n}\n")
endfunction()
