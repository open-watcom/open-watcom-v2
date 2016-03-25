@echo off
REM *****************************************************************
REM SETVARS.BAT - Windows NT/DOS version
REM *****************************************************************
REM NOTE: Do not use this batch file directly, but copy it and
REM       modify it as necessary for your own use!!

REM Change this to point your Open Watcom source tree
set OWROOT=c:\ow\ow

REM Set this entry to identify your toolchain used by build process
REM supported values are WATCOM VISUALC INTEL
set OWTOOLS=WATCOM

REM Documentation related variables

REM Set this variable to 0 to suppress documentation build
set OWDOCBUILD=1

REM Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
REM set OWGHOSTSCRIPTPATH=%PATH%

REM Change these variables to point Windows help compilers which you have installed
REM set OWWIN95HC=hcrtf
REM set OWHHC=hhc

if not '%OS%' == 'Windows_NT' goto common_setup
REM build process requires WGML utility which is available only as DOS executable
REM on Windows platforms which don't have NTVDM or if WGML doesn't work under NTVDM
REM DOSBOX emulator must be installed and OWDOSBOX variable must be set
REM It is necessary for all 64-bit Windows and Windows 7 32-bit
REM Uncoment and set OWDOSBOX variable bellow to point to DOSBOX emulator executable
REM path must not include any spaces

REM set OWDOSBOX=dosbox

:common_setup

REM Invoke the batch file for the common environment
call %OWROOT%\cmnvars.bat

REM Change the default command prompt
if not '%OS%' == 'Windows_NT' prompt $p$g

cd %OWROOT%
