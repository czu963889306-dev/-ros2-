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

import unittest
from unittest import mock

from tracetools_trace.tools.lttng import is_lttng_installed


@unittest.skipIf(not is_lttng_installed(), 'LTTng is required')
class TestLttngTracing(unittest.TestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
        )

    def test_lttng_not_installed(self):
        from tracetools_trace.tools.lttng import lttng_init
        with mock.patch('tracetools_trace.tools.lttng.is_lttng_installed', return_value=False):
            self.assertIsNone(lttng_init(session_name='test-session', base_path='/tmp'))

    def test_no_kernel_tracer(self):
        from tracetools_trace.tools.lttng_impl import setup
        with mock.patch(
            'tracetools_trace.tools.lttng_impl.is_kernel_tracer_available',
            return_value=(False, 'some error message'),
        ):
            with mock.patch('lttng.session_daemon_alive', return_value=1):
                self.assertIsNone(
                    setup(
                        session_name='test-session',
                        base_path='/tmp',
                        kernel_events=['sched_switch'],
                    ))
