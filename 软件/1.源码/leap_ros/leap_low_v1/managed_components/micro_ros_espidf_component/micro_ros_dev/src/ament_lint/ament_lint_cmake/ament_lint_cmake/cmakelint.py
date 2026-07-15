"""
Copyright 2009 Richard Quirk

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this file except in compliance with the License. You may obtain a copy of
the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

https://github.com/richq/cmake-lint/blob/7b85fe46b9bd66fe11ecfef20060f976a49d9662/cmakelint/main.py
- removed __version__ import
- allow closing parenthesis to be on the same level as the opening one (lines 281-282)
- ignore lines with exceeding length if they only contain a single string (lines 35, 196-207)
- improve SetFilters ability to parse new filters (lines 89-92)
- implement in file pragmas to control filters on a per file basis (lines 389 - 430)
"""
from __future__ import print_function
import sys
import re
import os
import getopt

if sys.version_info < (3,):
    # xrange slightly faster than range on python2
    range = xrange

_RE_CLEAN_COMMENT = re.compile(r'(\s*\#.*)', re.VERBOSE)
_RE_COMMAND = re.compile(r'^\s*(\w+)(\s*)\(', re.VERBOSE)
_RE_COMMAND_START_SPACES = re.compile(r'^\s*\w+\s*\((\s*)', re.VERBOSE)
_RE_COMMAND_END_SPACES = re.compile(r'(\s*)\)', re.VERBOSE)
_RE_LOGIC_CHECK = re.compile(r'(\w+)\s*\(\s*\S+[^)]+\)', re.VERBOSE)
_RE_COMMAND_ARG = re.compile(r'(\w+)', re.VERBOSE)
_RE_STRING = re.compile(r'^\s*\#?\s*"[^"]+"\)?', re.VERBOSE)
_logic_commands = """
else
endforeach
endfunction
endif
endmacro
endwhile
""".split()
_USAGE = """
Syntax: cmakelint.py [--version] [--config=file] [--filter=-x,+y] [--spaces=N] <file> [file] ...
    filter=-x,+y,...
      Specify a comma separated list of filters to apply

    spaces=N
      Indentation should be a multiple of N spaces

    config=file
      Use the given file for configuration. By default the file
      ~/.cmakelintrc is used if it exists.  Use the value "None" to use no
      configuration file (./None for a file called literally None)
      Only the option "filter=" is currently supported in this file.

    version
      Show the version number and end
"""
_ERROR_CATEGORIES = """\
        convention/filename
        linelength
        package/consistency
        package/stdargs
        readability/logic
        readability/mixedcase
        readability/wonkycase
        syntax
        whitespace/eol
        whitespace/extra
        whitespace/indent
        whitespace/mismatch
        whitespace/newline
        whitespace/tabs
"""
_DEFAULT_CMAKELINTRC = os.path.join(os.path.expanduser('~'), '.cmakelintrc')

class _CMakeLintState(object):
    def __init__(self):
        self.filters = []
        self.config = 0
        self.errors = 0
        self.spaces = 2
        self.linelength = 80
        self.allowed_categories = _ERROR_CATEGORIES.split()

    def SetFilters(self, filters):
        if not filters:
            return
        assert isinstance(self.filters, list)
        if isinstance(filters, list):
            self.filters.extend(filters)
        elif isinstance(filters, str):
            self.filters.extend([f.strip() for f in filters.split(',') if f])
        else:
            raise ValueError('Filters should be a list or a comma separated string')
        for f in self.filters:
            if f.startswith('-') or f.startswith('+'):
                allowed = False
                for c in self.allowed_categories:
                    if c.startswith(f[1:]):
                        allowed = True
                if not allowed:
                    raise ValueError('Filter not allowed: %s'%f)
            else:
                raise ValueError('Filter should start with - or +')

    def SetSpaces(self, spaces):
        self.spaces = int(spaces.strip())

    def SetLineLength(self, linelength):
        self.linelength = int(linelength)

