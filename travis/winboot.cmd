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
@echo on off
REM ...
if not "%OWTRAVIS_DEBUG%" == "1" goto no_env_info
    echo "INCLUDE=%INCLUDE%"
    echo "LIB=%LIB%"
    echo "LIBPATH=%LIBPATH%"
:no_env_info
REM ...
cd %OWSRCDIR%\wmake
mkdir %OWOBJDIR%
cd %OWOBJDIR%
nmake -f ..\nmake clean
nmake -f ..\nmake
if errorlevel == 1 goto error_exit
    cd %OWSRCDIR%\builder
    mkdir %OWOBJDIR%
    cd %OWOBJDIR%
    %OWBINDIR%\wmake -f ..\binmake clean
    %OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe
    if errorlevel == 1 goto error_exit
    cd %OWSRCDIR%
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder boot
    ) else (
        builder -q boot
    )
:error_exit
