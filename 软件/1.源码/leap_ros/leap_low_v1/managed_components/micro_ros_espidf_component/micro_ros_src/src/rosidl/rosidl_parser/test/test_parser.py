# Copyright 2018 Open Source Robotics Foundation, Inc.
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

import pytest

from rosidl_parser.definition import Action
from rosidl_parser.definition import Array
from rosidl_parser.definition import BasicType
from rosidl_parser.definition import BoundedSequence
from rosidl_parser.definition import BoundedString
from rosidl_parser.definition import BoundedWString
from rosidl_parser.definition import IdlLocator
from rosidl_parser.definition import Include
from rosidl_parser.definition import Message
from rosidl_parser.definition import NamespacedType
from rosidl_parser.definition import Service
from rosidl_parser.definition import UnboundedSequence
from rosidl_parser.definition import UnboundedString
from rosidl_parser.definition import UnboundedWString
from rosidl_parser.parser import get_ast_from_idl_string
from rosidl_parser.parser import get_string_literals_value
from rosidl_parser.parser import parse_idl_file

MESSAGE_IDL_LOCATOR = IdlLocator(
    pathlib.Path(__file__).parent, pathlib.Path('msg') / 'MyMessage.idl')
SERVICE_IDL_LOCATOR = IdlLocator(
    pathlib.Path(__file__).parent, pathlib.Path('srv') / 'MyService.idl')
ACTION_IDL_LOCATOR = IdlLocator(
    pathlib.Path(__file__).parent, pathlib.Path('action') / 'MyAction.idl')


@pytest.fixture(scope='module')
def message_idl_file():
    return parse_idl_file(MESSAGE_IDL_LOCATOR)


def test_whitespace_at_start_of_string():
    # Repeat to check ros2/rosidl#676
    for _ in range(10):
        ast = get_ast_from_idl_string('const string foo = " e";')
        token = next(ast.find_pred(lambda t: 'string_literals' == t.data))
        assert ' e' == get_string_literals_value(token)


def test_whitespace_at_start_of_wide_string():
    # Repeat to check ros2/rosidl#676
    for _ in range(10):
        ast = get_ast_from_idl_string('const wstring foo = L" e";')
        token = next(ast.find_pred(lambda t: 'wide_string_literals' == t.data))
        assert ' e' == get_string_literals_value(token, allow_unicode=True)


def test_whitespace_at_end_of_string():
    # Repeat to check ros2/rosidl#676
    for _ in range(10):
        ast = get_ast_from_idl_string('const string foo = "e ";')
        token = next(ast.find_pred(lambda t: 'string_literals' == t.data))
        assert 'e ' == get_string_literals_value(token)


def test_whitespace_at_end_of_wide_string():
    # Repeat to check ros2/rosidl#676
    for _ in range(10):
        ast = get_ast_from_idl_string('const wstring foo = L"e ";')
        token = next(ast.find_pred(lambda t: 'wide_string_literals' == t.data))
        assert 'e ' == get_string_literals_value(token, allow_unicode=True)


def test_message_parser(message_idl_file):
    messages = message_idl_file.content.get_elements_of_type(Message)
    assert len(messages) == 1


def test_message_parser_includes(message_idl_file):
    includes = message_idl_file.content.get_elements_of_type(Include)
    assert len(includes) == 2
    assert includes[0].locator == 'OtherMessage.idl'
    assert includes[1].locator == 'pkgname/msg/OtherMessage.idl'


