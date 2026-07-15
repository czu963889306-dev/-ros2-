# Copyright 2016 Open Source Robotics Foundation, Inc.
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

from test_msgs.msg import Arrays
from test_msgs.msg import BasicTypes
from test_msgs.msg import BoundedPlainSequences
from test_msgs.msg import BoundedSequences
from test_msgs.msg import Builtins
from test_msgs.msg import Constants
from test_msgs.msg import Defaults
from test_msgs.msg import Empty
from test_msgs.msg import MultiNested
from test_msgs.msg import Nested
from test_msgs.msg import Strings
from test_msgs.msg import UnboundedSequences
from test_msgs.msg import WStrings


def int_from_uint(value, nbits):
    value = value % (1 << nbits)
    if value >= (1 << (nbits - 1)):
        value = value - (1 << nbits)
    return value


def get_msg_builtins():
    msg = Builtins()
    msg.duration_value.sec = -1234567890
    msg.duration_value.nanosec = 123456789
    msg.time_value.sec = -1234567890
    msg.time_value.nanosec = 987654321

    return [msg]


def get_msg_empty():
    msg = Empty()

    return [msg]


def get_msg_basic_types():
    msgs = []
    msg = BasicTypes()
    msg.bool_value = False
    msg.byte_value = bytes([0])
    msg.char_value = 0
    msg.float32_value = float(0.0)
    msg.float64_value = float(0.0)
    msg.int8_value = 0
    msg.uint8_value = 0
    msg.int16_value = 0
    msg.uint16_value = 0
    msg.int32_value = 0
    msg.uint32_value = 0
    msg.int64_value = 0
    msg.uint64_value = 0
    msgs.append(msg)

    msg = BasicTypes()
    msg.bool_value = True
    msg.byte_value = bytes([255])
    msg.char_value = 255
    msg.float32_value = 1.125
    msg.float64_value = 1.125
    msg.int8_value = 127
    msg.uint8_value = 255
    msg.int16_value = 32767
    msg.uint16_value = 65535
    msg.int32_value = 2147483647
    msg.uint32_value = 4294967295
    msg.int64_value = 9223372036854775807
    msg.uint64_value = 18446744073709551615
    msgs.append(msg)

    msg = BasicTypes()
    msg.bool_value = False
    msg.byte_value = bytes([0])
    msg.char_value = 0
    msg.float32_value = -2.125
    msg.float64_value = -2.125
    msg.int8_value = -128
    msg.uint8_value = 0
    msg.int16_value = -32768
    msg.uint16_value = 0
    msg.int32_value = -2147483648
    msg.uint32_value = 0
    msg.int64_value = -9223372036854775808
    msg.uint64_value = 0
    msgs.append(msg)

    msg = BasicTypes()
    msg.bool_value = True
    msg.byte_value = bytes([1])
    msg.char_value = 1
    msg.float32_value = float(1.0)
    msg.float64_value = float(1.0)
    msg.int8_value = 1
    msg.uint8_value = 1
    msg.int16_value = 1
    msg.uint16_value = 1
    msg.int32_value = 1
    msg.uint32_value = 1
    msg.int64_value = 1
    msg.uint64_value = 1
    msgs.append(msg)

    return msgs


def get_msg_constants():
    msg = Constants()
    return [msg]


def get_msg_defaults():
    msg = Defaults()
    return [msg]


def get_msg_strings():
    msgs = []
    msg = Strings()
    msg.string_value = ''
    msg.bounded_string_value = ''
    msgs.append(msg)

    msg = Strings()
    msg.string_value = 'Hello world!'
    msg.bounded_string_value = 'Hello world!'
    msgs.append(msg)

    msg = Strings()
    msg.string_value = 'Hellö Wörld!'
    msg.bounded_string_value = 'Hellö Wörld!'
    msgs.append(msg)

    msg = Strings()
    msg.string_value = ''
    # check strings longer then 255 characters
    for i in range(20000):
        msg.string_value += str(i % 10)
    msg.bounded_string_value = ''
    for i in range(22):
        msg.bounded_string_value += str(i % 10)
    msgs.append(msg)

    return msgs


def get_msg_nested():
    msgs = []

    basic_types_msgs = get_msg_basic_types()
    for basic_types_msg in basic_types_msgs:
        msg = Nested()
        msg.basic_types_value = basic_types_msg
        msgs.append(msg)

    return msgs


def get_msg_arrays():
    basic_types_msgs = get_msg_basic_types()
    msgs = []
    msg = Arrays()
    msg.bool_values = [False, True, False]
    msg.char_values = [0, 255, 0]
    msg.byte_values = [bytes([0]), bytes([255]), bytes([0])]
    msg.float32_values = [0.0, 1.125, -2.125]
    msg.float64_values = [0.0, 1.125, -2.125]
    msg.int8_values = [0, 127, -128]
    msg.uint8_values = [0, 255, 0]
    msg.int16_values = [0, 32767, -32768]
    msg.uint16_values = [0, 65535, 0]
    msg.int32_values = [0, 2147483647, -2147483648]
    msg.uint32_values = [0, 4294967295, 0]
    msg.int64_values = [0, 9223372036854775807, -9223372036854775808]
    msg.uint64_values = [0, 18446744073709551615, 0]
    msg.string_values = ['', 'max value', 'min value']
    for i in range(len(msg.basic_types_values)):
        msg.basic_types_values[i] = basic_types_msgs[i]
    msgs.append(msg)

    return msgs


