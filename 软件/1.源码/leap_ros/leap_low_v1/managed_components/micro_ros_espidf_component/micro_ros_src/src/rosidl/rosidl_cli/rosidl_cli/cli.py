# Copyright 2021 Open Source Robotics Foundation, Inc.
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
import signal

from rosidl_cli.command.generate import GenerateCommand
from rosidl_cli.command.translate import TranslateCommand
from rosidl_cli.common import get_first_line_doc


def add_subparsers(parser, cli_name, commands):
    """
    Create argparse subparser for each command.

    The ``cli_name`` is used for the title and description of the
    ``add_subparsers`` function call.

    For each command a subparser is created.

    :param parser: the parent argument parser
    :type parser: :py:class:`argparse.ArgumentParser`
    :param str cli_name: name of the command line command to which the
      subparsers are being added
    :param commands: each of the commands contributing specific arguments
    :type commands: :py:class:`List[Command]`
    """
    # add subparser with description of available subparsers
    description = ''

    commands = sorted(commands, key=lambda command: command.name)
    max_length = max(len(command.name) for command in commands)
    for command in commands:
        description += '%s  %s\n' % (
            command.name.ljust(max_length),
            get_first_line_doc(command))
    subparser = parser.add_subparsers(
        title='Commands', description=description,
        metavar=f'Call `{cli_name} <command> -h` for more detailed usage.')
    subparser.dest = '_command'
    subparser.required = True

    # add extension specific sub-sub-parser with its arguments
    for command in commands:
        command_parser = subparser.add_parser(
            command.name,
            description=get_first_line_doc(command),
            formatter_class=argparse.RawDescriptionHelpFormatter)
        command_parser.set_defaults(_command=command)
        command.add_arguments(command_parser)

    return subparser


def main():
    script_name = 'rosidl'
    description = f'{script_name} is an extensible command-line tool ' \
        'for ROS interface generation.'

    # top level parser
    parser = argparse.ArgumentParser(
        description=description,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    commands = [GenerateCommand(), TranslateCommand()]

    # add arguments for command extension(s)
    add_subparsers(
        parser,
        script_name,
        commands
    )

    # register argcomplete hook if available
    try:
        from argcomplete import autocomplete
    except ImportError:
        pass
    else:
        autocomplete(parser, exclude=['-h', '--help'])

    # parse the command line arguments
    args = parser.parse_args()

    # call the main method of the command
    try:
        rc = args._command.main(args=args)
    except KeyboardInterrupt:
        rc = signal.SIGINT
    except (ValueError, RuntimeError) as e:
        rc = str(e)
    return rc