class _CMakePackageState(object):
    def __init__(self):
        self.sets = []
        self.have_included_stdargs = False
        self.have_used_stdargs = False

    def Check(self, filename, linenumber, clean_lines, errors):
        pass

    def _GetExpected(self, filename):
        package = os.path.basename(filename)
        package = re.sub(r'^Find(.*)\.cmake', lambda m: m.group(1), package)
        return package.upper()

    def Done(self, filename, errors):
        try:
            if not IsFindPackage(filename):
                return
            if self.have_included_stdargs and self.have_used_stdargs:
                return
            if not self.have_included_stdargs:
                errors(
                    filename,
                    0,
                    'package/consistency',
                    'Package should include FindPackageHandleStandardArgs')
            if not self.have_used_stdargs:
                errors(
                    filename,
                    0,
                    'package/consistency',
                    'Package should use FIND_PACKAGE_HANDLE_STANDARD_ARGS')
        finally:
            self.have_used_stdargs = False
            self.have_included_stdargs = False

    def HaveUsedStandardArgs(self, filename, linenumber, var, errors):
        expected = self._GetExpected(filename)
        self.have_used_stdargs = True
        if expected != var:
            errors(
                filename,
                linenumber,
                'package/stdargs',
                'Weird variable passed to std args, should be ' +
                expected + ' not ' + var)

    def HaveIncluded(self, var):
        if var == 'FindPackageHandleStandardArgs':
            self.have_included_stdargs = True

    def Set(self, var):
        self.sets.append(var)

_lint_state = _CMakeLintState()
_package_state = _CMakePackageState()

def CleanComments(line):
    return _RE_CLEAN_COMMENT.sub('', line)

class CleansedLines(object):
    def __init__(self, lines):
        self.have_seen_uppercase = None
        self.raw_lines = lines
        self.lines = [CleanComments(line) for line in lines]

    def LineNumbers(self):
        return range(0, len(self.lines))

def ShouldPrintError(category):
    should_print = True
    for f in _lint_state.filters:
        if f.startswith('-') and category.startswith(f[1:]):
            should_print = False
        elif f.startswith('+') and category.startswith(f[1:]):
            should_print = True
    return should_print

def Error(filename, linenumber, category, message):
    if ShouldPrintError(category):
        _lint_state.errors += 1
        print('%s:%d: %s [%s]' % (filename, linenumber, message, category))

def CheckLineLength(filename, linenumber, clean_lines, errors):
    """
    Check for lines longer than the recommended length
    """
    line = clean_lines.raw_lines[linenumber]
    if len(line) > _lint_state.linelength:
        if _RE_STRING.match(line):
            lineno = linenumber
            while lineno > 0:
                lineno -= 1
                line = clean_lines.raw_lines[lineno]
                cmd = GetCommand(line)
                if cmd == 'message':
                    # message string can be split so warning is feasible
                    break
                if cmd:
                    # since single strings can't be split don't warn about it
                    return
        return errors(
                filename,
                linenumber,
                'linelength',
                'Lines should be <= %d characters long' %
                    (_lint_state.linelength))

def ContainsCommand(line):
    return _RE_COMMAND.match(line)

def GetCommand(line):
    match = _RE_COMMAND.match(line)
    if match:
        return match.group(1)
    return ''

def IsCommandMixedCase(command):
    lower = command.lower()
    upper = command.upper()
    return not (command == lower or command == upper)

def IsCommandUpperCase(command):
    upper = command.upper()
    return command == upper

def CheckUpperLowerCase(filename, linenumber, clean_lines, errors):
    """
    Check that commands are either lower case or upper case, but not both
    """
    line = clean_lines.lines[linenumber]
    if ContainsCommand(line):
        command = GetCommand(line)
        if IsCommandMixedCase(command):
            return errors(
                    filename,
                    linenumber,
                    'readability/wonkycase',
                    'Do not use mixed case commands')
        if clean_lines.have_seen_uppercase is None:
            clean_lines.have_seen_uppercase = IsCommandUpperCase(command)
        else:
            is_upper = IsCommandUpperCase(command)
            if is_upper != clean_lines.have_seen_uppercase:
                return errors(
                        filename,
                        linenumber,
                        'readability/mixedcase',
                        'Do not mix upper and lower case commands')