def get_msg_unbounded_sequences():
    basic_types_msgs = get_msg_basic_types()
    msgs = []
    msg = UnboundedSequences()
    msg.bool_values = []
    msg.char_values = []
    msg.byte_values = []
    msg.float32_values = []
    msg.float64_values = []
    msg.int8_values = []
    msg.uint8_values = []
    msg.int16_values = []
    msg.uint16_values = []
    msg.int32_values = []
    msg.uint32_values = []
    msg.int64_values = []
    msg.uint64_values = []
    msg.string_values = []
    msg.basic_types_values = []
    msg.alignment_check = 0
    msgs.append(msg)

    msg = UnboundedSequences()
    msg.bool_values = [True]
    msg.byte_values = [bytes([255])]
    msg.char_values = [255]
    msg.float32_values = [1.125]
    msg.float64_values = [1.125]
    msg.int8_values = [127]
    msg.uint8_values = [255]
    msg.int16_values = [32767]
    msg.uint16_values = [65535]
    msg.int32_values = [2147483647]
    msg.uint32_values = [4294967295]
    msg.int64_values = [9223372036854775807]
    msg.uint64_values = [18446744073709551615]
    msg.string_values = ['max value']
    msg.basic_types_values = [basic_types_msgs[0]]
    msg.alignment_check = 1
    msgs.append(msg)

    msg = UnboundedSequences()
    msg.bool_values = [False, True]
    msg.byte_values = [bytes([0]), bytes([255])]
    msg.char_values = [0, 255]
    msg.float32_values = [0.0, 1.125, -2.125]
    msg.float64_values = [0.0, 1.125, -2.125]
    msg.int8_values = [0, 127, -128]
    msg.uint8_values = [0, 255]
    msg.int16_values = [0, 32767, -32768]
    msg.uint16_values = [0, 65535]
    msg.int32_values = [0, 2147483647, -2147483648]
    msg.uint32_values = [0, 4294967295]
    msg.int64_values = [0, 9223372036854775807, -9223372036854775808]
    msg.uint64_values = [0, 18446744073709551615]
    msg.string_values = ['', 'max value', 'optional min value']
    msg.basic_types_values = [basic_types_msgs[i % len(basic_types_msgs)] for i in range(3)]
    msg.alignment_check = 2
    msgs.append(msg)

    size = 1000

    msg = UnboundedSequences()
    msg.bool_values = [i % 2 != 0 for i in range(size)]
    msg.byte_values = [bytes([i % (1 << 8)]) for i in range(size)]
    msg.char_values = [i % (1 << 8) for i in range(size)]
    msg.float32_values = [float(1.125 * i) for i in range(size)]
    msg.float64_values = [1.125 * i for i in range(size)]
    msg.int8_values = [int_from_uint(i, 8) for i in range(size)]
    msg.uint8_values = [i % (1 << 8) for i in range(size)]
    msg.int16_values = [int_from_uint(i, 16) for i in range(size)]
    msg.uint16_values = [i % (1 << 16) for i in range(size)]
    msg.int32_values = [int_from_uint(i, 32) for i in range(size)]
    msg.uint32_values = [i % (1 << 32) for i in range(size)]
    msg.int64_values = [int_from_uint(i, 64) for i in range(size)]
    msg.uint64_values = [i % (1 << 64) for i in range(size)]
    msg.string_values = [str(i) for i in range(size)]
    msg.basic_types_values = [basic_types_msgs[i % len(basic_types_msgs)] for i in range(size)]
    msg.alignment_check = 3
    msgs.append(msg)

    msg = UnboundedSequences()
    msg.alignment_check = 4
    msgs.append(msg)

    return msgs


def get_msg_bounded_sequences():
    basic_types_msgs = get_msg_basic_types()
    msgs = []
    msg = BoundedSequences()
    msg.bool_values = [False, True, False]
    msg.byte_values = [bytes([0]), bytes([1]), bytes([255])]
    msg.char_values = [0, 1, 255]
    msg.float32_values = [0.0, 1.125, -2.125]
    msg.float64_values = [0.0, 1.125, -2.125]
    msg.int8_values = [0, 127, -128]
    msg.uint8_values = [0, 1, 255]
    msg.int16_values = [0, 32767, -32768]
    msg.uint16_values = [0, 1, 65535]
    msg.int32_values = [0, 2147483647, -2147483648]
    msg.uint32_values = [0, 1, 4294967295]
    msg.int64_values = [0, 9223372036854775807, -9223372036854775808]
    msg.uint64_values = [0, 1, 18446744073709551615]
    msg.string_values = ['', 'max value', 'optional min value']
    msg.basic_types_values = [basic_types_msgs[i % len(basic_types_msgs)] for i in range(3)]
    msg.alignment_check = 2
    msgs.append(msg)

    msg = BoundedSequences()
    msg.alignment_check = 4
    msgs.append(msg)

    return msgs


