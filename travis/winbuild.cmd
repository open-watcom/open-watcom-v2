@echo off
SETLOCAL EnableExtensions
REM Script to build the Open Watcom bootstrap tools
REM By Microsoft Visual Studio
REM ...
REM Remove NMAKE macros wrongly defined by Travis
set CC=
set CFLAGS=
set CPP=
set CPPFLAGS=
set CXX=
set CXXFLAGS=
REM ...
set OWROOT=%CD%
REM ...
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
REM ...
call cmnvars.bat
REM ...
cd %OWSRCDIR%
if "%OWTRAVISJOB%" == "BUILD" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build
    ) else (
        builder -q build
	set OWRELROOT=%OWROOT%\test
	builder -q cprel
    )
}
if "%OWTRAVISJOB%" == "BUILD-1" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build1
    ) else (
        builder -q build1
	set OWRELROOT=%OWROOT%\test
	builder -q cprel1
    )
}
if "%OWTRAVISJOB%" == "BUILD-2" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build2
    ) else (
        builder -q build2
	set OWRELROOT=%OWROOT%\test
	builder -q cprel2
    )
}
