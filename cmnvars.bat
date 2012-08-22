@echo off
REM *****************************************************************
REM CMNVARS.BAT - common environment variables
REM *****************************************************************
REM NOTE: All batch files to set the environment must call this batch
REM       file at the end.

REM Set the version numbers
set BLD_VER=20
set BLD_VER_STR=2.0

REM Set up default path information variable
if "%DEFPATH%" == "" set DEFPATH=%PATH%

REM Stuff for the Open Watcom build environment
set BUILD_PLATFORM=nt386
set DEVDIR=%OWROOT%\bld
REM Subdirectory to be used for bootstrapping/prebuild binaries
set OWBINDIR=%DEVDIR%\build\binnt
set RELROOT=%OWROOT%\rel2
set DWATCOM=%WATCOM%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt
set EDPATH=%WATCOM%\eddat
set PATH=%OWBINDIR%;%OWROOT%\build;%WATCOM%\binnt;%WATCOM%\binw;%DEFPATH%

echo Open Watcom compiler build environment

REM OS specifics

set DOS4G=quiet

REM setup right COMSPEC for non-standard COMSPEC setting on NT based systems
if '%OS%' == 'Windows_NT' set COMSPEC=%windir%\system32\cmd.exe

set COPYCMD=/y
