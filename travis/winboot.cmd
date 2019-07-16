@set OWECHO=off
@if "$OWTRAVIS_DEBUG" == "1" set OWECHO=on
@echo %OWECHO%
SETLOCAL EnableExtensions
REM Script to build the Open Watcom bootstrap tools
REM By Microsoft Visual Studio
REM ...
set OWROOT=%CD%
REM ...
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
REM ...
@echo %OWECHO%
REM ...
call cmnvars.bat
REM ...
@echo %OWECHO%
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
cd %OWSRCDIR%\wmake
mkdir %OWOBJDIR%
cd %OWOBJDIR%
nmake -f ..\nmake clean
nmake -f ..\nmake
if not errorlevel == 1 (
    cd %OWSRCDIR%\builder
    mkdir %OWOBJDIR%
    cd %OWOBJDIR%
    %OWBINDIR%\wmake -f ..\binmake clean
    %OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe
    if not errorlevel == 1 (
        cd %OWSRCDIR%
        if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
            builder boot
        ) else (
            builder -q boot
        )
    )
)
set RC=%ERRORLEVEL%
REM sleep 3
ping -n 3 127.0.0.1 >NUL
exit %RC%
