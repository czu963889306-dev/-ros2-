# Copyright 2019 Robert Bosch GmbH
# Copyright 2019, 2020 Christophe Bedard
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

import os
import time


def append_timestamp(
    session_name_base: str,
) -> str:
    """
    Append timestamp to base session name.

    :param session_name_base: the base name of the tracing session
    :return: the session name with timestamp
    """
    return session_name_base + '-' + time.strftime('%Y%m%d%H%M%S')


def get_tracing_directory() -> str:
    """
    Get tracing directory path.

    Uses various environment variables to construct a tracing directory path.
    Use $ROS_TRACE_DIR if ROS_TRACE_DIR is set and not empty.
    Otherwise, use $ROS_HOME/tracing, using ~/.ros for ROS_HOME if not set or if empty.
    It also expands '~' to the current user's home directory,
    and normalizes the path, converting the path separator if necessary.

    :return: the path to the tracing directory
    """
    trace_dir = os.environ.get('ROS_TRACE_DIR')
    if not trace_dir:
        trace_dir = os.environ.get('ROS_HOME')
        if not trace_dir:
            trace_dir = os.path.join('~', '.ros')
        trace_dir = os.path.join(trace_dir, 'tracing')
    return os.path.normpath(os.path.expanduser(trace_dir))
