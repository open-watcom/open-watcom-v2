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
        builder build
    ) else (
        builder -q build
    )
)
if "%OWTRAVISJOB%" == "BUILD-1" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build1
    ) else (
        builder -q build1
    )
    xcopy /Y /S /H /R /D %OWSRCDIR% %OWROOT%\buildx\
)
if "%OWTRAVISJOB%" == "BUILD-2" (
    xcopy /Y /S /H /R /D %OWROOT%\buildx %OWSRCDIR%\
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build2
    ) else (
        builder -q build2
    )
    xcopy /Y /S /H /R /D %OWSRCDIR% %OWROOT%\buildx\
)
if "%OWTRAVISJOB%" == "BUILD-3" (
    xcopy /Y /S /H /R /D %OWROOT%\buildx %OWSRCDIR%\
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder build3
    ) else (
        builder -q build3
    )
    xcopy /Y /S /H /R /D %OWSRCDIR% %OWROOT%\buildx\
)
echo ERRORLEVEL=%ERRORLEVEL%
exit %ERRORLEVEL%