def test_message_parser_structure(message_idl_file):
    messages = message_idl_file.content.get_elements_of_type(Message)
    assert len(messages) == 1

    constants = messages[0].constants
    assert len(constants) == 7

    assert constants[0].name == 'SHORT_CONSTANT'
    assert isinstance(constants[0].type, BasicType)
    assert constants[0].type.typename == 'int16'
    assert constants[0].value == -23

    assert constants[1].name == 'UNSIGNED_LONG_CONSTANT'
    assert isinstance(constants[1].type, BasicType)
    assert constants[1].type.typename == 'uint32'
    assert constants[1].value == 42

    assert constants[2].name == 'FLOAT_CONSTANT'
    assert isinstance(constants[2].type, BasicType)
    assert constants[2].type.typename == 'float'
    assert constants[2].value == 1.25

    assert constants[3].name == 'BOOLEAN_CONSTANT'
    assert isinstance(constants[3].type, BasicType)
    assert constants[3].type.typename == 'boolean'
    assert constants[3].value is True

    assert constants[4].name == 'STRING_CONSTANT'
    assert isinstance(constants[4].type, BoundedString)
    assert constants[4].value == 'string_value'

    assert constants[5].name == 'WSTRING_CONSTANT'
    assert isinstance(constants[5].type, BoundedWString)
    assert constants[5].value == 'wstring_value_\\u2122'

    assert constants[6].name == 'EMPTY_STRING_CONSTANT'
    assert isinstance(constants[6].type, BoundedString)
    assert constants[6].value == ''

    structure = messages[0].structure
    assert structure.namespaced_type.namespaces == ['rosidl_parser', 'msg']
    assert structure.namespaced_type.name == 'MyMessage'
    assert len(structure.members) == 45

    assert isinstance(structure.members[0].type, BasicType)
    assert structure.members[0].type.typename == 'int16'
    assert structure.members[0].name == 'short_value'
    assert isinstance(structure.members[1].type, BasicType)
    assert structure.members[1].type.typename == 'int16'
    assert structure.members[1].name == 'short_value2'

    assert isinstance(structure.members[22].type, UnboundedString)
    assert structure.members[22].name == 'string_value'
    assert isinstance(structure.members[23].type, BoundedString)
    assert structure.members[23].type.maximum_size == 5
    assert structure.members[23].name == 'bounded_string_value'

    assert isinstance(structure.members[24].type, UnboundedWString)
    assert structure.members[24].name == 'wstring_value'
    assert isinstance(structure.members[25].type, BoundedWString)
    assert structure.members[25].type.maximum_size == 23
    assert structure.members[25].name == 'bounded_wstring_value'
    assert isinstance(structure.members[26].type, BoundedWString)
    assert structure.members[26].type.maximum_size == 'UNSIGNED_LONG_CONSTANT'
    assert structure.members[26].name == 'constant_bounded_wstring_value'

    assert isinstance(structure.members[27].type, UnboundedSequence)
    assert isinstance(structure.members[27].type.value_type, BasicType)
    assert structure.members[27].type.value_type.typename == 'int16'
    assert structure.members[27].name == 'unbounded_short_values'
    assert isinstance(structure.members[28].type, BoundedSequence)
    assert isinstance(structure.members[28].type.value_type, BasicType)
    assert structure.members[28].type.value_type.typename == 'int16'
    assert structure.members[28].type.maximum_size == 5
    assert structure.members[28].name == 'bounded_short_values'

    assert isinstance(structure.members[29].type, UnboundedSequence)
    assert isinstance(structure.members[29].type.value_type, BoundedString)
    assert structure.members[29].type.value_type.maximum_size == 3
    assert structure.members[29].name == 'unbounded_values_of_bounded_strings'

    assert isinstance(structure.members[30].type, BoundedSequence)
    assert isinstance(structure.members[30].type.value_type, BoundedString)
    assert structure.members[30].type.value_type.maximum_size == 3
    assert structure.members[30].type.maximum_size == 4
    assert structure.members[30].name == 'bounded_values_of_bounded_strings'

    assert isinstance(structure.members[31].type, Array)
    assert isinstance(structure.members[31].type.value_type, BasicType)
    assert structure.members[31].type.value_type.typename == 'int16'
    assert structure.members[31].type.size == 23
    assert structure.members[31].name == 'array_short_values'


