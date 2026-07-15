# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
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

import argparse
import codecs
import errno
import locale
import os
import re
import subprocess
import sys
import time
from xml.etree.ElementTree import ElementTree
from xml.etree.ElementTree import ParseError
from xml.sax.saxutils import quoteattr


def separate_env_vars(env_str, env_argument_name, parser):
    try:
        index = env_str.index('=')
    except ValueError:
        parser.error("--%s argument '%s' contains no equal sign" % (env_argument_name, env_str))
    key = env_str[0:index]
    value = env_str[index + 1:]
    return key, value


def return_code(value):
    value = int(value)
    if value < 0 or value > 255:
        raise argparse.ArgumentTypeError(
            'Return code must be less than 256 and greater or equal to 0')
    return value


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description='Run the test command passed as an argument and ensures'
                    'that the expected result file is generated.')
    parser.add_argument(
        'result_file', help='The path to the xunit result file')
    parser.add_argument(
        '--package-name',
        help="The package name to be used as a prefix for the 'classname' "
             'attributes in gtest result files')
    parser.add_argument(
        '--command',
        nargs='+',
        help='The test command to execute. '
             'It must be passed after other arguments since it collects all '
             'following options.')
    parser.add_argument(
        '--env',
        nargs='+',
        help='Extra environment variables to set when running, e.g. FOO=foo BAR=bar')
    parser.add_argument(
        '--append-env',
        nargs='+',
        help='Extra environment variables to append, or set, when running, e.g. FOO=foo BAR=bar')
    parser.add_argument(
        '--output-file',
        help='The path to the output log file')
    parser.add_argument(
        '--generate-result-on-success',
        action='store_true',
        default=False,
        help='Generate a result file if the command returns with code zero')
    parser.add_argument(
        '--skip-test',
        action='store_true',
        default=False,
        help='Skip the test')
    parser.add_argument(
        '--skip-return-code',
        type=return_code,
        help="If the test returns this value and doesn't generate a result file, "
             'create one stating that the test was skipped.')

    if '--command' in argv:
        index = argv.index('--command')
        argv, command = argv[0:index + 1] + ['dummy'], argv[index + 1:]
    args = parser.parse_args(argv)
    args.command = command

    # if result file exists remove it before test execution
    if os.path.exists(args.result_file):
        os.remove(args.result_file)

    # create folder if necessary
    if not os.path.exists(os.path.dirname(args.result_file)):
        try:
            os.makedirs(os.path.dirname(args.result_file))
        except OSError as e:
            # catch case where folder has been created in the mean time
            if e.errno != errno.EEXIST:
                raise

    if args.skip_test:
        # generate a skipped test result file
        skipped_result_file = _generate_result(args.result_file, skip=True)
        with open(args.result_file, 'w') as h:
            h.write(skipped_result_file)
        return 0

    # generate result file with one failed test
    # in case the command segfaults or timeouts and does not generate one
    failure_result_file = _generate_result(
        args.result_file,
        error_message='The test did not generate a result file.')
    with open(args.result_file, 'w') as h:
        h.write(failure_result_file)

    # collect output / exception to generate more detailed result file
    # if the command fails to generate it
    output_handle = None
    if args.output_file:
        output_path = os.path.dirname(args.output_file)
        os.makedirs(output_path, exist_ok=True)
        output_handle = open(args.output_file, 'wb')

    try:
        return _run_test(parser, args, failure_result_file, output_handle)
    finally:
        if output_handle:
            output_handle.close()


