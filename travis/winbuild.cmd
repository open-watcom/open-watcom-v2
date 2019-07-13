@set OWECHO_OFF=off
@if "$OWTRAVIS_DEBUG" == "1" set OWECHO_OFF=on
@echo %OWECHO_OFF%
SETLOCAL EnableExtensions
REM Script to build the Open Watcom bootstrap tools
REM By Microsoft Visual Studio
REM ...
set OWROOT=%CD%
REM ...
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
REM ...
@echo %OWECHO_OFF%
REM ...
call cmnvars.bat
REM ...
@echo %OWECHO_OFF%
REM
REM setup DOSBOX
REM
set OWDOSBOX=%OWROOT%\travis\dosbox\dosbox.exe
REM ...
if "%OWTRAVIS_ENV_DEBUG%" == "1" (
    set
) else (
    if "%OWTRAVIS_DEBUG%" == "1" (
        echo INCLUDE="%INCLUDE%"
        echo LIB="%LIB%"
        echo LIBPATH="%LIBPATH%"
    )
)
REM ...
cd %OWSRCDIR%
if "%OWTRAVISJOB%" == "BUILD" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel
    ) else (
        builder -q rel
    )
)
if "%OWTRAVISJOB%" == "BUILD-1" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel1
    ) else (
        builder -q rel1
    )
)
if "%OWTRAVISJOB%" == "BUILD-2" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel2
    ) else (
        builder -q rel2
    )
)
if "%OWTRAVISJOB%" == "BUILD-3" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel3
    ) else (
        builder -q rel3
    )
)
set RC=%ERRORLEVEL%
REM sleep 3
ping -n 3 127.0.0.1 >NUL
exit %RC%
