import os
from pathlib import Path
import xml.etree.ElementTree as ET

import ament_mypy.main

import pytest


@pytest.fixture()
def use_dir(tmpdir_factory):
    """Create a sample data directory for testing.

    Directory layout::
    use_me
        |-- me_too
        |    +-- 03.py
        |-- 01.py
        |-- 02.py
        +-- 03.txt

    """
    use_me = tmpdir_factory.mktemp('use_me')
    files = [use_me.join(arg) for arg in ['01.py', '02.py', '03.txt']]
    files.append(use_me.join('me_too', '03.py'))

    for tmp_file in files:
        tmp_file.write('', ensure=True)
    return use_me


@pytest.fixture()
def ignore_dir(tmpdir_factory):
    """Create a sample data directory for testing.

    Directory layout::
    ignore_me
        |-- me_too
        |    +-- 03.py
        |-- 01.py
        |-- 02.py
        |-- 03.txt
        +-- AMENT_IGNORE

    """
    ignore_me = tmpdir_factory.mktemp('ignore_me')
    files = [ignore_me.join(arg) for arg in ['01.py', '02.py', '03.txt', 'AMENT_IGNORE']]
    files.append(ignore_me.join('me_too', '03.py'))

    for tmp_file in files:
        tmp_file.write('', ensure=True)
    return ignore_me


@pytest.fixture()
def mock_mypy_succ(mocker):
    return mocker.patch('ament_mypy.main.mypy.api.run', return_value=('', '', 0))


@pytest.fixture()
def sample_errors(use_dir):
    def filename(slug: str):
        return str(use_dir.join(slug))
    error_line_col = '{}:0:0: error: error message'.format(filename('lc.py'))
    error_line = '{}:0: error: error message'.format(filename('l.py'))
    error_no_pos = '{}: error: error message'.format(filename('no_pos.py'))
    warning = '{}: warning: warning message'.format(filename('warn.py'))
    return [error_line_col, error_line, error_no_pos, warning]


@pytest.fixture()
def mock_mypy_generate_fail(mocker, sample_errors, use_dir):
    mock_fail = mocker.patch('ament_mypy.main._generate_mypy_report')
    mock_fail.return_value = ('\n'.join(sample_errors), '', 1)
    return mock_fail


@pytest.fixture()
def mock_generate_report(mocker):
    return mocker.patch('ament_mypy.main._generate_mypy_report')


def test__generate_mypy_report(mock_mypy_succ):
    # Test if correctly returns mypy output
    files = ['a.py', 'b.py']
    assert ament_mypy.main._generate_mypy_report(files) == mock_mypy_succ.return_value

    # Test if paths were forwarded to mypy
    args, _ = mock_mypy_succ.call_args
    assert all(file_name in args[0] for file_name in files)

    # Test if config file is forwarded to mypy
    assert ament_mypy.main._generate_mypy_report(files, 'a.ini') == mock_mypy_succ.return_value
    args, _ = mock_mypy_succ.call_args
    assert len(args[0]) > 1
    assert any(args[0][i] == '--config-file' and
               args[0][i + 1] == 'a.ini' for i in range(len(args[0]) - 1))

    # Test if setting null cache dir prevents caching
    assert ament_mypy.main._generate_mypy_report(files, cache_dir=os.devnull) \
        == mock_mypy_succ.return_value
    args, _ = mock_mypy_succ.call_args
    assert len(args[0]) > 1
    assert any(args[0][i] == '--cache-dir' and
               args[0][i + 1] == os.devnull for i in range(len(args[0]) - 1))
    assert '--no-incremental' in args[0]

    # Test if non-null cache dir uses caching
    assert ament_mypy.main._generate_mypy_report(files, cache_dir='/tmp') \
        == mock_mypy_succ.return_value
    args, _ = mock_mypy_succ.call_args
    assert len(args[0]) > 1
    assert any(args[0][i] == '--cache-dir' and
               args[0][i + 1] == '/tmp' for i in range(len(args[0]) - 1))
    assert '--no-incremental' not in args[0]


def test_main_success(mock_generate_report, use_dir):
    mock_generate_report.return_value = ('', '', 0)

    # Test that a successful lint returns 0
    assert ament_mypy.main.main([str(use_dir.join('01.py'))]) == 0

    # Sub-test that no other files in directory were checked, too
    args, _ = mock_generate_report.call_args
    assert str(use_dir.join('01.py')) in args[0]
    assert str(use_dir.join('02.py')) not in args[0]
    assert str(use_dir.join('03.txt')) not in args[0]

    # Test that a directory recursively is checked
    assert ament_mypy.main.main([str(use_dir)]) == 0
    args, _ = mock_generate_report.call_args
    assert str(use_dir.join('01.py')) in args[0]
    assert str(use_dir.join('02.py')) in args[0]
    assert str(use_dir.join('03.py')) not in args[0]

    # Test that non-'.py' files were ignored
    assert str(use_dir.join('03.txt')) not in args[0]


