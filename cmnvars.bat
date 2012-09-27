@echo off
REM *****************************************************************
REM CMNVARS.BAT - common environment variables
REM *****************************************************************
REM NOTE: All batch files to set the environment must call this batch
REM       file at the end.

REM Set the version numbers
set BLD_VER=20
set BLD_VER_STR=2.0

REM Set up default path information variables
if not "%OWDEFPATH%" == "" goto defpath_set
set OWDEFPATH=%PATH%;
set OWDEFINCLUDE=%INCLUDE%
set OWDEFWATCOM=%WATCOM%
:defpath_set

REM Subdirectory to be used for build binaries
set OWBINDIR=%OWROOT%\build\bin

REM Subdirectory containing OW sources
set OWSRCDIR=%OWROOT%\bld

REM Set environment variables
set PATH=%OWBINDIR%;%OWROOT%\build;%OWDEFPATH%
set INCLUDE=%OWDEFINCLUDE%
set WATCOM=%OWDEFWATCOM%

REM OS specifics

REM setup right COMSPEC for non-standard COMSPEC setting on NT based systems
if not '%OS%' == 'Windows_NT' goto no_windows_nt
set COMSPEC=%WINDIR%\system32\cmd.exe
set COPYCMD=/y
:no_windows_nt

set DOS4G=quiet

echo Open Watcom build environment
