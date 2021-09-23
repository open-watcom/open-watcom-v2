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

REM Build control related variables
REM ###############################

REM Set this variable to 0 to suppress documentation build
set OWDOCBUILD=1

REM Set this variable to 1 to suppress tools GUI version build
REM If it is used then only tools character mode version is build
REM set OWGUINOBUILD=1

REM Set this variable to list of OW projects to suppress their build
REM Example set OWNOBUILD=ide browser dlgprs
REM set OWNOBUILD=

REM Set this variable to 1 to enable build all installers
set OWDISTRBUILD=0

REM Documentation related variables
REM ###############################

REM Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
REM For Windows host we use Windows CI copy of appropriate tools
if not '%OS%' == 'Windows_NT' goto csetup1
set OWGHOSTSCRIPTPATH=%OWROOT%\ci\ntx64
:csetup1

REM Change these variables to point Windows help compilers which you have installed
REM For Windows host we use Windows CI copy of appropriate tools
if not '%OS%' == 'Windows_NT' goto csetup2
set OWWIN95HC=%OWROOT%\ci\nt386\hcrtf
set OWHHC=%OWROOT%\ci\nt386\hhc
:csetup2

if not '%OS%' == 'Windows_NT' goto csetup3
REM build process requires WGML utility which is available only as DOS executable
REM on Windows platforms which don't have NTVDM or if WGML doesn't work under NTVDM
REM DOSBOX emulator must be installed and OWDOSBOX variable must be set
REM It is necessary for all 64-bit Windows and Windows 7 32-bit
REM Uncoment and set OWDOSBOX variable bellow to point to DOSBOX emulator executable
REM path must not include any spaces

REM For Windows host we use Windows CI copy of appropriate tools
set OWDOSBOX=%OWROOT%\ci\nt386\dosbox.exe
:csetup3

REM ###############################

REM Subdirectory to be used for building OW build tools
set OWOBJDIR=binbuild

REM DOS4GW 1.97 
set DOS4G=QUIET
REM DOS4GW 2.0x 
REM set DOS4G=STARTUPBANNER:1

REM Invoke the batch file for the common environment
call %OWROOT%\cmnvars.bat

REM Change the default command prompt
if not '%OS%' == 'Windows_NT' prompt $p$g

cd %OWROOT%
