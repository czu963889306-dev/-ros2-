# Copyright 2020 Christophe Bedard
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

"""Signal handling utilities."""

import signal
from typing import Any
from typing import Callable
from typing import Dict
from typing import List
from typing import Optional


class SignalHandledException(RuntimeError):
    """Exception raised after a signal is handled."""

    pass


class SignalHandlerUtil():
    """
    Signal handler as a context manager.

    Modified version of: https://stackoverflow.com/a/35798485/6476709
    """

    def __init__(
        self,
        release_callback: Optional[Callable[[], None]] = None,
        raise_after_signal: bool = False,
        signals: List[int] = [signal.SIGINT],
    ) -> None:
        """
        Create a SignalHandlerUtil object.

        :param release_callback: the function to call on release, possibly after handling a signal
        :param raise_after_signal: whether to raise a SignalHandledException after signal/callback
        :param signals: the list of signals to handle
        """
        self.release_callback = release_callback
        self.raise_after_signal = raise_after_signal
        self.signals = signals
        self.original_handlers: Dict[int, Any] = {}

    def __enter__(self) -> 'SignalHandlerUtil':
        """Enter context and setup signal handlers."""
        self.interrupted = False
        self.released = False

        for sig in self.signals:
            self.original_handlers[sig] = signal.getsignal(sig)
            signal.signal(sig, self._handler)

        return self

    def _handler(self, signum, frame) -> None:
        """Handle signal and trigger release."""
        self.interrupted = True
        if signal.SIGINT == signum:
            print()
        self._release()

    def __exit__(self, exc_type, exc_value, traceback) -> Optional[bool]:
        """Exit context and trigger release."""
        self._release()
        # Suppress this specific exception, since it is only meant to be a notification
        if SignalHandledException == exc_type:
            return True
        return None

    def _release(self) -> bool:
        """Release and restore signal handlers."""
        if self.released:
            return False

        for sig in self.signals:
            signal.signal(sig, self.original_handlers[sig])

        self.released = True
        if self.release_callback:
            self.release_callback()
        if self.interrupted and self.raise_after_signal:
            raise SignalHandledException()
        return True


def execute_and_handle_sigint(
    run_function: Callable[[], None],
    fini_function: Optional[Callable[[], None]] = None,
) -> None:
    """
    Execute a task and handle SIGINT to always finalize cleanly.

    The main task function is interrupted on SIGINT.
    The finalization function (if provided) is always executed, either
    after the main task function is done or after it is interrupted.

    :param run_function: the task function, which may be interrupted
    :param fini_function: the optional finalization/cleanup function
    """
    with SignalHandlerUtil(release_callback=fini_function, raise_after_signal=True):
        try:
            run_function()
        except SignalHandledException:
            pass