def _run_test(parser, args, failure_result_file, output_handle):
    output = ''

    def log(msg, **kwargs):
        print(msg, **kwargs)
        if output_handle:
            output_handle.write((msg + '\n').encode())
            output_handle.flush()

    env = None
    if args.env or args.append_env:
        env = dict(os.environ)
        if args.env:
            log('-- run_test.py: extra environment variables:')
            previous_key = None
            updated_env_keys = set()
            for env_str in args.env:
                # if CMake has split a single value containing semicolons
                # into multiple arguments they are put back together here
                if previous_key and '=' not in env_str:
                    key = previous_key
                    value = env[key] + ';' + env_str
                else:
                    key, value = separate_env_vars(env_str, 'env', parser)
                env[key] = value
                updated_env_keys.add(key)
                previous_key = key
            for key in sorted(updated_env_keys):
                log(' - {0}={1}'.format(key, env[key]))
        if args.append_env:
            log('-- run_test.py: extra environment variables to append:')
            previous_key = None
            for env_str in args.append_env:
                # if CMake has split a single value containing semicolons
                # into multiple arguments they are put back together here
                if previous_key and '=' not in env_str:
                    key = previous_key
                    value = env[key] + ';' + env_str
                    log(' - {0}+={1}'.format(key, env_str))
                else:
                    key, value = separate_env_vars(env_str, 'append-env', parser)
                    log(' - {0}+={1}'.format(key, value))
                if key not in env:
                    env[key] = ''
                if not env[key].endswith(os.pathsep):
                    env[key] += os.pathsep
                env[key] += value
                previous_key = key

    log("-- run_test.py: invoking following command in '%s':\n - %s" %
        (os.getcwd(), ' '.join(args.command)))
    if output_handle:
        output_handle.write('\n'.encode())
        output_handle.flush()

    encodings = ['utf-8']
    if locale.getpreferredencoding(False) not in encodings:
        encodings.append(locale.getpreferredencoding(False))

    start_time = time.monotonic()

    try:
        proc = subprocess.Popen(
            args.command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            env=env)
        while True:
            line = proc.stdout.readline()
            if not line:
                break
            for i, encoding in enumerate(encodings):
                try:
                    decoded_line = line.decode(encoding)
                except UnicodeDecodeError:
                    if i == len(encodings) - 1:
                        decoded_line = line.decode(encoding, errors='replace')
                else:
                    break
            print(decoded_line, end='')
            output += decoded_line
            if output_handle:
                output_handle.write(decoded_line.encode())
                output_handle.flush()
        proc.wait()
        rc = proc.returncode
        if output_handle:
            # separate progress of this script from subprocess output
            output_handle.write('\n\n'.encode())
        log('-- run_test.py: return code ' + str(rc), file=sys.stderr if rc else sys.stdout)
    except Exception as e:
        if output_handle:
            # separate subprocess output from progress of this script
            output_handle.write('\n\n'.encode())
        log('-- run_test.py: invocation failed: ' + str(e), file=sys.stderr)
        output += str(e)
        rc = 1

    test_time = time.monotonic() - start_time

    if not rc and args.generate_result_on_success:
        # generate result file with one passed test
        # if it was expected that no result file was generated
        # and the command returned with code zero
        log("-- run_test.py: generate result file '%s' with successful test" % args.result_file)
        success_result_file = _generate_result(args.result_file, test_time=test_time)
        with open(args.result_file, 'w') as h:
            h.write(success_result_file)

    elif os.path.exists(args.result_file):
        # check if content of result file has actually changed
        with open(args.result_file, 'r', encoding='utf-8') as h:
            content = h.read()

        if content == failure_result_file:
            if args.skip_return_code is not None and args.skip_return_code == rc:
                log(
                    '-- run_test.py: generate result file '
                    "'%s' with skipped test" % args.result_file)
                # regenerate result file to indicate that the test was skipped
                result_file = _generate_result(args.result_file, skip=True, test_time=test_time)
            else:
                log(
                    '-- run_test.py: generate result file '
                    "'%s' with failed test" % args.result_file, file=sys.stderr)
                # regenerate result file to include output / exception of the invoked command
                result_file = _generate_result(
                    args.result_file,
                    error_message='The test did not generate a result file:\n\n' + output,
                    test_time=test_time)
            with open(args.result_file, 'w') as h:
                h.write(result_file)
        else:
            # prefix classname attributes
            if args.result_file.endswith('.gtest.xml') and args.package_name:
                prefix = ' classname="'
                pattern = '%s(?!%s)' % (prefix, args.package_name)
                new_content = re.sub(
                    pattern, prefix + args.package_name + '.', content)
                if new_content != content:
                    log(
                        '-- run_test.py: inject classname prefix into gtest '
                        "result file '%s'" % args.result_file)
                    with open(args.result_file, 'w') as h:
                        h.write(new_content)

        log("-- run_test.py: verify result file '%s'" % args.result_file)
        # if result file exists ensure that it contains valid xml
        # unit test suites are not good about screening out
        # illegal unicode characters
        tree = None
        try:
            tree = ElementTree(None, args.result_file)
        except ParseError as e:
            modified = _tidy_xml(args.result_file)
            if not modified:
                log("Invalid XML in result file '%s': %s" %
                    (args.result_file, str(e)), file=sys.stderr)
            else:
                try:
                    tree = ElementTree(None, args.result_file)
                except ParseError as e:
                    log("Invalid XML in result file '%s' (even after trying to tidy it): %s" %
                        (args.result_file, str(e)), file=sys.stderr)

        if not tree:
            # set error code when result file is not parsable
            rc = 1
        else:
            # set error code when result file contains errors or failures
            root = tree.getroot()
            num_errors = int(root.attrib.get('errors', 0))
            num_failures = int(root.attrib.get('failures', 0))
            if num_errors or num_failures:
                rc = 1

    # ensure that a result file exists at the end
    if not rc and not os.path.exists(args.result_file):
        log('-- run_test.py: override return code since no result file was '
            'generated', file=sys.stderr)
        rc = 1

    return rc