def CheckCommandSpaces(filename, linenumber, clean_lines, errors):
    """
    No extra spaces between command and parenthesis
    """
    line = clean_lines.lines[linenumber]
    match = ContainsCommand(line)
    if match and len(match.group(2)):
        errors(filename, linenumber, 'whitespace/extra',
                "Extra spaces between '%s' and its ()"%(match.group(1)))
    if match:
        spaces_after_open = len(_RE_COMMAND_START_SPACES.match(line).group(1))
        initial_linenumber = linenumber
        end = None
        while True:
            line = clean_lines.lines[linenumber]
            end = _RE_COMMAND_END_SPACES.search(line)
            linenumber += 1
            if end or linenumber >= len(clean_lines.lines):
                break
        if linenumber == len(clean_lines.lines) and not end:
            errors(filename, initial_linenumber, 'syntax',
                    'Unable to find the end of this command')
        if end:
            spaces_before_end = len(end.group(1))
            if spaces_after_open != spaces_before_end:
                spaces_before_command = match.group(0).find(match.group(1))
                if spaces_before_command != spaces_before_end:
                    errors(filename, initial_linenumber, 'whitespace/mismatch',
                            'Mismatching spaces inside () after command')

def CheckRepeatLogic(filename, linenumber, clean_lines, errors):
    """
    Check for logic inside else, endif etc
    """
    line = clean_lines.lines[linenumber]
    for cmd in _logic_commands:
        if re.search(r'\b%s\b'%cmd, line.lower()):
            m = _RE_LOGIC_CHECK.search(line)
            if m:
                errors(filename, linenumber, 'readability/logic',
                        'Expression repeated inside %s; '
                        'better to use only %s()'%(cmd, m.group(1)))
            break

def CheckIndent(filename, linenumber, clean_lines, errors):
    initial_spaces = 0
    line = clean_lines.raw_lines[linenumber]
    while initial_spaces < len(line) and line[initial_spaces] == ' ':
        initial_spaces += 1
    remainder = initial_spaces % _lint_state.spaces
    if remainder != 0:
        errors(filename, linenumber, 'whitespace/indent',
                'Weird indentation; use %d spaces'%(_lint_state.spaces))

def CheckStyle(filename, linenumber, clean_lines, errors):
    """
    Check style issues. These are:
    No extra spaces between command and parenthesis
    Matching spaces between parenthesis and arguments
    No repeated logic in else(), endif(), endmacro()
    """
    CheckIndent(filename, linenumber, clean_lines, errors)
    CheckCommandSpaces(filename, linenumber, clean_lines, errors)
    line = clean_lines.raw_lines[linenumber]
    if line.find('\t') != -1:
        errors(filename, linenumber, 'whitespace/tabs', 'Tab found; please use spaces')

    if line and line[-1].isspace():
        errors(filename, linenumber, 'whitespace/eol', 'Line ends in whitespace')

    CheckRepeatLogic(filename, linenumber, clean_lines, errors)

def CheckFileName(filename, errors):
    name_match = re.match(r'Find(.*)\.cmake', os.path.basename(filename))
    if name_match:
        package = name_match.group(1)
        if not package.isupper():
            errors(filename, 0, 'convention/filename',
                    'Find modules should use uppercase names; '
                    'consider using Find' + package.upper() + '.cmake')
    else:
        if filename.lower() == 'cmakelists.txt' and filename != 'CMakeLists.txt':
            errors(filename, 0, 'convention/filename',
                    'File should be called CMakeLists.txt')

def IsFindPackage(filename):
    return os.path.basename(filename).startswith('Find') and filename.endswith('.cmake')

def GetCommandArgument(linenumber, clean_lines):
    line = clean_lines.lines[linenumber]
    skip = GetCommand(line)
    while True:
        line = clean_lines.lines[linenumber]
        m = _RE_COMMAND_ARG.finditer(line)
        for i in m:
            if i.group(1) == skip:
                continue
            return i.group(1)
        linenumber += 1
    return ''

def CheckFindPackage(filename, linenumber, clean_lines, errors):
    cmd = GetCommand(clean_lines.lines[linenumber])
    if cmd:
        if cmd.lower() == 'include':
            var_name = GetCommandArgument(linenumber, clean_lines)
            _package_state.HaveIncluded(var_name)
        elif cmd.lower() == 'find_package_handle_standard_args':
            var_name = GetCommandArgument(linenumber, clean_lines)
            _package_state.HaveUsedStandardArgs(filename, linenumber, var_name, errors)

