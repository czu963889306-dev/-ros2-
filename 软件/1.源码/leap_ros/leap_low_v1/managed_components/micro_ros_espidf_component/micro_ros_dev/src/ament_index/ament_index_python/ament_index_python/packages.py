# Copyright 2017 Open Source Robotics Foundation, Inc.
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
import pathlib
import re
import warnings

from .resources import get_resource
from .resources import get_resources
from .search_paths import get_search_paths


class PackageNotFoundError(KeyError):
    pass


def get_packages_with_prefixes():
    """
    Return a dict of package names to the prefixes in which they are found.

    :returns: dict of package names to their prefixes
    :rtype: dict
    """
    return get_resources('packages')


def get_package_prefix(package_name):
    """
    Return the installation prefix directory of the given package.

    For example, if you install the package 'foo' into
    '/home/user/ros2_ws/install' and you called this function with 'foo' as the
    argument, then it will return that directory.

    :param str package_name: name of the package to locate
    :returns: installation prefix of the package
    :raises: :exc:`PackageNotFoundError` if the package is not found
    :raises: :exc:`ValueError` if the package name is invalid
    """
    # This regex checks for a valid package name as defined by REP-127 including the recommended
    #  exemptions. See https://ros.org/reps/rep-0127.html#name
    if re.fullmatch('[a-zA-Z0-9][a-zA-Z0-9_-]+', package_name, re.ASCII) is None:
        raise ValueError(
            "'{}' is not a valid package name".format(package_name))
    try:
        content, package_prefix = get_resource('packages', package_name)
    except LookupError:
        raise PackageNotFoundError(
            "package '{}' not found, searching: {}".format(package_name, get_search_paths()))
    return package_prefix


def get_package_share_directory(package_name, print_warning=True):
    """
    Return the share directory of the given package.

    For example, if you install the package 'foo' into
    '/home/user/ros2_ws/install' and you called this function with 'foo' as the
    argument, then it will return '/home/user/ros2_ws/install/share/foo' as
    the package's share directory.

    :param str package_name: name of the package to locate
    :param bool print_warning: if true, print a warning if the directory does not exist
    :returns: share directory of the package
    :raises: :exc:`PackageNotFoundError` if the package is not found
    :raises: :exc:`ValueError` if the package name is invalid
    """
    path = os.path.join(get_package_prefix(package_name), 'share', package_name)
    if print_warning and not os.path.exists(path):
        warnings.warn(f'Share directory for {package_name} ({path}) does not exist.', stacklevel=2)
    return path


def get_package_share_path(package_name, print_warning=True):
    """
    Return the share directory of the given package as a pathlib.Path.

    For example, if you install the package 'foo' into
    '/home/user/ros2_ws/install' and you called this function with 'foo' as the
    argument, then it will return a path representing '/home/user/ros2_ws/install/share/foo'
    and then you could use it to construct the path to a shared file with
    `get_package_share_path('foo') / 'urdf/robot.urdf'`

    :param str package_name: name of the package to locate
    :param bool print_warning: if true, print a warning if the path does not exist
    :returns: share directory of the package as a pathlib.Path
    :raises: :exc:`PackageNotFoundError` if the package is not found
    """
    path = pathlib.Path(get_package_share_directory(package_name, print_warning=False))
    if print_warning and not path.exists():
        warnings.warn(f'Share path for {package_name} ({path}) does not exist.', stacklevel=2)
    return path