def _generate_result(result_file, *, failure_message=None, skip=False,
                     error_message=None, test_time=0):
    # the generated result file must be readable
    # by any of the Jenkins test result report publishers
    pkgname = os.path.basename(os.path.dirname(result_file))
    testname = os.path.splitext(os.path.basename(result_file))[0]
    error_message = '<error message=%s/>' % quoteattr(error_message) \
        if error_message else ''
    failure_message = '<failure message=%s/>' % quoteattr(failure_message) \
        if failure_message else ''
    skipped_message = \
        '<skipped type="skip" message="">![CDATA[Test Skipped by developer]]</skipped>' \
        if skip else ''
    return """<?xml version="1.0" encoding="UTF-8"?>
<testsuite name="%s" tests="1" failures="%d" time="%.3f" errors="%d" skipped="%d">
  <testcase classname="%s" name="%s.missing_result" time="%.3f">
    %s%s%s
  </testcase>
</testsuite>\n""" % \
        (
            pkgname,
            1 if failure_message else 0,
            test_time,
            1 if error_message else 0,
            1 if skip else 0,
            pkgname, testname, test_time,
            failure_message, skipped_message, error_message
        )


def _tidy_xml(filename):
    assert os.path.isfile(filename)

    # try reading utf-8 first then iso
    # this is ugly but the files in question do not declare a unicode type
    data = None
    for encoding in ['utf-8', 'iso8859-1']:
        f = None
        try:
            f = codecs.open(filename, 'r', encoding)
            data = f.read()
            break
        except ValueError:
            continue
        finally:
            if f:
                f.close()

    if data is None:
        return False

    try:
        char = unichr
    except NameError:
        char = chr
    re_xml_illegal = (
        '([%s-%s%s-%s%s-%s%s-%s])' +
        '|' +
        '([%s-%s][^%s-%s])|([^%s-%s][%s-%s])|([%s-%s]$)|(^[%s-%s])') % \
        (char(0x0000), char(0x0008), char(0x000b), char(0x000c),
         char(0x000e), char(0x001f), char(0xfffe), char(0xffff),
         char(0xd800), char(0xdbff), char(0xdc00), char(0xdfff),
         char(0xd800), char(0xdbff), char(0xdc00), char(0xdfff),
         char(0xd800), char(0xdbff), char(0xdc00), char(0xdfff))
    safe_xml_regex = re.compile(re_xml_illegal)

    for match in safe_xml_regex.finditer(data):
        data = data[:match.start()] + '?' + data[match.end():]

    with open(filename, 'w', encoding='utf-8') as h:
        h.write(data)
    return True
