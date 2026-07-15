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

import os

from .constants import AMENT_PREFIX_PATH_ENV_VAR


def get_search_paths():
    """
    Get the paths from the environment variable '{AMENT_PREFIX_PATH_ENV_VAR}'.

    :returns: list of paths
    :raises: :exc:`EnvironmentError`
    """.format(AMENT_PREFIX_PATH_ENV_VAR=AMENT_PREFIX_PATH_ENV_VAR)
    ament_prefix_path = os.environ.get(AMENT_PREFIX_PATH_ENV_VAR)
    if not ament_prefix_path:
        raise EnvironmentError(
            "Environment variable '{}' is not set or empty".format(AMENT_PREFIX_PATH_ENV_VAR))

    paths = ament_prefix_path.split(os.pathsep)
    return [p for p in paths if p and os.path.exists(p)]