def test_main_exclude(mock_generate_report, use_dir):
    mock_generate_report.return_value = ('', '', 0)
    # Test that excluding a file that was passed as an arg works
    assert ament_mypy.main.main([str(use_dir.join('01.py')),
                                 str(use_dir.join('02.py')),
                                 '--exclude',
                                 '02.py']) == 0
    args, _ = mock_generate_report.call_args
    assert str(use_dir.join('01.py')) in args[0]
    assert str(use_dir.join('02.py')) not in args[0]

    # Test that excluding a file when its directory was passed works
    assert ament_mypy.main.main([str(use_dir), '--exclude', '02.py']) == 0
    args, _ = mock_generate_report.call_args
    assert str(use_dir.join('01.py')) in args[0]
    assert str(use_dir.join('02.py')) not in args[0]

    # Test that an error is raised when all files are excluded
    mock_generate_report.reset_mock()
    assert ament_mypy.main.main(['02.py', '--exclude', '02.py'])
    mock_generate_report.assert_not_called()


def test_ignore(use_dir, ignore_dir):
    mock_generate_report.return_value = ('', '', 0)

    # Test if returns no error if at least one valid dir is presented
    assert ament_mypy.main.main([str(use_dir), str(ignore_dir)]) == 0


def test_fail(mocker, mock_mypy_generate_fail, use_dir):
    # Test if an error message from mypy causes a non-zero return
    assert ament_mypy.main.main([str(use_dir.join('01.py'))])

    # Test that a failure correctly forwards the error code out of main
    ret_val = mock_mypy_generate_fail.return_value
    test_val = 5
    mock_mypy_generate_fail.return_value = (ret_val[0], ret_val[1], test_val)
    assert ament_mypy.main.main([str(use_dir.join('01.py'))]) == test_val
    test_val = 6
    mock_mypy_generate_fail.return_value = (ret_val[0], ret_val[1], test_val)
    assert ament_mypy.main.main([str(use_dir.join('01.py'))]) == test_val

    # Test that an error code even with a blank message still gets forwarded out
    mock_mypy_generate_fail.return_value = ('', '', test_val)
    assert ament_mypy.main.main([str(use_dir.join('01.py'))]) == test_val


def test_main_error(mocker, use_dir):
    # Test if an error from mypy invocation causes a non-zero return
    mock_error = mocker.patch('ament_mypy.main._generate_mypy_report')
    mock_error.return_value = ('', 'mypy error occurred', 15)

    assert ament_mypy.main.main([str(use_dir)]) == 15


def test_main_config_file(mock_generate_report, mocker, use_dir):
    # Test that a valid config file is passed on to mypy
    conf_file = use_dir.join('mypy.ini')
    conf_file.write('[mypy]')

    mock_generate_report.return_value = ('', None, 0)
    assert ament_mypy.main.main([str(use_dir.join('01.py')), '--config', str(conf_file)]) == 0
    args, _ = mock_generate_report.call_args
    assert args[1] == conf_file

    # Test program handles no config file being passed correctly
    assert ament_mypy.main.main([str(use_dir.join('01.py'))]) == 0
    args, _ = mock_generate_report.call_args
    assert args[1] is not None

    # Test program raises error when invalid config file is presented
    assert ament_mypy.main.main([str(use_dir.join('01.py')),
                                 '--config',
                                 str(use_dir.join('aeiou.ini'))]) == 1


def test_main_xunit(mock_mypy_generate_fail, mocker, use_dir):
    mock_xunit = mocker.patch('ament_mypy.main._get_xunit_content')
    mock_xunit.return_value = "<?xml version='1.0' encoding='UTF-8'?></xml>\n"

    # Test that generating report files works
    assert ament_mypy.main.main([str(use_dir), '--xunit-file', str(use_dir.join('testgen'))])
    assert mock_xunit.call_args[0][1].endswith('testgen')
    assert Path(use_dir.join('testgen')).is_file()

    # Test that .xml file suffix is processed correctly
    assert ament_mypy.main.main([str(use_dir), '--xunit-file', str(use_dir.join('testgen.xml'))])
    assert mock_xunit.call_args[0][1].endswith('testgen')
    assert Path(use_dir.join('testgen.xml')).is_file()

    # Test that .xunit suffix is also handled
    assert ament_mypy.main.main([str(use_dir), '--xunit-file',
                                 str(use_dir.join('testgen.xunit.xml'))])
    assert mock_xunit.call_args[0][1].endswith('testgen')
    assert Path(use_dir.join('testgen.xunit.xml')).is_file()

    # Test that intermediate directories are handled
    assert ament_mypy.main.main([str(use_dir), '--xunit-file',
                                 str(use_dir.join('testdir', 'testgen'))])
    assert mock_xunit.call_args[0][1].endswith('testgen')
    assert Path(use_dir.join('testdir', 'testgen')).is_file()

    # Test that the errors/warnings generated are all forwarded to the xml generator
    assert len(mock_xunit.call_args[0][0]) == 4


def test__get_xunit_content(mocker, sample_errors):
    # Test that all errors are accounted for in xml
    errors = ament_mypy.main._get_errors('\n'.join(sample_errors))
    xml = ament_mypy.main._get_xunit_content(errors, 'tst',
                                             [err.group('filename') for err in errors], .01)
    root = ET.fromstring(xml)
    assert root.get('name') == 'tst'
    assert root.get('failures') == str(len(errors))
    assert len(root) == len(errors) + 1

    # Test that a no failure case works
    xml = ament_mypy.main._get_xunit_content([], 'tst',
                                             [err.group('filename') for err in errors], .01)
    root = ET.fromstring(xml)
    assert root.get('failures') == '0'
    assert(len(root)) == 2
