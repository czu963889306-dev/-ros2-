# Copyright 2015 Open Source Robotics Foundation, Inc.
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

import os
from pathlib import Path, PurePath

from ament_index_python import get_package_prefix
from ament_index_python import get_package_share_directory
from ament_index_python import get_package_share_path
from ament_index_python import get_packages_with_prefixes
from ament_index_python import get_resource
from ament_index_python import get_resource_types
from ament_index_python import get_resources
from ament_index_python import get_search_paths
from ament_index_python import has_resource
from ament_index_python import InvalidResourceNameError
from ament_index_python import InvalidResourceTypeNameError
from ament_index_python import PackageNotFoundError
from ament_index_python.cli import main
from ament_index_python.cli import resource_name_completer
from ament_index_python.cli import resource_type_completer

import pytest


def set_ament_prefix_path(subfolders):
    paths = []
    base_path = Path(__file__).parent
    for subfolder in subfolders:
        path = base_path / subfolder
        if path.is_dir():
            paths.append(str(path))
    ament_prefix_path = os.pathsep.join(paths)
    os.environ['AMENT_PREFIX_PATH'] = ament_prefix_path


def test_empty_search_paths():
    set_ament_prefix_path([])
    with pytest.raises(EnvironmentError):
        get_search_paths()


def test_search_paths():
    set_ament_prefix_path(['prefix1', 'prefix2'])
    search_paths = get_search_paths()
    assert len(search_paths) == 2, 'Expected two search paths'


def test_not_existing_search_paths():
    set_ament_prefix_path(['prefix1', 'not_existing_prefix'])
    search_paths = get_search_paths()
    assert len(search_paths) == 1, 'Expected one search paths'


def test_unknown_resources():
    set_ament_prefix_path(['prefix1'])
    resources = get_resources('unknown_resource_type')
    assert len(resources) == 0, 'Expected no resources'


def test_invalid_resources():
    set_ament_prefix_path(['prefix1'])

    invalid_resource_type_names = [
        '/invalid/name', 'invalid/name', '\\invalid\\name', 'invalid\\name']

    for name in invalid_resource_type_names:
        with pytest.raises(InvalidResourceTypeNameError):
            resources = get_resources(name)
            assert len(resources) == 0, 'Expected no resources'

        with pytest.raises(InvalidResourceTypeNameError):
            exists = has_resource(name, 'example_resource')
            assert not exists, 'Resource should not exist'

        with pytest.raises(InvalidResourceTypeNameError):
            get_resource(name, 'example_resource')


def test_resources():
    set_ament_prefix_path(['prefix1'])
    resources = get_resources('resource_type1')
    assert len(resources) == 2, 'Expected two resources'
    assert set(resources.keys()) == {'foo', 'bar'}, 'Expected different resources'


def test_resources_overlay():
    set_ament_prefix_path(['prefix1', 'prefix2'])
    resources = get_resources('resource_type2')
    assert len(resources) == 2, 'Expected two resource'
    assert set(resources.keys()) == {'foo', 'bar'}, 'Expected different resources'


def test_resources_underlay():
    set_ament_prefix_path(['prefix1', 'prefix2'])
    resources = get_resources('resource_type3')
    assert len(resources) == 1, 'Expected one resource'
    assert set(resources.keys()) == {'bar'}, 'Expected different resources'


def test_unknown_resource():
    set_ament_prefix_path(['prefix1'])
    exists = has_resource('resource_type4', 'bar')
    assert not exists, 'Resource should not exist'

    with pytest.raises(LookupError):
        get_resource('resource_type4', 'bar')


def test_invalid_resource_names():
    set_ament_prefix_path(['prefix1'])

    invalid_resource_names = [
        '/invalid/name', 'invalid/name', '\\invalid\\name', 'invalid\\name']

    for name in invalid_resource_names:
        with pytest.raises(InvalidResourceNameError):
            exists = has_resource('resource_type4', name)
            assert not exists, 'Resource should not exist'

        with pytest.raises(InvalidResourceNameError):
            get_resource('resource_type4', name)


