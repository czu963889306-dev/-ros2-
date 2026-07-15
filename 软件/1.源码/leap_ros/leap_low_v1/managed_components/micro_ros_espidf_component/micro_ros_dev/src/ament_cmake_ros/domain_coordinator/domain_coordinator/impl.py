# Copyright 2019 Apex.AI, Inc.
# Copyright 2021 Fraunhofer FKIE
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

import contextlib
import socket

# To coordinate ROS_DOMAIN_IDs between multiple test process instances, we
# open a high numbered port "PORT_BASE + ROS_DOMAIN_ID."
# If we manage to open the port, then we can use that ROS_DOMAIN_ID for the duration of the
# test run
PORT_BASE = 32768  # Use ephemeral port range that does not conflict with DDS domains


class default_selector:

    def __init__(self):
        # When we need to coordinate 10 or 20 domains, it's about 10x faster
        # to start with a random seed value here.
        # It's also the difference between 1ms and 100us so it's totally insignificant.
        self._value = 0

    def __call__(self):
        retval = (self._value % 100) + 1
        self._value += 1
        return retval


@contextlib.contextmanager
def domain_id(selector=None):
    """
    Get a ROS_DOMAIN_ID from 1 to 100 that will not conflict with other ROS_DOMAIN_IDs.

    Processes can use domain_id() to generate ROS_DOMAIN_IDs which allow them
    to use ROS 2 without unexpected cross-talk between processes.
    This is similar to the ROS 1 rostest behavior of putting the ROS master on a unique port.

    domain_id() is a context manager. The returned domain ID remains reserved
    for the scope of the context and can be reused afterwards.
    """
    if selector is None:
        selector = default_selector()

    # 100 attempts at most
    for _ in range(100):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            domain_id = selector()
            s.bind(('', PORT_BASE + domain_id))
            yield domain_id
            break
        except OSError:
            pass
        finally:
            s.close()
    else:
        raise RuntimeError('Failed to get a unique ROS domain ID')
