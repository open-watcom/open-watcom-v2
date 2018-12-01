@echo on off
SETLOCAL EnableExtensions
REM Script to build the Open Watcom bootstrap tools
REM By Microsoft Visual Studio
REM ...
REM Remove NMAKE macros wrongly defined by Travis
set CC=
REM ...
set OWROOT=%CD%
REM ...
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
REM ...
@echo on off
cd %OWROOT%
call cmnvars.bat
set OWDOSBOX=%OWROOT%\travis\dosbox\dosbox.exe
REM ...
if not "%OWTRAVIS_DEBUG%" == "1" goto no_env_info
    echo "INCLUDE=%INCLUDE%"
    echo "LIB=%LIB%"
    echo "LIBPATH=%LIBPATH%"
:no_env_info
REM ...
@echo on off
cd %OWSRCDIR%
if not "%OWTRAVISJOB%" == "BUILD" goto no_build
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build
    ) else (
        builder -q build
	    set OWRELROOT=%OWROOT%\test
	    builder -q cprel
    )
:no_build
if not "%OWTRAVISJOB%" == "BUILD-1" goto no_build1
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build1
    ) else (
        builder -q build1
	    set OWRELROOT=%OWROOT%\test
	    builder -q cprel1
    )
:no_build1
if not "%OWTRAVISJOB%" == "BUILD-2" goto no_build2
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build2
    ) else (
        builder -q build2
	    set OWRELROOT=%OWROOT%\test
	    builder -q cprel2
    )
:no_build2
