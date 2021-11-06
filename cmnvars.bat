@echo off
REM *****************************************************************
REM CMNVARS.BAT - common environment variables
REM *****************************************************************
REM NOTE: All batch files to set the environment must call this batch
REM       file at the end.

REM Set the version numbers
set OWBLDVER=20
set OWBLDVERSTR=2.0
set OWBLDVERTOOL=1300

REM Set up default path information variables
if not "%OWDEFPATH%" == "" goto skip1
set OWDEFPATH=%PATH%;
set OWDEFINCLUDE=%INCLUDE%
set OWDEFWATCOM=%WATCOM%
:skip1

REM Subdirectory to be used for building OW build tools
if "%OWOBJDIR%" == "" set OWOBJDIR=binbuild

REM Subdirectory to be used for build binaries
set OWBINDIR=%OWROOT%\build

REM Subdirectory containing OW sources
set OWSRCDIR=%OWROOT%\bld

REM Subdirectory containing documentation sources
set OWDOCSDIR=%OWROOT%\docs

REM Subdirectory containing distribution sources
set OWDISTRDIR=%OWROOT%\distrib

REM Set environment variables
set PATH=%OWBINDIR%\%OWOBJDIR%;%OWBINDIR%;%OWDEFPATH%;%OWGHOSTSCRIPTPATH%;%OWDOSBOXPATH%
set INCLUDE=%OWDEFINCLUDE%
set WATCOM=%OWDEFWATCOM%

REM Set the toolchain version to OWTOOLSVER variable
set OWTOOLSVER=0
if not '%OWTOOLS%' == 'WATCOM' goto nowatcom
echo set OWTOOLSVER=__WATCOMC__>getversi.gc
echo set OWCYEAR=__DATE__>>getversi.gc
wcc386 -p getversi.gc >getversi.bat
:nowatcom
if not '%OWTOOLS%' == 'VISUALC' goto novisualc
echo set OWTOOLSVER=_MSC_VER>getversi.gc
echo set OWCYEAR=__DATE__>>getversi.gc
cl -nologo -EP getversi.gc>getversi.bat
:novisualc
if not '%OWTOOLS%' == 'INTEL' goto nointel
echo set OWTOOLSVER=__INTEL_COMPILER>getversi.gc
echo set OWCYEAR=__DATE__>>getversi.gc
icl -nologo -EP getversi.gc>getversi.bat
:nointel
if not exist getversi.bat goto notoolsver
call getversi.bat
del getversi.*
:notoolsver

REM OS specifics

REM setup right COMSPEC for non-standard COMSPEC setting on NT based systems
if not '%OS%' == 'Windows_NT' goto nowinnt
if '%NTDOS%' == '1' goto nowinnt
set COMSPEC=%WINDIR%\system32\cmd.exe
set COPYCMD=/y
set OWCYEAR=%OWCYEAR:~8,4%
:nowinnt

echo Open Watcom build environment (%OWTOOLS% version=%OWTOOLSVER%, CYEAR=%OWCYEAR%)
