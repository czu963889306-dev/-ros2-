# Copyright 2019 Robert Bosch GmbH
# Copyright 2021 Christophe Bedard
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
import platform
import unittest
from unittest import mock

from launch import LaunchDescription
from launch import LaunchService

from tracetools_launch.actions.ld_preload import LdPreload


@unittest.skipIf('Linux' != platform.system(), 'Linux-specific test')
class TestLdPreloadAction(unittest.TestCase):

    def _assert_launch_no_errors(self, actions):
        ld = LaunchDescription(actions)
        ls = LaunchService(debug=True)
        ls.include_launch_description(ld)
        assert 0 == ls.run()

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
        )

    def test_get_shared_lib_path_fail(self) -> None:
        self.assertIsNone(
            LdPreload.get_shared_lib_path('random_lib_that_does_not_exist_I_hope.so')
        )
        # Not on Linux
        with mock.patch('platform.system', return_value='Darwin'):
            self.assertIsNone(
                LdPreload.get_shared_lib_path('some_lib.so')
            )
        with mock.patch('platform.system', return_value='Windows'):
            self.assertIsNone(
                LdPreload.get_shared_lib_path('some_lib.so')
            )
        # Command failure
        with mock.patch('subprocess.getstatusoutput', return_value=(1, 'failure')):
            self.assertIsNone(
                LdPreload.get_shared_lib_path('some_lib.so')
            )
        # Command works but no results
        with mock.patch('subprocess.getstatusoutput', return_value=(0, 'some_lib:')):
            self.assertIsNone(
                LdPreload.get_shared_lib_path('some_lib.so')
            )
        # Command works but no shared lib
        with mock.patch(
            'subprocess.getstatusoutput',
            return_value=(0, 'some_lib: /path/to/some_lib.a'),
        ):
            self.assertIsNone(
                LdPreload.get_shared_lib_path('some_lib.so')
            )

    def test_get_shared_lib_path(self) -> None:
        # Only a shared lib
        with mock.patch(
            'subprocess.getstatusoutput',
            return_value=(0, 'liblttng-ust-libc-wrapper: /usr/lib/x86_64-linux-gnu/liblttng-ust-libc-wrapper.so'),  # noqa: E501
        ):
            self.assertEqual(
                LdPreload.get_shared_lib_path('liblttng-ust-libc-wrapper.so'),
                '/usr/lib/x86_64-linux-gnu/liblttng-ust-libc-wrapper.so',
            )
        # Both a shared lib and a static lib
        with mock.patch(
            'subprocess.getstatusoutput',
            return_value=(0, 'libc: /usr/lib/x86_64-linux-gnu/libc.so /usr/lib/x86_64-linux-gnu/libc.a'),  # noqa: E501
        ):
            self.assertEqual(
                LdPreload.get_shared_lib_path('libc.so'),
                '/usr/lib/x86_64-linux-gnu/libc.so',
            )
        # Both a static lib and a shared lib
        with mock.patch(
            'subprocess.getstatusoutput',
            return_value=(0, 'libc++: /usr/lib/x86_64-linux-gnu/libc++.a /usr/lib/x86_64-linux-gnu/libc++.so'),  # noqa: E501
        ):
            self.assertEqual(
                LdPreload.get_shared_lib_path('libc++.so'),
                '/usr/lib/x86_64-linux-gnu/libc++.so',
            )

    def test_action(self) -> None:
        self.assertIsNone(os.environ.get('LD_PRELOAD', None))

        # Not found
        ldpreload_action = LdPreload('random_lib_that_does_not_exist_I_hope.so')
        self.assertEqual('random_lib_that_does_not_exist_I_hope.so', ldpreload_action.lib_name)
        self._assert_launch_no_errors([ldpreload_action])
        self.assertFalse(ldpreload_action.lib_found())
        self.assertIsNone(ldpreload_action.lib_path)
        self.assertIsNone(os.environ.get('LD_PRELOAD', None))

        # Found
        with mock.patch(
            'subprocess.getstatusoutput',
            return_value=(0, 'libc++: /usr/lib/x86_64-linux-gnu/libc++.a /usr/lib/x86_64-linux-gnu/libc++.so'),  # noqa: E501
        ):
            ldpreload_action = LdPreload('libc++.so')
            self.assertEqual('libc++.so', ldpreload_action.lib_name)
            self._assert_launch_no_errors([ldpreload_action])
            self.assertTrue(ldpreload_action.lib_found())
            self.assertEqual('/usr/lib/x86_64-linux-gnu/libc++.so', ldpreload_action.lib_path)
            self.assertEqual('/usr/lib/x86_64-linux-gnu/libc++.so', os.environ.get('LD_PRELOAD'))

        # Found as well and appended to LD_PRELOAD env var
        with mock.patch(
            'subprocess.getstatusoutput',
            return_value=(0, 'liblttng-ust-libc-wrapper: /usr/lib/x86_64-linux-gnu/liblttng-ust-libc-wrapper.so'),  # noqa: E501
        ):
            ldpreload_action = LdPreload('liblttng-ust-libc-wrapper.so')
            self.assertEqual('liblttng-ust-libc-wrapper.so', ldpreload_action.lib_name)
            self._assert_launch_no_errors([ldpreload_action])
            self.assertTrue(ldpreload_action.lib_found())
            self.assertEqual(
                '/usr/lib/x86_64-linux-gnu/liblttng-ust-libc-wrapper.so',
                ldpreload_action.lib_path,
            )
            self.assertEqual(
                '/usr/lib/x86_64-linux-gnu/libc++.so:/usr/lib/x86_64-linux-gnu/liblttng-ust-libc-wrapper.so',  # noqa: E501
                os.environ.get('LD_PRELOAD'),
            )

        del os.environ['LD_PRELOAD']


if __name__ == '__main__':
    unittest.main()
