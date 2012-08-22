@echo off
REM *****************************************************************
REM SETVARS.BAT - Windows NT version
REM *****************************************************************
REM NOTE: Do not use this batch file directly, but copy it and
REM       modify it as necessary for your own use!!

REM Setup environment variables for CVS repository

REM Change this to point to your Open Watcom source tree - must be an 8.3 name!
set OWROOT=d:\ow

REM Change this to point to your existing Open Watcom installation
set WATCOM=c:\c\ow10

REM Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
set OWGHOSTSCRIPTPATH=c:\gs\gs7.04\bin;c:\gs\gs7.04\lib

REM Set this variable to 1 to get debug build
set DEBUG_BUILD=0

REM Set this variable to 1 to get default windowing support in clib
set DEFAULT_WINDOWING=0

REM Set this variable to 0 to suppress documentation build
set OWDOCBUILD=1

REM Documentation related variables
REM set appropriate variables to point to Windows help compilers which you have installed
rem set OWWIN95HC=hcrtf
rem set OWHHC=hhc

REM check if DOS is available on Windows
if not '%OS%' == 'Windows_NT' goto dos_ok
if exist %SystemRoot%\system32\dosx.exe goto dos_ok
REM set OWDOSBOX variable to point to DOSBOX emulator
REM it can be LFN, but must not include any spaces
set OWDOSBOX=dosbox
:dos_ok

REM Subdirectory to be used for bootstrapping
set OWOBJDIR=bootstrp

REM Subdirectory to be used for building prerequisite utilities
set OWPREOBJDIR=prebuild

REM Invoke the batch file for the common environment
call %OWROOT%\cmnvars.bat

cd %DEVDIR%
