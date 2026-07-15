# Copyright 2019 Open Source Robotics Foundation, Inc.
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

import argparse
import sys

from ament_index_python.resources import get_resource
from ament_index_python.resources import get_resource_types
from ament_index_python.resources import get_resources


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description='Query the ament resource index.')
    arg = parser.add_argument(
        'resource_type', nargs='?', metavar='TYPE',
        help='The type of the resource')
    arg.completer = resource_type_completer
    arg = parser.add_argument(
        'resource_name', nargs='?', metavar='NAME',
        help='The name of the resource')
    arg.completer = resource_name_completer

    try:
        from argcomplete import autocomplete
    except ImportError:
        pass
    else:
        autocomplete(parser, exclude=['-h', '--help'])

    args = parser.parse_args(argv)

    if args.resource_type is None:
        for resource_type in sorted(get_resource_types()):
            print(resource_type)
        return

    if args.resource_name is None:
        resources = get_resources(args.resource_type)
        for resource_name in sorted(resources.keys()):
            print(resource_name + '\t' + resources[resource_name])
        return

    try:
        content, path = get_resource(args.resource_type, args.resource_name)
    except (LookupError, OSError) as e:
        return str(e)
    print(path)
    if content:
        print('<<<')
        print(content)
        print('>>>')


def resource_type_completer(prefix, **kwargs):
    return (t for t in get_resource_types() if t.startswith(prefix))


def resource_name_completer(prefix, parsed_args, **kwargs):
    resource_type = getattr(parsed_args, 'resource_type', None)
    if not resource_type:
        return []
    return (t for t in get_resources(resource_type).keys() if t.startswith(prefix))


if __name__ == '__main__':
    sys.exit(main())