def test_absolute_path_resource():
    extant_absolute_path = os.path.abspath(__file__)

    set_ament_prefix_path(['prefix1'])
    with pytest.raises(InvalidResourceNameError):
        exists = has_resource('resource_type4', str(extant_absolute_path))
        assert not exists, 'Resource should not exist'

    with pytest.raises(InvalidResourceNameError):
        get_resource('resource_type4', str(extant_absolute_path))


def test_resource():
    set_ament_prefix_path(['prefix1'])
    exists = has_resource('resource_type4', 'foo')
    assert exists, 'Resource should exist'

    resource, prefix = get_resource('resource_type4', 'foo')
    assert resource == 'foo', 'Expected different content'
    assert PurePath(prefix).name == 'prefix1', 'Expected different prefix'


def test_resource_overlay():
    set_ament_prefix_path(['prefix1', 'prefix2'])

    resource, prefix = get_resource('resource_type5', 'foo')
    assert resource == 'foo1', 'Expected different content'
    assert PurePath(prefix).name == 'prefix1', 'Expected different prefix'


def test_get_packages_with_prefixes():
    set_ament_prefix_path(['prefix1', 'prefix2'])

    packages = get_packages_with_prefixes()
    assert 'foo' in packages, "Expected to find 'foo'"
    assert PurePath(packages['foo']).name == 'prefix1', "Expected to find 'foo' in 'prefix1'"
    assert 'bar' in packages, "Expected to find 'bar'"
    assert PurePath(packages['bar']).name == 'prefix1', "Expected to find 'bar' in 'prefix1'"
    assert 'baz' in packages, "Expected to find 'baz'"
    assert PurePath(packages['baz']).name == 'prefix2', "Expected to find 'baz' in 'prefix2'"
    os.environ['AMENT_PREFIX_PATH'] = '/path/does/not/exist'

    assert not get_packages_with_prefixes(), 'Expected to find no packages'


def test_get_package_prefix():
    set_ament_prefix_path(['prefix1', 'prefix2'])

    def get_package_prefix_basename(package_name):
        return PurePath(get_package_prefix(package_name)).name

    assert get_package_prefix_basename('foo') == 'prefix1', "Expected 'foo' in 'prefix1'"
    # found in both prefix1 and prefix2, but prefix1 is ahead on the APP
    assert get_package_prefix_basename('bar') == 'prefix1', "Expected 'bar' in 'prefix2'"
    assert get_package_prefix_basename('baz') == 'prefix2', "Expected 'baz' in 'prefix2'"

    with pytest.raises(PackageNotFoundError):
        get_package_prefix('does_not_exist')
    assert issubclass(PackageNotFoundError, KeyError)

    invalid_package_names = [
        '_package', 'package a', 'package/a', 'package.a']
    for name in invalid_package_names:
        with pytest.raises(ValueError):
            get_package_prefix(name)

    with pytest.raises(ValueError):
        # An absolute path is not a valid package name
        extant_absolute_path = os.path.abspath(__file__)
        get_package_prefix(extant_absolute_path)


def test_get_package_share_directory():
    set_ament_prefix_path(['prefix1', 'prefix2'])

    def get_package_share_directory_test(package_name, expect_prefix):
        full_share_dir = get_package_share_directory(package_name)
        left_over, dirname = os.path.split(full_share_dir)
        assert dirname == package_name, f"Expected package name '{package_name}'"
        left_over, dirname = os.path.split(left_over)
        assert dirname == 'share', "Expected 'share'"
        left_over, dirname = os.path.split(left_over)
        assert dirname == expect_prefix, f"Expected '{expect_prefix}'"

    get_package_share_directory_test('foo', 'prefix1')
    # found in both prefix1 and prefix2, but prefix1 is ahead on the APP
    get_package_share_directory_test('bar', 'prefix1')
    get_package_share_directory_test('baz', 'prefix2')

    with pytest.raises(PackageNotFoundError):
        get_package_share_directory('does_not_exist')

    with pytest.raises(ValueError):
        get_package_share_directory('/invalid/package/name')

    with pytest.warns(UserWarning):
        # Package exists, but should print warning because there is no share dir
        get_package_share_directory('trogdor')

    get_package_share_directory('trogdor', print_warning=False)