def test_message_parser_annotations(message_idl_file):
    messages = message_idl_file.content.get_elements_of_type(Message)
    assert len(messages) == 1
    structure = messages[0].structure

    assert len(structure.annotations) == 2
    assert structure.annotations[0].name == 'verbatim'
    assert len(structure.annotations[0].value) == 2
    assert 'language' in structure.annotations[0].value
    assert structure.annotations[0].value['language'] == 'comment'
    assert 'text' in structure.annotations[0].value
    assert structure.annotations[0].value['text'] == \
        'Documentation of MyMessage.Adjacent string literal.'

    assert structure.annotations[1].name == 'transfer_mode'
    assert structure.annotations[1].value == 'SHMEM_REF'

    assert len(structure.members[2].annotations) == 1

    assert structure.members[2].annotations[0].name == 'default'
    assert len(structure.members[2].annotations[0].value) == 1
    assert 'value' in structure.members[2].annotations[0].value
    assert structure.members[2].annotations[0].value['value'] == 123

    assert len(structure.members[3].annotations) == 2

    assert structure.members[3].annotations[0].name == 'key'
    assert structure.members[3].annotations[0].value is None

    assert structure.members[3].annotations[1].name == 'range'
    assert len(structure.members[3].annotations[1].value) == 2
    assert 'min' in structure.members[3].annotations[1].value
    assert structure.members[3].annotations[1].value['min'] == -10
    assert 'max' in structure.members[3].annotations[1].value
    assert structure.members[3].annotations[1].value['max'] == 10

    assert isinstance(structure.members[32].type, BasicType)
    assert structure.members[32].type.typename == 'float'
    assert structure.members[32].name == 'int_and_frac_with_positive_scientific'
    assert len(structure.members[32].annotations) == 1
    assert structure.members[32].annotations[0].name == 'default'

    assert isinstance(structure.members[33].type, BasicType)
    assert structure.members[33].type.typename == 'float'
    assert structure.members[33].name == 'int_and_frac_with_explicit_positive_scientific'
    assert len(structure.members[33].annotations) == 1
    assert structure.members[33].annotations[0].name == 'default'

    assert isinstance(structure.members[34].type, BasicType)
    assert structure.members[34].type.typename == 'float'
    assert structure.members[34].name == 'int_and_frac_with_negative_scientific'
    assert len(structure.members[34].annotations) == 1
    assert structure.members[34].annotations[0].name == 'default'

    assert isinstance(structure.members[35].type, BasicType)
    assert structure.members[35].type.typename == 'float'
    assert structure.members[35].name == 'int_and_frac'
    assert len(structure.members[35].annotations) == 1
    assert structure.members[35].annotations[0].name == 'default'

    assert isinstance(structure.members[36].type, BasicType)
    assert structure.members[36].type.typename == 'float'
    assert structure.members[36].name == 'int_with_empty_frac'
    assert len(structure.members[36].annotations) == 1
    assert structure.members[36].annotations[0].name == 'default'

    assert isinstance(structure.members[37].type, BasicType)
    assert structure.members[37].type.typename == 'float'
    assert structure.members[37].name == 'frac_only'
    assert len(structure.members[37].annotations) == 1
    assert structure.members[37].annotations[0].name == 'default'

    assert isinstance(structure.members[38].type, BasicType)
    assert structure.members[38].type.typename == 'float'
    assert structure.members[38].name == 'int_with_positive_scientific'
    assert len(structure.members[38].annotations) == 1
    assert structure.members[38].annotations[0].name == 'default'

    assert isinstance(structure.members[39].type, BasicType)
    assert structure.members[39].type.typename == 'float'
    assert structure.members[39].name == 'int_with_explicit_positive_scientific'
    assert len(structure.members[39].annotations) == 1
    assert structure.members[39].annotations[0].name == 'default'

    assert isinstance(structure.members[40].type, BasicType)
    assert structure.members[40].type.typename == 'float'
    assert structure.members[40].name == 'int_with_negative_scientific'
    assert len(structure.members[40].annotations) == 1
    assert structure.members[40].annotations[0].name == 'default'

    assert isinstance(structure.members[41].type, BasicType)
    assert structure.members[41].type.typename == 'float'
    assert structure.members[41].name == 'fixed_int_and_frac'
    assert len(structure.members[41].annotations) == 1
    assert structure.members[41].annotations[0].name == 'default'

    assert isinstance(structure.members[42].type, BasicType)
    assert structure.members[42].type.typename == 'float'
    assert structure.members[42].name == 'fixed_int_with_dot_only'
    assert len(structure.members[42].annotations) == 1
    assert structure.members[42].annotations[0].name == 'default'

    assert isinstance(structure.members[43].type, BasicType)
    assert structure.members[43].type.typename == 'float'
    assert structure.members[43].name == 'fixed_frac_only'
    assert len(structure.members[43].annotations) == 1
    assert structure.members[43].annotations[0].name == 'default'

    assert isinstance(structure.members[44].type, BasicType)
    assert structure.members[44].type.typename == 'float'
    assert structure.members[44].name == 'fixed_int_only'
    assert len(structure.members[44].annotations) == 1
    assert structure.members[44].annotations[0].name == 'default'


