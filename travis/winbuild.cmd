@echo off
SETLOCAL EnableExtensions
REM Script to build the Open Watcom bootstrap tools
REM By Microsoft Visual Studio
REM ...
set OWROOT=%CD%
REM ...
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
REM ...
call cmnvars.bat
REM
REM setup DOSBOX
REM
set OWDOSBOX=%OWROOT%\travis\dosbox\dosbox.exe
REM ...
if "%OWTRAVIS_DEBUG%" == "1" (
    echo INCLUDE="%INCLUDE%"
    echo LIB="%LIB%"
    echo LIBPATH="%LIBPATH%"
)
REM ...
xcopy /S /D /R buildx $OWSRCDIR\
cd %OWSRCDIR%
if "%OWTRAVISJOB%" == "BUILD" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build
    ) else (
        builder -q build
    	if not errorlevel == 1 (
            set OWRELROOT=%OWROOT%\test
            builder -q cprel
        )
    )
)
if "%OWTRAVISJOB%" == "BUILD-1" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build1
    ) else (
        builder -q build1
    	if not errorlevel == 1 (
            set OWRELROOT=%OWROOT%\test
            builder -q cprel1
        )
    )
)
if "%OWTRAVISJOB%" == "BUILD-2" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build2
    ) else (
        builder -q build2
    	if not errorlevel == 1 (
            set OWRELROOT=%OWROOT%\test
            builder -q cprel2
        )
    )
)
if "%OWTRAVISJOB%" == "BUILD-3" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build3
    ) else (
        builder -q build3
    	if not errorlevel == 1 (
            set OWRELROOT=%OWROOT%\test
            builder -q cprel3
        )
    )
)
set RC=%ERRORLEVEL%
cd %OWROOT%
if not errorlevel == 1 xcopy /S /D /R $OWSRCDIR buildx\
echo ERRORLEVEL=%RC%
Exit %RC%
