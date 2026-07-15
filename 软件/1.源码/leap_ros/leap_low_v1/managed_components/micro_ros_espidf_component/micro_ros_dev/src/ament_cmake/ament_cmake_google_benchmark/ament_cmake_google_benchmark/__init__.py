# Copyright 2020 Open Source Robotics Foundation, Inc.
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
import json
import os
import subprocess
import sys


common_test_properties = {
    'name',
    'run_name',
    'run_type',
    'repetitions',
    'repetition_index',
    'threads',
    'time_unit',
}


common_aggregate_test_properties = common_test_properties | {
    'aggregate_name',
}


common_iteration_test_properties = common_test_properties | {
    'iterations',
    'real_time',
    'cpu_time',
}


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        description='Run a Google Benchmark test and convert the results to '
                    'a common format.')
    parser.add_argument(
        'result_file_in', help='The path to the Google Benchmark result file')
    parser.add_argument(
        'result_file_out',
        help='The path to where the common result file should be written')
    parser.add_argument(
        '--package-name',
        default=None,
        help="The package name to be used as a prefix for the 'group' "
             'value in benchmark result files')
    parser.add_argument(
        '--command',
        nargs='+',
        help='The test command to execute. '
             'It must be passed after other arguments since it collects all '
             'following options.')
    parser.add_argument(
        '--result-file-overlay',
        default=None,
        help='If specified, this json file will be overlaid on top of the '
             'generated results. This is primarily useful for specifying '
             'descriptions and/or thresholds.')
    if '--command' in argv:
        index = argv.index('--command')
        argv, command = argv[0:index + 1] + ['dummy'], argv[index + 1:]
    args = parser.parse_args(argv)
    args.command = command

    print('Executing benchmark test command: %s\n\n' % ' '.join(args.command))
    res = subprocess.run(args.command)
    print('\n\nTest command returned result status %d' % res.returncode)

    try:
        with open(args.result_file_in, 'r') as in_file:
            in_text = in_file.read()
    except FileNotFoundError:
        if res.returncode == 0:
            print(
                'ERROR: No performance test results were found at: %s' % args.result_file_in,
                file=sys.stderr)
            res.returncode = 1
        return res.returncode

    if not in_text and res.returncode == 0:
        print(
            'NOTE: Performance test results file was empty at: %s' % args.result_file_in,
            file=sys.stderr)
        open(args.result_file_out, 'w').close()
        return res.returncode

    try:
        in_data = json.loads(in_text)
    except json.decoder.JSONDecodeError as e:
        print(
            'Failure parsing performance results file at: %s' % args.result_file_in,
            file=sys.stderr)
        print(e)
        if res.returncode == 0:
            res.returncode = 1
        return res.returncode

    overlay_data = None
    if args.result_file_overlay:
        with open(args.result_file_overlay, 'r') as overlay_file:
            overlay_data = json.load(overlay_file)
    out_data = convert_google_benchark_to_jenkins_benchmark(
        in_data, overlay_data, args.package_name)
    with open(args.result_file_out, 'w') as out_file:
        json.dump(out_data, out_file)

    if res.returncode == 0 and any(
            b.get('error_occurred') for b in in_data.get('benchmarks', [])):
        res.returncode = 1

    return res.returncode


def convert_google_benchark_to_jenkins_benchmark(
    in_data,
    overlay_data=None,
    package_name=None,
):
    group_name = os.path.basename(in_data['context']['executable'])
    if package_name:
        group_name = '%s.%s' % (package_name, group_name)

    out_data = {
        group_name: {},
    }
    for benchmark in in_data.get('benchmarks', []):
        benchmark_type = benchmark['run_type']
        if benchmark_type == 'aggregate':
            out_data[group_name][benchmark['name']] = convert_aggregate_benchmark(benchmark)
        elif benchmark_type == 'iteration':
            out_data[group_name][benchmark['name']] = convert_iteration_benchmark(benchmark)
        else:
            print("WARNING: Unsupported benchmark type '%s'" % benchmark_type, file=sys.stderr)

    if not out_data[group_name]:
        print(
            'WARNING: The performance test results file contained no results',
            file=sys.stderr)

    if overlay_data:
        _merge_results(out_data[group_name], overlay_data.get(group_name, {}))

    return out_data


def convert_aggregate_benchmark(in_data):
    out_data = {
        'parameters': {
            'repetitions': {
                'value': in_data['repetitions'],
            },
        },
    }

    value_override = None
    if in_data.get('error_occurred', False):
        value_override = 'failure'

    out_data.update(convert_extra_metrics(
        in_data, common_aggregate_test_properties, value_override))

    return out_data


def convert_iteration_benchmark(in_data):
    out_data = {
        'parameters': {
            'iterations': {
                'value': in_data['iterations'],
            },
            'repetitions': {
                'value': in_data['repetitions'],
            },
        },
        'cpu_time': {
            'unit': in_data['time_unit'],
        },
        'real_time': {
            'unit': in_data['time_unit'],
        },
    }

    value_override = None
    if in_data.get('error_occurred', False):
        value_override = 'failure'
        out_data['cpu_time']['value'] = value_override
        out_data['real_time']['value'] = value_override
    else:
        out_data['cpu_time']['dblValue'] = in_data['cpu_time']
        out_data['real_time']['dblValue'] = in_data['real_time']

    out_data.update(convert_extra_metrics(
        in_data, common_iteration_test_properties, value_override))

    return out_data


def convert_extra_metrics(in_data, properties_to_ignore, value_override=None):
    for k, v in in_data.items():
        if k in properties_to_ignore:
            continue
        if value_override:
            yield k, {
                'value': value_override,
            }
        elif isinstance(v, bool):
            yield k, {
                'boolValue': 'true' if v else 'false',
            }
        elif isinstance(v, (int, float)):
            yield k, {
                'dblValue': v,
            }
        else:
            yield k, {
                'value': v,
            }


def _merge_results(target, overlay):
    for k, v in overlay.items():
        if isinstance(v, dict) and isinstance(target.get(k), dict):
            _merge_results(target[k], v)
        else:
            target[k] = v
