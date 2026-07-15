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

import os
import re

try:
    import importlib.resources as importlib_resources
except ModuleNotFoundError:
    import importlib_resources

IS_WINDOWS = os.name == 'nt'


def get_environment_hook_template_path(name):
    with importlib_resources.path('ament_package.template.environment_hook', name) as path:
        return str(path)


def get_package_level_template_names(all_platforms=False):
    names = ['local_setup.%s.in' % ext for ext in [
        'bash',
        'bat',
        'sh',
        'zsh',
    ]]
    if not all_platforms:
        names = [name for name in names if _is_platform_specific_extension(name)]
    return names


def get_package_level_template_path(name):
    with importlib_resources.path('ament_package.template.package_level', name) as path:
        return str(path)


def get_prefix_level_template_names(*, all_platforms=False):
    extensions = [
        'bash',
        'bat.in',
        'sh.in',
        'zsh',
    ]
    names = ['local_setup.%s' % ext for ext in extensions] + \
        ['setup.%s' % ext for ext in extensions] + \
        ['_local_setup_util.py']
    if not all_platforms:
        names = [name for name in names if _is_platform_specific_extension(name)]
    return names


def get_prefix_level_template_path(name):
    with importlib_resources.path('ament_package.template.prefix_level', name) as path:
        return str(path)


def get_isolated_prefix_level_template_names(*, all_platforms=False):
    extensions = [
        'bash',
        'bat.in',
        'sh.in',
        'zsh',
    ]
    names = ['local_setup.%s' % ext for ext in extensions] + \
        ['_order_isolated_packages.py']
    # + ['setup.%s' % ext for ext in extensions]
    if not all_platforms:
        names = [name for name in names if _is_platform_specific_extension(name)]
    return names


def get_isolated_prefix_level_template_path(name):
    with importlib_resources.path('ament_package.template.isolated_prefix_level', name) as path:
        return str(path)


def configure_file(template_file, environment):
    """
    Evaluate a .in template file used in CMake with configure_file.

    :param template_file: path to the template, ``str``
    :param environment: dictionary of placeholders to substitute,
      ``dict``
    :returns: string with evaluates template
    :raises: KeyError for placeholders in the template which are not
      in the environment
    """
    with open(template_file, 'r') as f:
        template = f.read()
        return configure_string(template, environment)


def configure_string(template, environment):
    """
    Substitute variables enclosed by @ characters.

    :param template: the template, ``str``
    :param environment: dictionary of placeholders to substitute,
      ``dict``
    :returns: string with evaluates template
    :raises: KeyError for placeholders in the template which are not
      in the environment
    """
    def substitute(match):
        var = match.group(0)[1:-1]
        if var in environment:
            return environment[var]
        return ''
    return re.sub(r'\@[a-zA-Z0-9_]+\@', substitute, template)


def _is_platform_specific_extension(filename):
    if filename.endswith('.in'):
        filename = filename[:-3]
    if not IS_WINDOWS and filename.endswith('.bat'):
        # On non-Windows system, ignore .bat
        return False
    if IS_WINDOWS and os.path.splitext(filename)[1] not in ['.bat', '.py']:
        # On Windows, ignore anything other than .bat and .py
        return False
    return True
