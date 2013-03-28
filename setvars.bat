@echo off
REM *****************************************************************
REM SETVARS.BAT - Windows NT/DOS version
REM *****************************************************************
REM NOTE: Do not use this batch file directly, but copy it and
REM       modify it as necessary for your own use!!

REM Change this to point your Open Watcom source tree
set OWROOT=c:\ow\ow

REM Set this entry to use native compiler tools
set OWUSENATIVETOOLS=0

REM Subdirectory to be used for building OW build tools
set OWOBJDIR=binbuild

REM Set this variable to 1 to get debug build
set OWDEBUGBUILD=0

REM Set this variable to 1 to get default windowing support in clib
set OWDEFAULT_WINDOWING=0

REM Change following entries to point your existing Open Watcom installation
if '%OWUSENATIVETOOLS%' == '1' goto docs_setup
set WATCOM=c:\ow\ow19
set INCLUDE=%WATCOM%\h
set PATH=%WATCOM%\binw;%PATH%
if not '%OS%' == 'Windows_NT' goto docs_setup
set INCLUDE=%INCLUDE%;%WATCOM%\h\nt
set PATH=%WATCOM%\binnt;%PATH%

:docs_setup

REM Documentation related variables

REM Set this variable to 0 to suppress documentation build
set OWDOCBUILD=1

REM Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
REM set OWGHOSTSCRIPTPATH=%PATH%

REM Change these variables to point Windows help compilers which you have installed
REM set OWWIN95HC=hcrtf
REM set OWHHC=hhc

REM check if DOS is available on Windows
REM set OWDOSBOX variable to point to DOSBOX emulator
REM it can be LFN, but must not include any spaces
if not '%OS%' == 'Windows_NT' goto common_setup
if exist %SystemRoot%\system32\dosx.exe goto common_setup
set OWDOSBOX=dosbox

:common_setup

REM Invoke the batch file for the common environment
call %OWROOT%\cmnvars.bat

REM Change the default command prompt
if not '%OS%' == 'Windows_NT' prompt $p$g

REM Make the window bigger
if '%OS%' == 'Windows_NT' mode 80,50

cd %OWROOT%
