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

# to add more licenses do not extend this file
# instead create a separate package and register custom licenses as entry points

from collections import namedtuple
import os

LicenseEntryPoint = namedtuple(
    'LicenseEntryPoint', ['name', 'spdx', 'file_headers', 'license_files', 'contributing_files'])

TEMPLATE_DIRECTORY = os.path.join(os.path.dirname(__file__), 'template')


def read_data(path, name, prefix, license_type):
    path_template = os.path.join(path, prefix + '_' + license_type + '_%d.txt')
    data = []

    with open(os.path.join(path, prefix + '_' + license_type + '.txt'), 'r') as h:
        data.append(h.read())

    index = 0
    while True:
        try:
            with open(path_template % index, 'r') as h:
                data.append(h.read())
                index += 1
        except OSError:
            break

    return data


def read_license_data(path, name, spdx, prefix):
    file_headers = read_data(path, name, prefix, 'header')
    license_files = read_data(path, name, prefix, 'license')
    contributing_files = read_data(path, name, prefix, 'contributing')

    return LicenseEntryPoint(name, spdx, file_headers, license_files, contributing_files)


# The SPDX identifier (the 3rd argument) comes from the official list at https://spdx.org/licenses/

apache2 = read_license_data(TEMPLATE_DIRECTORY,
                            'Apache License, Version 2.0',
                            'Apache-2.0',
                            'apache2')
boost1 = read_license_data(TEMPLATE_DIRECTORY,
                           'Boost Software License - Version 1.0',
                           'BSL-1.0',
                           'boost1')
bsd2 = read_license_data(TEMPLATE_DIRECTORY,
                         'BSD License 2.0',
                         'BSD-2.0',
                         'bsd2')
bsd_3clause = read_license_data(TEMPLATE_DIRECTORY,
                                '3-Clause BSD License',
                                'BSD-3-Clause',
                                'bsd_3clause')
bsd_2clause = read_license_data(TEMPLATE_DIRECTORY,
                                '2-Clause BSD License',
                                'BSD-2-Clause',
                                'bsd_2clause')
mit = read_license_data(TEMPLATE_DIRECTORY,
                        'MIT License',
                        'MIT',
                        'mit')
mit0 = read_license_data(TEMPLATE_DIRECTORY,
                         'MIT-0 License',
                         'MIT-0',
                         'mit0')
gplv3 = read_license_data(TEMPLATE_DIRECTORY,
                          'GNU General Public License 3.0',
                          'GPL-3.0-only',
                          'gplv3')
lgplv3 = read_license_data(TEMPLATE_DIRECTORY,
                           'GNU Lesser General Public License 3.0',
                           'LGPL-3.0-only',
                           'lgplv3')