def get_msg_bounded_plain_sequences():
    basic_types_msgs = get_msg_basic_types()
    msgs = []
    msg = BoundedPlainSequences()
    msg.bool_values = [False, True, False]
    msg.byte_values = [bytes([0]), bytes([1]), bytes([255])]
    msg.char_values = [0, 1, 255]
    msg.float32_values = [0.0, 1.125, -2.125]
    msg.float64_values = [0.0, 1.125, -2.125]
    msg.int8_values = [0, 127, -128]
    msg.uint8_values = [0, 1, 255]
    msg.int16_values = [0, 32767, -32768]
    msg.uint16_values = [0, 1, 65535]
    msg.int32_values = [0, 2147483647, -2147483648]
    msg.uint32_values = [0, 1, 4294967295]
    msg.int64_values = [0, 9223372036854775807, -9223372036854775808]
    msg.uint64_values = [0, 1, 18446744073709551615]
    msg.basic_types_values = [basic_types_msgs[i % len(basic_types_msgs)] for i in range(3)]
    msg.alignment_check = 2
    msgs.append(msg)

    msg = BoundedPlainSequences()
    msg.alignment_check = 4
    msgs.append(msg)

    return msgs


def get_msg_multi_nested():
    arrays_msgs = get_msg_arrays()
    bounded_sequences_msgs = get_msg_bounded_sequences()
    unbounded_sequences_msgs = get_msg_unbounded_sequences()
    msg = MultiNested()
    msg.array_of_arrays = [arrays_msgs[i % len(arrays_msgs)] for i in range(3)]
    msg.array_of_bounded_sequences = [
        bounded_sequences_msgs[i % len(bounded_sequences_msgs)]
        for i in range(3)]
    msg.array_of_unbounded_sequences = [
        unbounded_sequences_msgs[i % len(unbounded_sequences_msgs)]
        for i in range(3)]
    msg.bounded_sequence_of_arrays = [arrays_msgs[i % len(arrays_msgs)] for i in range(3)]
    msg.bounded_sequence_of_bounded_sequences = [
        bounded_sequences_msgs[i % len(bounded_sequences_msgs)]
        for i in range(3)]
    msg.bounded_sequence_of_unbounded_sequences = [
        unbounded_sequences_msgs[i % len(unbounded_sequences_msgs)]
        for i in range(3)]
    msg.unbounded_sequence_of_arrays = [arrays_msgs[i % len(arrays_msgs)] for i in range(3)]
    msg.unbounded_sequence_of_bounded_sequences = [
        bounded_sequences_msgs[i % len(bounded_sequences_msgs)]
        for i in range(3)]
    msg.unbounded_sequence_of_unbounded_sequences = [
        unbounded_sequences_msgs[i % len(unbounded_sequences_msgs)]
        for i in range(3)]
    return [msg]


def get_msg_wstrings():
    msgs = []

    msg = WStrings()
    msg.wstring_value = ''
    msg.array_of_wstrings = ['1', 'two', '三']
    msg.bounded_sequence_of_wstrings = ['one', '二']
    msg.unbounded_sequence_of_wstrings = ['.', '..', '...', '四']
    msgs.append(msg)

    msg = WStrings()
    msg.wstring_value = 'ascii'
    msgs.append(msg)

    msg = WStrings()
    msg.wstring_value = 'Hellö Wörld!'
    msgs.append(msg)

    msg = WStrings()
    msg.wstring_value = 'ハローワールド'  # Hello world in Japanese
    msgs.append(msg)

    return msgs


def get_test_msg(message_name):
    if 'Builtins' == message_name:
        msg = get_msg_builtins()
    elif 'Empty' == message_name:
        msg = get_msg_empty()
    elif 'BasicTypes' == message_name:
        msg = get_msg_basic_types()
    elif 'Constants' == message_name:
        msg = get_msg_constants()
    elif 'Defaults' == message_name:
        msg = get_msg_defaults()
    elif 'Strings' == message_name:
        msg = get_msg_strings()
    elif 'Nested' == message_name:
        msg = get_msg_nested()
    elif 'Arrays' == message_name:
        msg = get_msg_arrays()
    elif 'BoundedPlainSequences' == message_name:
        msg = get_msg_bounded_plain_sequences()
    elif 'BoundedSequences' == message_name:
        msg = get_msg_bounded_sequences()
    elif 'UnboundedSequences' == message_name:
        msg = get_msg_unbounded_sequences()
    elif 'MultiNested' == message_name:
        msg = get_msg_multi_nested()
    elif 'WStrings' == message_name:
        msg = get_msg_wstrings()
    else:
        raise NotImplementedError
    return msg
