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

"""Module for the Trace action."""

import fnmatch
import re
import shlex
from typing import Dict
from typing import Iterable
from typing import List
from typing import Optional
from typing import Text
from typing import Union

from launch import logging
from launch.action import Action
from launch.event import Event
from launch.event_handlers import OnShutdown
from launch.frontend import Entity
from launch.frontend import expose_action
from launch.frontend import Parser
from launch.launch_context import LaunchContext
from launch.some_substitutions_type import SomeSubstitutionsType
from launch.substitutions import TextSubstitution
from launch.utilities import normalize_to_list_of_substitutions
from launch.utilities import perform_substitutions
from tracetools_trace.tools import lttng
from tracetools_trace.tools import names
from tracetools_trace.tools import path

from .actions.ld_preload import LdPreload


@expose_action('trace')
class Trace(Action):
    """
    Tracing action for launch.

    Sets up and enables tracing through a launch file description.

    It also automatically makes sure that instrumented shared libraries are LD_PRELOADed if the
    corresponding events are enabled:
        * liblttng-ust-libc-wrapper.so: 'lttng_ust_libc:*' events
            * see https://lttng.org/docs/#doc-liblttng-ust-libc-pthread-wrapper
        * liblttng-ust-pthread-wrapper.so: 'lttng_ust_pthread:*' events
            * see https://lttng.org/docs/#doc-liblttng-ust-libc-pthread-wrapper
        * liblttng-ust-cyg-profile-fast.so: lttng_ust_cyg_profile_fast:func_{entry,exit}' events
            * see https://lttng.org/docs/#doc-liblttng-ust-cyg-profile
        * liblttng-ust-cyg-profile.so: 'lttng_ust_cyg_profile:func_{entry,exit}' events
            * see https://lttng.org/docs/#doc-liblttng-ust-cyg-profile
        * liblttng-ust-dl.so: 'lttng_ust_dl:*' events
            * see https://lttng.org/docs/#doc-liblttng-ust-dl
    Note that, if the user provides events or patterns that match both the normal AND the fast
    profiling library, the fast profiling library will be used in practice.
    """

    LIB_LIBC_WRAPPER = 'liblttng-ust-libc-wrapper.so'
    LIB_PTHREAD_WRAPPER = 'liblttng-ust-pthread-wrapper.so'
    LIB_PROFILE_NORMAL = 'liblttng-ust-cyg-profile.so'
    LIB_PROFILE_FAST = 'liblttng-ust-cyg-profile-fast.so'
    LIB_DL = 'liblttng-ust-dl.so'

    EVENTS_LIBC_WRAPPER = [
        'lttng_ust_libc:malloc',
        'lttng_ust_libc:calloc',
        'lttng_ust_libc:realloc',
        'lttng_ust_libc:free',
        'lttng_ust_libc:memalign',
        'lttng_ust_libc:posix_memalign',
    ]
    EVENTS_PTHREAD_WRAPPER = [
        'lttng_ust_pthread:pthread_mutex_lock_req',
        'lttng_ust_pthread:pthread_mutex_lock_acq',
        'lttng_ust_pthread:pthread_mutex_trylock',
        'lttng_ust_pthread:pthread_mutex_unlock',
    ]
    EVENTS_PROFILE_NORMAL = [
        'lttng_ust_cyg_profile:func_entry',
        'lttng_ust_cyg_profile:func_exit',
    ]
    EVENTS_PROFILE_FAST = [
        'lttng_ust_cyg_profile_fast:func_entry',
        'lttng_ust_cyg_profile_fast:func_exit',
    ]
    EVENTS_DL = [
        'lttng_ust_dl:dlopen',
        'lttng_ust_dl:dlmopen',
        'lttng_ust_dl:dlclose',
        'lttng_ust_dl:debug_link',
        'lttng_ust_dl:build_id',
    ]

    def __init__(
        self,
        *,
        session_name: SomeSubstitutionsType,
        append_timestamp: bool = False,
        base_path: Optional[SomeSubstitutionsType] = None,
        events_ust: Iterable[SomeSubstitutionsType] = names.DEFAULT_EVENTS_ROS,
        events_kernel: Iterable[SomeSubstitutionsType] = [],
        context_fields:
            Union[Iterable[SomeSubstitutionsType], Dict[str, Iterable[SomeSubstitutionsType]]]
            = names.DEFAULT_CONTEXT,
        context_names:
            Optional[
                Union[Iterable[SomeSubstitutionsType], Dict[str, Iterable[SomeSubstitutionsType]]]
            ]
            = None,
        **kwargs,
    ) -> None:
        """
        Create a Trace.

        Substitutions are supported for the session name,
        base path, and the lists of events and context fields.

        For the lists of events, wildcards can be used, e.g., 'ros2:*' for
        all events from the 'ros2' tracepoint provider or '*' for all events.

        To disable a type of events (e.g., disable all kernel events) or disable all context
        fields, set the corresponding parameter to an empty list (for Python launch files) or to
        an empty string (through launch frontends).

        :param session_name: the name of the tracing session
        :param append_timestamp: whether to append timestamp to the session name
        :param base_path: the path to the base directory in which to create the session directory,
        or `None` for default
        :param events_ust: the list of ROS UST events to enable
        :param events_kernel: the list of kernel events to enable
        :param context_fields: the names of context fields to enable
            if it's a list or a set, the context fields are enabled for both kernel and userspace;
            if it's a dictionary: { domain type string -> context fields list }
                with the domain type string being either 'kernel' or 'userspace'
        :param context_names: DEPRECATED, use context_fields instead
        """
        super().__init__(**kwargs)
        self.__logger = logging.get_logger(__name__)
        self.__append_timestamp = append_timestamp
        self.__session_name = normalize_to_list_of_substitutions(session_name)
        self.__base_path = base_path \
            if base_path is None else normalize_to_list_of_substitutions(base_path)
        self.__trace_directory = None
        self.__events_ust = [normalize_to_list_of_substitutions(x) for x in events_ust]
        self.__events_kernel = [normalize_to_list_of_substitutions(x) for x in events_kernel]
        # Use value from deprecated param if it is provided
        # TODO(christophebedard) remove context_names param in Rolling after Humble release
        if context_names is not None:
            context_fields = context_names
            self.__logger.warning('context_names parameter is deprecated, use context_fields')
        self.__context_fields = \
            {
                domain: [normalize_to_list_of_substitutions(field) for field in fields]
                for domain, fields in context_fields.items()
            } \
            if isinstance(context_fields, dict) \
            else [normalize_to_list_of_substitutions(field) for field in context_fields]
        self.__ld_preload_actions: List[LdPreload] = []

    @property
    def session_name(self):
        return self.__session_name

    @property
    def base_path(self):
        return self.__base_path

    @property
    def trace_directory(self):
        return self.__trace_directory

    @property
    def events_ust(self):
        return self.__events_ust

    @property
    def events_kernel(self):
        return self.__events_kernel

    @property
    def context_fields(self):
        return self.__context_fields

    @property
    def context_names(self):
        self.__logger.warning('context_names parameter is deprecated, use context_fields')
        return self.__context_fields

    @classmethod
    def _parse_cmdline(
        cls,
        cmd: Text,
        parser: Parser
    ) -> List[SomeSubstitutionsType]:
        """
        Parse text apt for command line execution.

        :param: cmd a space (' ') delimited command line arguments list.
           All found `TextSubstitution` items are split and added to the
           list again as a `TextSubstitution`.
        :return: a list of command line arguments.
        """
        result_args = []
        arg: List[SomeSubstitutionsType] = []

        def _append_arg():
            nonlocal arg
            result_args.append(arg)
            arg = []
        for sub in parser.parse_substitution(cmd):
            if isinstance(sub, TextSubstitution):
                tokens = shlex.split(sub.text)
                if not tokens:
                    # Sting with just spaces.
                    # Appending args allow splitting two substitutions
                    # separated by a space.
                    # e.g.: `$(subst1 asd) $(subst2 bsd)` will be two separate arguments.
                    _append_arg()
                    continue
                if sub.text[0].isspace():
                    # Needed for splitting from the previous argument
                    # e.g.: `$(find-exec bsd) asd`
                    # It splits `asd` from the path of `bsd` executable.
                    if len(arg) != 0:
                        _append_arg()
                arg.append(TextSubstitution(text=tokens[0]))
                if len(tokens) > 1:
                    # Needed to split the first argument when more than one token.
                    # e.g. `$(find-pkg-prefix csd)/asd bsd`
                    # will split `$(find-pkg-prefix csd)/asd` from `bsd`.
                    _append_arg()
                    arg.append(TextSubstitution(text=tokens[-1]))
                if len(tokens) > 2:
                    # If there are more than two tokens, just add all the middle tokens to
                    # `result_args`.
                    # e.g. `$(find-pkg-prefix csd)/asd bsd dsd xsd`
                    # 'bsd' 'dsd' will be added.
                    result_args.extend([TextSubstitution(text=x)] for x in tokens[1:-1])
                if sub.text[-1].isspace():
                    # Allows splitting from next argument.
                    # e.g. `exec $(find-some-file)`
                    # Will split `exec` argument from the result of `find-some-file` substitution.
                    _append_arg()
            else:
                arg.append(sub)
        if arg:
            result_args.append(arg)
        return result_args

    @classmethod
    def parse(cls, entity: Entity, parser: Parser):
        """Parse."""
        _, kwargs = super().parse(entity, parser)

        kwargs['session_name'] = entity.get_attr('session-name')
        append_timestamp = entity.get_attr(
            'append-timestamp', data_type=bool, optional=True, can_be_str=False)
        if append_timestamp is not None:
            kwargs['append_timestamp'] = append_timestamp
        base_path = entity.get_attr('base-path', optional=True)
        if base_path:
            kwargs['base_path'] = parser.parse_substitution(base_path)
        # Make sure to handle empty strings and replace with empty lists,
        # otherwise an empty string enables all events
        events_ust = entity.get_attr('events-ust', optional=True)
        if events_ust is not None:
            kwargs['events_ust'] = cls._parse_cmdline(events_ust, parser) \
                if events_ust else []
        events_kernel = entity.get_attr('events-kernel', optional=True)
        if events_kernel is not None:
            kwargs['events_kernel'] = cls._parse_cmdline(events_kernel, parser) \
                if events_kernel else []
        context_fields = entity.get_attr('context-fields', optional=True)
        if context_fields is not None:
            kwargs['context_fields'] = cls._parse_cmdline(context_fields, parser) \
                if context_fields else []
        context_names = entity.get_attr('context-names', optional=True)
        if context_names is not None:
            kwargs['context_names'] = cls._parse_cmdline(context_names, parser) \
                if context_names else []

        return cls, kwargs

    @staticmethod
    def any_events_match(
        event_patterns: List[str],
        events: List[str],
    ) -> bool:
        """
        Check if the given event names or patterns match any event names in the given list.

        Users can provide exact event names or UNIX-style patterns with wildcards (i.e., asterisk).

        :param event_patterns: the pattern(s) to use for event names
        :param events: the list of event names against which to check for match
        :return: `True` if any of the event patterns matches any of the events, `False` otherwise
        """
        # Translate UNIX-style event patterns into regexes
        event_pattern_regexes = [fnmatch.translate(pattern) for pattern in event_patterns]
        return any(
            any(re.match(event_pattern_regex, event_name) for event_name in events)
            for event_pattern_regex in event_pattern_regexes
        )

    @classmethod
    def has_libc_wrapper_events(
        cls,
        events: List[str],
    ) -> bool:
        """Check if the events list contains at least one libc wrapper event."""
        return cls.any_events_match(events, cls.EVENTS_LIBC_WRAPPER)

    @classmethod
    def has_pthread_wrapper_events(
        cls,
        events: List[str],
    ) -> bool:
        """Check if the events list contains at least one pthread event."""
        return cls.any_events_match(events, cls.EVENTS_PTHREAD_WRAPPER)

    @classmethod
    def has_profiling_events(
        cls,
        events: List[str],
        fast: bool,
    ) -> bool:
        """Check if the events list contains at least one profiling event, fast or normal."""
        return cls.any_events_match(
            events,
            cls.EVENTS_PROFILE_NORMAL if not fast else cls.EVENTS_PROFILE_FAST)

    @classmethod
    def has_dl_events(
        cls,
        events: List[str],
    ) -> bool:
        """Check if the events list contains at least one dl event."""
        return cls.any_events_match(events, cls.EVENTS_DL)

    def __perform_substitutions(self, context: LaunchContext) -> None:
        self.__session_name = perform_substitutions(context, self.__session_name)
        if self.__append_timestamp:
            self.__session_name = path.append_timestamp(self.__session_name)
        self.__base_path = perform_substitutions(context, self.__base_path) \
            if self.__base_path else path.get_tracing_directory()
        self.__events_ust = [perform_substitutions(context, x) for x in self.__events_ust]
        self.__events_kernel = [perform_substitutions(context, x) for x in self.__events_kernel]
        self.__context_fields = \
            {
                domain: [perform_substitutions(context, field) for field in fields]
                for domain, fields in self.__context_fields.items()
            } \
            if isinstance(self.__context_fields, dict) \
            else [perform_substitutions(context, field) for field in self.__context_fields]

        # Add LD_PRELOAD actions if corresponding events are enabled
        if self.has_libc_wrapper_events(self.__events_ust):
            self.__ld_preload_actions.append(LdPreload(self.LIB_LIBC_WRAPPER))
        if self.has_pthread_wrapper_events(self.__events_ust):
            self.__ld_preload_actions.append(LdPreload(self.LIB_PTHREAD_WRAPPER))
        if self.has_dl_events(self.__events_ust):
            self.__ld_preload_actions.append(LdPreload(self.LIB_DL))
        # Warn if events match both normal AND fast profiling libs
        has_fast_profiling_events = self.has_profiling_events(self.__events_ust, True)
        has_normal_profiling_events = self.has_profiling_events(self.__events_ust, False)
        # In practice, the first lib in the LD_PRELOAD list will be used, so the fast one here
        if has_fast_profiling_events:
            self.__ld_preload_actions.append(LdPreload(self.LIB_PROFILE_FAST))
        if has_normal_profiling_events:
            self.__ld_preload_actions.append(LdPreload(self.LIB_PROFILE_NORMAL))
        if has_normal_profiling_events and has_fast_profiling_events:
            self.__logger.warning('events match both normal and fast profiling shared libraries')

    def execute(self, context: LaunchContext) -> Optional[List[Action]]:
        self.__perform_substitutions(context)
        # TODO make sure this is done as early as possible
        if not self._setup():
            # Fail right away if tracing setup fails
            raise RuntimeError('tracing setup failed, see errors above')
        # TODO make sure this is done as late as possible
        context.register_event_handler(OnShutdown(on_shutdown=self._destroy))
        return self.__ld_preload_actions

    def _setup(self) -> bool:
        self.__trace_directory = lttng.lttng_init(
            session_name=self.__session_name,
            base_path=self.__base_path,
            ros_events=self.__events_ust,
            kernel_events=self.__events_kernel,
            context_fields=self.__context_fields,
        )
        if self.__trace_directory is None:
            return False
        self.__logger.info(f'Writing tracing session to: {self.__trace_directory}')
        self.__logger.debug(f'UST events: {self.__events_ust}')
        self.__logger.debug(f'Kernel events: {self.__events_kernel}')
        self.__logger.debug(f'Context fields: {self.__context_fields}')
        self.__logger.debug(f'LD_PRELOAD: {self.__ld_preload_actions}')
        return True

    def _destroy(self, event: Event, context: LaunchContext) -> None:
        self.__logger.debug(f'Finalizing tracing session: {self.__session_name}')
        lttng.lttng_fini(session_name=self.__session_name)

    def __repr__(self):
        return (
            'Trace('
            f'session_name={self.__session_name}, '
            f'base_path={self.__base_path}, '
            f'trace_directory={self.__trace_directory}, '
            f'events_ust={self.__events_ust}, '
            f'events_kernel={self.__events_kernel}, '
            f'context_fields={self.__context_fields}, '
            f'ld_preload_actions={self.__ld_preload_actions})'
        )
