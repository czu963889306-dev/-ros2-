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

# TODO(jacobperron): Rename functions to match C++ equivalents
from test_msgs.message_fixtures import get_msg_basic_types as get_messages_basic_types
from test_msgs.srv import Arrays
from test_msgs.srv import BasicTypes
from test_msgs.srv import Empty


def get_msg_empty():
    req = Empty.Request()
    resp = Empty.Response()

    return [[req, resp]]


def get_msg_basic_types():
    srvs = []
    req = BasicTypes.Request()
    req.bool_value = False
    req.byte_value = bytes([0])
    req.char_value = 0
    req.float32_value = float(0.0)
    req.float64_value = float(0.0)
    req.int8_value = 0
    req.uint8_value = 0
    req.int16_value = 0
    req.uint16_value = 0
    req.int32_value = 0
    req.uint32_value = 0
    req.int64_value = 0
    req.uint64_value = 0
    req.string_value = 'request'
    resp = BasicTypes.Response()
    resp.bool_value = False
    resp.byte_value = bytes([0])
    resp.char_value = 0
    resp.float32_value = float(0.0)
    resp.float64_value = float(0.0)
    resp.int8_value = 0
    resp.uint8_value = 0
    resp.int16_value = 0
    resp.uint16_value = 0
    resp.int32_value = 0
    resp.uint32_value = 0
    resp.int64_value = 0
    resp.uint64_value = 0
    resp.string_value = 'reply'
    srvs.append([req, resp])

    req = BasicTypes.Request()
    req.bool_value = True
    req.byte_value = bytes([1])
    req.char_value = 1
    req.float32_value = float(1.125)
    req.float64_value = float(1.11)
    req.int8_value = 1
    req.uint8_value = 2
    req.int16_value = 3
    req.uint16_value = 4
    req.int32_value = 5
    req.uint32_value = 6
    req.int64_value = 7
    req.uint64_value = 8
    # check strings longer then 256 characters
    req.string_value = ''
    for i in range(20000):
        req.string_value += str(i % 10)
    resp = BasicTypes.Response()
    resp.bool_value = True
    resp.byte_value = bytes([11])
    resp.char_value = 11
    resp.float32_value = float(11.125)
    resp.float64_value = float(11.11)
    resp.int8_value = 11
    resp.uint8_value = 22
    resp.int16_value = 33
    resp.uint16_value = 44
    resp.int32_value = 55
    resp.uint32_value = 66
    resp.int64_value = 77
    resp.uint64_value = 88
    # check strings longer then 256 characters
    resp.string_value = ''
    for i in range(20000):
        resp.string_value += str(i % 10)
    srvs.append([req, resp])
    return srvs


def get_msg_arrays():
    srvs = []
    req = Arrays.Request()
    req.bool_values = [False, True, False]
    req.char_values = [0, 255, 0]
    req.byte_values = [bytes([0]), bytes([255]), bytes([0])]
    req.float32_values = [0.0, 1.125, -2.125]
    req.float64_values = [0.0, 1.125, -2.125]
    req.int8_values = [0, 127, -128]
    req.uint8_values = [0, 255, 0]
    req.int16_values = [0, 32767, -32768]
    req.uint16_values = [0, 65535, 0]
    req.int32_values = [0, 2147483647, -2147483648]
    req.uint32_values = [0, 4294967295, 0]
    req.int64_values = [0, 9223372036854775807, -9223372036854775808]
    req.uint64_values = [0, 18446744073709551615, 0]
    req.string_values = ['', 'max value', 'min value']
    basic_types_msgs = get_messages_basic_types()
    req.basic_types_values[0] = basic_types_msgs[0]
    req.basic_types_values[1] = basic_types_msgs[1]
    req.basic_types_values[2] = basic_types_msgs[2]

    resp = Arrays.Response()
    resp.bool_values = [True, False, False]
    resp.char_values = [255, 0, 0]
    resp.byte_values = [bytes([255]), bytes([0]), bytes([0])]
    resp.float32_values = [1.125, 0.0, -2.125]
    resp.float64_values = [1.125, 0.0, -2.125]
    resp.int8_values = [127, 0, -128]
    resp.uint8_values = [255, 0, 0]
    resp.int16_values = [32767, 0, -32768]
    resp.uint16_values = [65535, 0, 0]
    resp.int32_values = [2147483647, 0, -2147483648]
    resp.uint32_values = [4294967295, 0, 0]
    resp.int64_values = [9223372036854775807, 0, -9223372036854775808]
    resp.uint64_values = [18446744073709551615, 0, 0]
    resp.string_values = ['max value', '', 'min value']
    resp.basic_types_values[0] = basic_types_msgs[1]
    resp.basic_types_values[1] = basic_types_msgs[0]
    resp.basic_types_values[2] = basic_types_msgs[2]

    srvs.append([req, resp])
    return srvs


def get_test_srv(service_name):
    if 'Arrays' == service_name:
        srv = get_msg_arrays()
    elif 'BasicTypes' == service_name:
        srv = get_msg_basic_types()
    elif 'Empty' == service_name:
        srv = get_msg_empty()
    else:
        raise NotImplementedError('%s service is not part of the test suite', service_name)
    return srv
