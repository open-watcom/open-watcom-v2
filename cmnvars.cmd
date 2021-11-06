@echo off
REM *****************************************************************
REM CMNVARS.CMD - common environment variables
REM *****************************************************************
REM NOTE: All scripts to set the environment must call this script at
REM       the end.

REM Set the version numbers
set OWBLDVER=20
set OWBLDVERSTR=2.0
set OWBLDVERTOOL=1300

REM Set up default path information variable
if not "%OWDEFPATH%" == "" goto defpath_set
set OWDEFPATH=%PATH%;
set OWDEFINCLUDE=%INCLUDE%
set OWDEFWATCOM=%WATCOM%
set OWDEFBEGINLIBPATH=%BEGINLIBPATH%
:defpath_set

REM Subdirectory to be used for building OW build tools
if "%OWOBJDIR%" == "" set OWOBJDIR=binbuild

REM Subdirectory to be used for build binaries
set OWBINDIR=%OWROOT%\build

REM Subdirectory containing OW sources
set OWSRCDIR=%OWROOT%\bld

REM Subdirectory containing documentation sources
set OWDOCSDIR=%OWROOT%\docs

REM Subdirectory containing distribution sources
set OWDISTRDIR=%OWROOT%\distrib

REM Set environment variables
set PATH=%OWBINDIR%\%OWOBJDIR%;%OWBINDIR%;%OWDEFPATH%;%OWGHOSTSCRIPTPATH%
set INCLUDE=%OWDEFINCLUDE%
set WATCOM=%OWDEFWATCOM%
set BEGINLIBPATH=%OWDEFBEGINLIBPATH%

REM Set the toolchain version to OWTOOLSVER variable
set OWTOOLSVER=0
if not '%OWTOOLS%' == 'WATCOM' goto no_watcom
echo set OWTOOLSVER=__WATCOMC__>getversi.gc
wcc386 -p getversi.gc >getversi.cmd
goto toolsver
:no_watcom
:toolsver
if not exist getversi.cmd goto no_toolsver
call getversi.cmd
del getversi.*
:no_toolsver

REM OS specifics

REM Ensure COMSPEC points to CMD.EXE
set COMSPEC=CMD.EXE

echo Open Watcom build environment (%OWTOOLS% version=%OWTOOLSVER%)