def ProcessLine(filename, linenumber, clean_lines, errors):
    """
    Arguments:
      filename    the name of the file
      linenumber  the line number index
      clean_lines CleansedLines instance
      errors      the error handling function
    """
    CheckLineLength(filename, linenumber, clean_lines, errors)
    CheckUpperLowerCase(filename, linenumber, clean_lines, errors)
    CheckStyle(filename, linenumber, clean_lines, errors)
    if IsFindPackage(filename):
        CheckFindPackage(filename, linenumber, clean_lines, errors)

def IsValidFile(filename):
    return filename.endswith('.cmake') or os.path.basename(filename).lower() == 'cmakelists.txt'

def ProcessFile(filename):
    # Store and then restore the filters to prevent pragmas in the file from persisting.
    original_filters = list(_lint_state.filters)
    try:
        return _ProcessFile(filename)
    finally:
        _lint_state.filters = original_filters


def _ProcessFile(filename):
    linter_pragma_start = '# lint_cmake: '
    lines = ['# Lines start at 1']
    have_cr = False
    if not IsValidFile(filename):
        print('Ignoring file: ' + filename)
        return
    global _package_state
    _package_state = _CMakePackageState()
    with open(filename) as f:
        for l in f.readlines():
            l = l.rstrip('\n')
            if l.endswith('\r'):
                have_cr = True
                l = l.rstrip('\r')
            lines.append(l)
            # Check this line to see if it is a lint_cmake pragma
            if l.startswith(linter_pragma_start):
                try:
                    _lint_state.SetFilters(l[len(linter_pragma_start):])
                except:
                    print("Exception occurred while processing '{0}:{1}':"
                          .format(filename, len(lines)))
                    raise
    lines.append('# Lines end here')
    # Check file name after reading lines incase of a # lint_cmake: pragma
    CheckFileName(filename, Error)
    if have_cr and os.linesep != '\r\n':
        Error(filename, 0, 'whitespace/newline', 'Unexpected carriage return found; '
                'better to use only \\n')
    clean_lines = CleansedLines(lines)
    for line in clean_lines.LineNumbers():
        ProcessLine(filename, line, clean_lines, Error)
    _package_state.Done(filename, Error)

def PrintVersion():
    sys.stderr.write("cmakelint 1.2.01\n")
    sys.exit(0)

def PrintUsage(message):
    sys.stderr.write(_USAGE)
    if message:
        sys.exit('FATAL ERROR: '+message)
    else:
        sys.exit(1)

def PrintCategories():
    sys.stderr.write(_ERROR_CATEGORIES)
    sys.exit(0)

def ParseOptionFile(contents, ignore_space):
    filters = None
    spaces = None
    linelength = None
    for line in contents:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        if line.startswith('filter='):
            filters = line.replace('filter=', '')
        if line.startswith('spaces='):
            spaces = line.replace('spaces=', '')
        if line.startswith('linelength='):
            linelength = line.replace('linelength=', '')
    _lint_state.SetFilters(filters)
    if spaces and not ignore_space:
        _lint_state.SetSpaces(spaces)
    if linelength is not None:
        _lint_state.SetLineLength(linelength)

def ParseArgs(argv):
    try:
        (opts, filenames) = getopt.getopt(argv, '',
                ['help', 'filter=', 'config=', 'spaces=', 'version'])
    except getopt.GetoptError:
        PrintUsage('Invalid Arguments')
    filters = ""
    _lint_state.config = _DEFAULT_CMAKELINTRC
    ignore_space = False
    for (opt, val) in opts:
        if opt == '--version':
            PrintVersion()
        elif opt == '--help':
            PrintUsage(None)
        elif opt == '--filter':
            filters = val
            if not filters:
                PrintCategories()
        elif opt == '--config':
            _lint_state.config = val
            if _lint_state.config == 'None':
                _lint_state.config = None
        elif opt == '--spaces':
            try:
                _lint_state.SetSpaces(val)
                ignore_space = True
            except:
                PrintUsage('spaces expects an integer value')
    try:
        if _lint_state.config:
            try: ParseOptionFile(open(_lint_state.config, 'rU').readlines(), ignore_space)
            except IOError:
                pass
        _lint_state.SetFilters(filters)
    except ValueError as ex:
        PrintUsage(str(ex))

    if not filenames:
        PrintUsage('No files were specified!')
    return filenames

def main():
    files = ParseArgs(sys.argv[1:])

    for filename in files:
        ProcessFile(filename)
    sys.stderr.write("Total Errors: %d\n" % _lint_state.errors)
