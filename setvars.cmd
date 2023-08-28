@echo off
REM *****************************************************************
REM SETVARS.CMD - OS/2 version
REM *****************************************************************
REM NOTE: Do not use this batch file directly, but copy it and
REM       modify it as necessary for your own use!!

REM Change OWROOT to point your Open Watcom source tree.
REM Note: '=' sign in path is not allowed (build will fail).
REM
REM set OWROOT=c:\ow\ow
REM
REM If space character is used in the path then you must double quote
REM this path.
REM
REM set "OWROOT=c:\ow\o w"
REM

set OWROOT=c:\ow\ow

REM Set OWTOOLS entry to identify your toolchain used by build process
REM supported values are WATCOM

set OWTOOLS=WATCOM

REM Build control related variables
REM ###############################

REM Set OWDOCBUILD variable to 0 to suppress documentation build

set OWDOCBUILD=1

REM Set OWNOWGML variable to 1 to not use WGML utility
REM It doesn't requires appropriate DOS emulator
REM and suppress documentation build

REM set OWNOWGML=1

REM Set OWGUINOBUILD variable to 1 to suppress tools GUI version build
REM If it is used then only tools character mode version is build

REM set OWGUINOBUILD=1

REM Set OWNOBUILD variable to list of OW projects to suppress their build
REM Example set OWNOBUILD=ide browser dlgprs

REM set OWNOBUILD=

REM Set OWDISTRBUILD variable to 1 to enable build all installers

set OWDISTRBUILD=0

REM Documentation related variables
REM ###############################

REM Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
REM set OWGHOSTSCRIPTPATH=%PATH%

REM ###############################

REM Subdirectory to be used for building OW build tools
REM default is 'binbuild'
REM set OWOBJDIR=binbuild

REM Invoke the script for the common environment setup
call "%OWROOT%\cmnvars.cmd"

cd "%OWROOT%"