@pytest.fixture(scope='module')
def service_idl_file():
    return parse_idl_file(SERVICE_IDL_LOCATOR)


def test_service_parser(service_idl_file):
    services = service_idl_file.content.get_elements_of_type(Service)
    assert len(services) == 1

    srv = services[0]
    assert isinstance(srv, Service)
    assert srv.namespaced_type.namespaces == ['rosidl_parser', 'srv']
    assert srv.namespaced_type.name == 'MyService'
    assert len(srv.request_message.structure.members) == 2
    assert len(srv.response_message.structure.members) == 1

    constants = srv.request_message.constants
    assert len(constants) == 1

    assert constants[0].name == 'SHORT_CONSTANT'
    assert isinstance(constants[0].type, BasicType)
    assert constants[0].type.typename == 'int16'
    assert constants[0].value == -23

    constants = srv.response_message.constants
    assert len(constants) == 1

    assert constants[0].name == 'UNSIGNED_LONG_CONSTANT'
    assert isinstance(constants[0].type, BasicType)
    assert constants[0].type.typename == 'uint32'
    assert constants[0].value == 42


@pytest.fixture(scope='module')
def action_idl_file():
    return parse_idl_file(ACTION_IDL_LOCATOR)


def test_action_parser(action_idl_file):
    actions = action_idl_file.content.get_elements_of_type(Action)
    assert len(actions) == 1

    action = actions[0]
    assert isinstance(action, Action)
    assert action.namespaced_type.namespaces == ['rosidl_parser', 'action']
    assert action.namespaced_type.name == 'MyAction'

    # check messages defined in the idl file
    constants = action.goal.constants
    assert len(constants) == 1
    assert constants[0].name == 'SHORT_CONSTANT'
    assert isinstance(constants[0].type, BasicType)
    assert constants[0].type.typename == 'int16'
    assert constants[0].value == -23

    structure = action.goal.structure
    assert structure.namespaced_type.namespaces == ['rosidl_parser', 'action']
    assert structure.namespaced_type.name == 'MyAction_Goal'
    assert len(structure.members) == 1
    assert isinstance(structure.members[0].type, BasicType)
    assert structure.members[0].type.typename == 'int32'
    assert structure.members[0].name == 'input_value'

    constants = action.result.constants
    assert len(constants) == 1
    assert constants[0].name == 'UNSIGNED_LONG_CONSTANT'
    assert isinstance(constants[0].type, BasicType)
    assert constants[0].type.typename == 'uint32'
    assert constants[0].value == 42

    structure = action.result.structure
    assert structure.namespaced_type.namespaces == ['rosidl_parser', 'action']
    assert structure.namespaced_type.name == 'MyAction_Result'
    assert len(structure.members) == 1
    assert isinstance(structure.members[0].type, BasicType)
    assert structure.members[0].type.typename == 'uint32'
    assert structure.members[0].name == 'output_value'

    constants = action.feedback.constants
    assert len(constants) == 1
    assert constants[0].name == 'FLOAT_CONSTANT'
    assert isinstance(constants[0].type, BasicType)
    assert constants[0].type.typename == 'float'
    assert constants[0].value == 1.25

    structure = action.feedback.structure
    assert structure.namespaced_type.namespaces == ['rosidl_parser', 'action']
    assert structure.namespaced_type.name == 'MyAction_Feedback'
    assert len(structure.members) == 1
    assert isinstance(structure.members[0].type, BasicType)
    assert structure.members[0].type.typename == 'float'
    assert structure.members[0].name == 'progress_value'

    # check derived goal service
    namespaced_type = action.send_goal_service.namespaced_type
    assert namespaced_type.namespaces == ['rosidl_parser', 'action']
    assert namespaced_type.name == 'MyAction_SendGoal'

    structure = action.send_goal_service.request_message.structure
    assert len(structure.members) == 2

    assert isinstance(structure.members[0].type, NamespacedType)
    assert structure.members[0].type.namespaces == [
        'unique_identifier_msgs', 'msg']
    assert structure.members[0].type.name == 'UUID'
    assert structure.members[0].name == 'goal_id'

    assert isinstance(structure.members[1].type, NamespacedType)
    assert structure.members[1].type.namespaces == \
        action.goal.structure.namespaced_type.namespaces
    assert structure.members[1].type.name == \
        action.goal.structure.namespaced_type.name

    structure = action.send_goal_service.response_message.structure
    assert len(structure.members) == 2

    assert isinstance(structure.members[0].type, BasicType)
    assert structure.members[0].type.typename == 'boolean'
    assert structure.members[0].name == 'accepted'

    assert isinstance(structure.members[1].type, NamespacedType)
    assert structure.members[1].type.namespaces == [
        'builtin_interfaces', 'msg']
    assert structure.members[1].type.name == 'Time'
    assert structure.members[1].name == 'stamp'

    # check derived result service
    namespaced_type = action.get_result_service.namespaced_type
    assert namespaced_type.namespaces == ['rosidl_parser', 'action']
    assert namespaced_type.name == 'MyAction_GetResult'

    structure = action.get_result_service.request_message.structure
    assert len(structure.members) == 1

    assert isinstance(structure.members[0].type, NamespacedType)
    assert structure.members[0].type.namespaces == [
        'unique_identifier_msgs', 'msg']
    assert structure.members[0].type.name == 'UUID'
    assert structure.members[0].name == 'goal_id'

    structure = action.get_result_service.response_message.structure
    assert len(structure.members) == 2

    assert isinstance(structure.members[0].type, BasicType)
    assert structure.members[0].type.typename == 'int8'
    assert structure.members[0].name == 'status'

    assert isinstance(structure.members[1].type, NamespacedType)
    assert structure.members[1].type.namespaces == \
        action.result.structure.namespaced_type.namespaces
    assert structure.members[1].type.name == \
        action.result.structure.namespaced_type.name

    # check derived feedback message
    structure = action.feedback_message.structure
    assert structure.namespaced_type.namespaces == ['rosidl_parser', 'action']
    assert structure.namespaced_type.name == 'MyAction_FeedbackMessage'

    assert len(structure.members) == 2

    assert isinstance(structure.members[0].type, NamespacedType)
    assert structure.members[0].type.namespaces == [
        'unique_identifier_msgs', 'msg']
    assert structure.members[0].type.name == 'UUID'
    assert structure.members[0].name == 'goal_id'

    assert isinstance(structure.members[1].type, NamespacedType)
    assert structure.members[1].type.namespaces == \
        action.feedback.structure.namespaced_type.namespaces
    assert structure.members[1].type.name == \
        action.feedback.structure.namespaced_type.name
