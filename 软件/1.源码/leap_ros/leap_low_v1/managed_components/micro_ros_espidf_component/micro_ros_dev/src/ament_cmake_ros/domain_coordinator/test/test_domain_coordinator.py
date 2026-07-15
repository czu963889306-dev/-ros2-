# Copyright 2019 Apex.AI, Inc.
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

import domain_coordinator


class TestDomainCoordinator(unittest.TestCase):
    """Verify that the domain_coordinator.domain_id() context works as intended."""

    @classmethod
    def setUpClass(cls):
        # If a ros_isolated test (that uses the domain coordinator) runs at the same time as the
        # domain_coordinator test, then the domain_coordinator tests may fail.
        # When we're self-testing the domain_coordinator, select a different PORT_BASE so the
        # the ports used by the test are different than the ports used for real
        domain_coordinator.impl.PORT_BASE += 100

    def test_uniqueness(self):
        """Verify that domain_id() produces no collisions."""
        def nested_contexts(n):
            if n <= 0:
                return
            with domain_coordinator.domain_id() as domain_id:
                self.assertNotIn(domain_id, nested_contexts.used_domains)
                nested_contexts.used_domains.add(domain_id)
                nested_contexts(n - 1)
                nested_contexts.used_domains.remove(domain_id)

        nested_contexts.used_domains = set()
        # We test only 95 domains to prevent spurious failures if some ports in our range
        # happen to be in use by other applications
        nested_contexts(95)

    def test_context_scope(self):
        """Verify that domain_id() releases IDs when the context goes out of scope."""
        used_domains = set()
        for _ in range(101):
            with domain_coordinator.domain_id() as domain_id:
                used_domains.add(domain_id)
        # Check that the domain IDs have been reused, with some slack to prevent
        # spurious failures if some ports in our range happen to be allocated by
        # other applications while the test is running
        self.assertLess(len(used_domains), 5)

    def test_exhaustion(self):
        """Verify that domain_id() raises an exception if no domain ID is available."""
        def nested_contexts(n):
            if n <= 0:
                return
            with domain_coordinator.domain_id():
                nested_contexts(n - 1)

        with self.assertRaises(RuntimeError):
            nested_contexts(101)
