:: generated from ament_package/template/prefix_level/setup.bat.in
@echo off

:: get the directory of this file without trailing backslash
set "_current_prefix_path=%~dp0"
if %_current_prefix_path:~-1%==\ set _current_prefix_path=%_current_prefix_path:~0,-1%

:: find parent prefix path files for all packages under the current prefix
call:list_files _resource_names "%_current_prefix_path%\share\ament_index\resource_index\parent_prefix_path"

:: get the unique parent prefix path in reverse order
call:get_prefix_path _unique_reverse_prefix_path "%_current_prefix_path%" "%_resource_names%"
set "_resource_names="

:: append this directory to the prefix path
call:ament_append_unique_value _unique_reverse_prefix_path "%_current_prefix_path%"
set "_current_prefix_path="

:: source local_setup.bat file for each prefix path
for %%p in ("%_unique_reverse_prefix_path:;=";"%") do (
  call:call_file "%%~p\local_setup.bat"
)
set "_unique_reverse_prefix_path="

:: end of script
goto:eof


:: List the files in a directory in alphabetical order.
:: first argument: the name of the result variable
:: second argument: the directory
:list_files
  setlocal enabledelayedexpansion
  set "files="
  for /f %%f in ('dir /b "%~2"') do (
    if "!files!" NEQ "" set "files=!files!;"
    set "files=!files!%%~f"
  )
  endlocal & (
    :: set result variable in parent scope
    set "%~1=%files%"
  )
goto:eof

:: Iterate over all parent_prefix_path files and
:: get the unique parent prefix path in reverse order.
:: first argument: the name of the result variable
:: second parameter: the prefix path names
:: third parameter: the resource names
:get_prefix_path
  setlocal enabledelayedexpansion
  set "prefix_path="
  set "resource_names=%~3"
  if "%resource_names%" NEQ "" (
    for %%a in ("%resource_names:;=";"%") do (
      :: reset variable otherwise it keeps its previous value if the file is empty
      set "reverse_ppp="
      for /f "tokens=1 delims=" %%p in (%~2\share\ament_index\resource_index\parent_prefix_path\%%~a) do (
        if "!reverse_ppp!" NEQ "" set "reverse_ppp=;!reverse_ppp!"
        :: replace placeholder of current prefix
        if "%%~p" EQU "{prefix}" (
          set "p=%~2"
        )
        set "reverse_ppp=%%~p!reverse_ppp!"
      )

      :: append unique prefix path
      for %%p in ("!reverse_ppp:;=";"!") do (
        call:ament_append_unique_value prefix_path "%%~p"
      )
    )
  )
  endlocal & (
    :: set result variable in parent scope
    set "%~1=%prefix_path%"
  )
goto:eof

:: Append non-duplicate values to environment variables
:: using semicolons as separators and avoiding trailing separators.
:: first argument: the name of the result variable
:: second argument: the value
:ament_append_unique_value
  setlocal enabledelayedexpansion
  :: arguments
  set "listname=%~1"
  set "value=%~2"
  :: expand the list variable
  set "list=!%listname%!"
  :: check if the list contains the value
  set "is_duplicate="
  if "%list%" NEQ "" (
    for %%v in ("%list:;=";"%") do (
      if "%%~v" == "%value%" set "is_duplicate=1"
    )
  )
  :: if it is not a duplicate append it
  if "%is_duplicate%" == "" (
    :: if not empty, append a semi-colon
    if "!list!" NEQ "" set "list=!list!;"
    :: append the value
    set "list=!list!%value%"
  )
  endlocal & (
    :: set result variable in parent scope
    set "%~1=%list%"
  )
goto:eof

:: Call the specified batch file and output the name when tracing is requested.
:: first argument: the batch file
:call_file
  if "%AMENT_TRACE_SETUP_FILES%" NEQ "" echo call "%~1"
  if exist "%~1" call "%~1%"
goto:eof