def test_get_package_share_path():
    set_ament_prefix_path(['prefix1', 'prefix2'])

    def get_package_share_path_test(package_name, expect_prefix):
        my_path = get_package_share_path(package_name)
        assert len(my_path.parts) >= 3
        assert my_path.parts[-1] == package_name, f"Expected package name '{package_name}'"
        assert my_path.parts[-2] == 'share', "Expected 'share'"
        assert my_path.parts[-3] == expect_prefix, f"Expected '{expect_prefix}'"

    get_package_share_path_test('foo', 'prefix1')
    # found in both prefix1 and prefix2, but prefix1 is ahead on the APP
    get_package_share_path_test('bar', 'prefix1')
    get_package_share_path_test('baz', 'prefix2')

    with pytest.raises(PackageNotFoundError):
        get_package_share_path('does_not_exist')

    with pytest.warns(UserWarning):
        # Package exists, but should print warning because there is no share dir
        get_package_share_path('trogdor')

    get_package_share_path('trogdor', print_warning=False)


def test_get_resource_types():
    set_ament_prefix_path([])
    with pytest.raises(EnvironmentError):
        get_resource_types()

    set_ament_prefix_path(['prefix1', 'prefix2'])
    resources = get_resource_types()
    assert resources == {
        'resource_type1',
        'resource_type2',
        'resource_type3',
        'resource_type4',
        'resource_type5',
        'packages'
    }, ('Expected resources to be: resource_type1, resource_type2, resource_type3, '
        'resource_type4, resource_type5 and packages')

    set_ament_prefix_path(['prefix1'])
    resources = get_resource_types()
    assert resources == {
        'resource_type1',
        'resource_type2',
        'resource_type4',
        'resource_type5',
        'packages'
    }, ('Expected resources to be: resource_type1, resource_type2, resource_type4, '
        'resource_type5 and packages')


def test_main_tool(capsys):
    set_ament_prefix_path(['prefix1', 'prefix2'])
    base_path = Path(__file__).parent

    main()
    captured = capsys.readouterr()
    expected_result = (
        'packages\n'
        'resource_type1\n'
        'resource_type2\n'
        'resource_type3\n'
        'resource_type4\n'
        'resource_type5\n'
    )
    assert captured.out == expected_result

    main(argv=['packages'])
    captured = capsys.readouterr()
    expected_result = '\n'.join([
        f"bar\t{base_path / 'prefix1'}",
        f"baz\t{base_path / 'prefix2'}",
        f"foo\t{base_path / 'prefix1'}",
        f"trogdor\t{base_path / 'prefix1'}",
        ''
    ])
    assert captured.out == expected_result

    main(argv=['packages', 'bar'])
    captured = capsys.readouterr()
    expected_result = str(base_path / 'prefix1\n')
    assert captured.out == expected_result

    main(argv=['resource_type4', 'foo'])
    captured = capsys.readouterr()
    expected_result = f"{base_path / 'prefix1'}\n<<<\nfoo\n>>>\n"
    assert captured.out == expected_result

    result = main(argv=['packages', 'not_available'])
    captured = capsys.readouterr()
    expected_result = "Could not find the resource 'not_available' of type 'packages'"
    assert result == expected_result


def test_autocomplete():
    set_ament_prefix_path(['prefix1', 'prefix2'])

    result = sorted(resource_type_completer('res'))
    expected_result = [
        'resource_type1',
        'resource_type2',
        'resource_type3',
        'resource_type4',
        'resource_type5'
    ]
    assert result == expected_result

    class arguments():
        resource_type = 'packages'

    result = sorted(resource_name_completer('ba', arguments))
    expected_result = ['bar', 'baz']
    assert result == expected_result

    setattr(arguments, 'resource_type', None)
    result = sorted(resource_name_completer('ba', arguments))
    expected_result = []
    assert result == expected_result
