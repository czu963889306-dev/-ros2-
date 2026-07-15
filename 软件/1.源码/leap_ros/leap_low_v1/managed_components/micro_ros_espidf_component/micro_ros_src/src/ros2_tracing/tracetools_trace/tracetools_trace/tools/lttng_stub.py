# Copyright 2019 Robert Bosch GmbH
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

"""Stub version of the interface for tracing with LTTng."""


ERROR_MESSAGE = 'lttng module not found, but still tried to use it'


def setup(*args, **kwargs) -> None:
    raise RuntimeError(ERROR_MESSAGE)


def start(*args, **kwargs) -> None:
    raise RuntimeError(ERROR_MESSAGE)


def stop(*args, **kwargs) -> None:
    raise RuntimeError(ERROR_MESSAGE)


def destroy(*args, **kwargs) -> None:
    raise RuntimeError(ERROR_MESSAGE)
