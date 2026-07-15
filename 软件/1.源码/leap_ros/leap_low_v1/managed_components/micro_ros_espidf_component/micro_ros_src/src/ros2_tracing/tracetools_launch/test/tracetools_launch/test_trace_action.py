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

from typing import List
import unittest

from tracetools_launch.action import Trace


class TestTraceAction(unittest.TestCase):

    def __init__(self, *args) -> None:
        super().__init__(
            *args,
        )

    def test_has_profiling_events(self) -> None:
        events_lists_match: List[List[str]] = [
            [
                '*',
                'ros2:*',
            ],
            [
                'lttng_ust_cyg_profile*:*',
            ],
        ]
        events_lists_match_normal: List[List[str]] = [
            [
                'lttng_ust_cyg_profile:*',
            ],
            [
                'lttng_ust_cyg_profile:func_entry',
                'some_other_event',
                'lttng_ust_cyg_profile:func_exit',
            ],
        ]
        events_lists_match_fast: List[List[str]] = [
            [
                'lttng_ust_cyg_profile_fast:*',
            ],
            [
                'lttng_ust_cyg_profile_fast:func_entry',
                'hashtag:yopo',
            ],
        ]
        events_lists_no_match: List[List[str]] = [
            [
                'ros2:*',
            ],
            [
                'lttng_ust_statedump:bin_info',
                'ros2:event',
            ],
            [
                'lttng_ust_cyg_profile:fake_event',
                'lttng_ust_cyg_profile_fast:fake_event',
            ],
            [],
        ]
        for events in events_lists_match + events_lists_match_normal:
            self.assertTrue(Trace.has_profiling_events(events, False), events)
        for events in events_lists_match + events_lists_match_fast:
            self.assertTrue(Trace.has_profiling_events(events, True), events)
        for events in events_lists_no_match:
            self.assertFalse(Trace.has_profiling_events(events, False), events)
            self.assertFalse(Trace.has_profiling_events(events, True), events)

    def test_has_libc_wrapper_events(self) -> None:
        events_lists_match: List[List[str]] = [
            [
                '*',
                'ros2:*',
            ],
            [
                'lttng_ust_libc:*',
            ],
            [
                'hashtag:yopo',
                'lttng_ust_libc:malloc',
                'lttng_ust_libc:realloc',
            ],
        ]
        events_lists_no_match: List[List[str]] = [
            [
                'ros2:*',
            ],
            [
                'my_random:event',
                'lttng_ust_whatever'
            ],
            [
                'lttng_ust_libc:not_a_match',
            ],
            [],
        ]
        for events in events_lists_match:
            self.assertTrue(Trace.has_libc_wrapper_events(events), events)
        for events in events_lists_no_match:
            self.assertFalse(Trace.has_libc_wrapper_events(events), events)

    def test_has_pthread_wrapper_events(self) -> None:
        events_lists_match: List[List[str]] = [
            [
                '*',
                'ros2:*',
            ],
            [
                'lttng_ust_pthread:*',
            ],
            [
                'hashtag:yopo',
                'lttng_ust_pthread:pthread_mutex_trylock',
                'lttng_ust_pthread:pthread_mutex_unlock',
            ],
        ]
        events_lists_no_match: List[List[str]] = [
            [
                'ros2:*',
            ],
            [
                'my_random:event',
                'lttng_ust_whatever'
            ],
            [
                'lttng_ust_pthread:not_a_match',
            ],
            [],
        ]
        for events in events_lists_match:
            self.assertTrue(Trace.has_pthread_wrapper_events(events), events)
        for events in events_lists_no_match:
            self.assertFalse(Trace.has_pthread_wrapper_events(events), events)

    def test_has_dl_events(self) -> None:
        events_lists_match: List[List[str]] = [
            [
                '*',
                'ros2:*',
            ],
            [
                'lttng_ust_dl:*',
            ],
            [
                'hashtag:yopo',
                'lttng_ust_dl:dlopen',
                'lttng_ust_dl:dlmopen',
            ],
        ]
        events_lists_no_match: List[List[str]] = [
            [
                'ros2:*',
            ],
            [
                'my_random:event',
                'lttng_ust_whatever'
            ],
            [
                'lttng_ust_dl:not_a_match',
            ],
            [],
        ]
        for events in events_lists_match:
            self.assertTrue(Trace.has_dl_events(events), events)
        for events in events_lists_no_match:
            self.assertFalse(Trace.has_dl_events(events), events)


if __name__ == '__main__':
    unittest.main()
