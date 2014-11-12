@echo off
REM *****************************************************************
REM SETVARS.CMD - OS/2 version
REM *****************************************************************
REM NOTE: Do not use this batch file directly, but copy it and
REM       modify it as necessary for your own use!!

REM Change this to point your Open Watcom source tree
set OWROOT=c:\ow\ow

REM Subdirectory to be used for building OW build tools
set OWOBJDIR=binbuild

REM Set this entry to identify tools used by build process
REM supported values are WATCOM
set OWTOOLS=WATCOM

REM Change following entries to point your existing Open Watcom installation
if not '%OWTOOLS%' == 'WATCOM' goto host_setup
set WATCOM=c:\ow\ow19
set INCLUDE=%WATCOM%\h;%WATCOM%\h\os2
set BEGINLIBPATH=%WATCOM%\binp\dll;%BEGINLIBPATH%
set PATH=%WATCOM%\binp;%WATCOM%\binw;%PATH%
:host_setup

REM Documentation related variables

REM Set this variable to 0 to suppress documentation build
set OWDOCBUILD=1

REM Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
REM set OWGHOSTSCRIPTPATH=%PATH%

REM Change these variables to point Windows help compilers which you have installed
REM set OWWIN95HC=hcrtf
REM set OWHHC=hhc

REM Invoke the script for the common environment
call %OWROOT%\cmnvars.cmd

cd %OWROOT%
