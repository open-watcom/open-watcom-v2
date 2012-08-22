@echo off
REM *****************************************************************
REM SETVARS.CMD - OS/2 version
REM *****************************************************************
REM NOTE: Do not use this batch file directly, but copy it and
REM       modify it as necessary for your own use!!

REM Setup environment variables for CVS repository

REM Change this to point to your Open Watcom source tree
set OWROOT=d:\ow

REM Change this to point to your existing Open Watcom installation
set WATCOM=c:\c\ow10

REM Change this to point to your OS/2 Toolkit directory
set OS2TKROOT=c:\Toolkit

REM Change this to the PATH required by GhostScript for PDF creation on used host OS (optional)
set OWGHOSTSCRIPTPATH=c:\gs\gs7.04

REM Set this variable to 1 to get debug build
set DEBUG_BUILD=0

REM Set this variable to 1 to get default windowing support in clib
set DEFAULT_WINDOWING=0

REM Set this variable to 0 to suppress documentation build
set OWDOCBUILD=1

REM Documentation related variables
REM set appropriate variables to blank for help compilers which you haven't installed
rem set OWWIN95HC=hcrtf
rem set OWHHC=hhc

REM Subdirectory to be used for bootstrapping
set OWOBJDIR=bootstrp

REM Subdirectory to be used for building prerequisite utilities
set OWPREOBJDIR=prebuild

REM Invoke the script for the common environment
call %OWROOT%\cmnvars.cmd

cd %OWROOT%\bld
