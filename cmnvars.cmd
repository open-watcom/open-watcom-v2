@echo off
REM *****************************************************************
REM CMNVARS.CMD - common environment variables
REM *****************************************************************
REM NOTE: All scripts to set the environment must call this script at
REM       the end.

REM Set the version numbers
set BLD_VER=20
set BLD_VER_STR=2.0

REM Set up default path information variable
if not "%OWDEFPATH%" == "" goto defpath_set
set OWDEFPATH=%PATH%;
set OWDEFINCLUDE=%INCLUDE%
set OWDEFWATCOM=%WATCOM%
set OWDEFBEGINLIBPATH=%BEGINLIBPATH%
:defpath_set

REM Subdirectory to be used for build binaries
set OWBINDIR=%OWROOT%\build\bin

REM Subdirectory containing OW sources
set OWSRCDIR=%OWROOT%\bld

REM Set environment variables
set PATH=%OWBINDIR%;%OWROOT%\build;%OWDEFPATH%
set INCLUDE=%OWDEFINCLUDE%
set WATCOM=%OWDEFWATCOM%
set BEGINLIBPATH=%OWDEFBEGINLIBPATH%

REM OS specifics

REM Ensure COMSPEC points to CMD.EXE
set COMSPEC=CMD.EXE

echo Open Watcom compiler build environment

