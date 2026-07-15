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

import pathlib

from ament_index_python import get_resources
from rosidl_cli.command.generate.api import generate

TEST_DIR = str(pathlib.Path(__file__).parent)


def test_cli_extension_for_smoke(tmp_path, capsys):
    # NOTE(hidmic): pytest and empy do not play along,
    # the latter expects some proxy will stay in sys.stdout
    # and the former insists in overwriting it
    interface_files = [TEST_DIR + ':msg/Test.msg']

    with capsys.disabled():  # so do everything in one run
        # Passing target typesupport implementations explictly
        ts = 'cpp[typesupport_implementations:{}]'.format(
            list(get_resources('rosidl_typesupport_cpp'))
        )
        generate(
            package_name='rosidl_typesupport_cpp',
            interface_files=interface_files,
            typesupports=[ts],
            output_path=tmp_path / 'explicit_args'
        )

        # Using default typesupport implementations
        generate(
            package_name='rosidl_typesupport_cpp',
            interface_files=interface_files,
            typesupports=['cpp'],
            output_path=tmp_path / 'defaults'
        )
