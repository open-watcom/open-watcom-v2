@echo on
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
@echo on
cd %OWROOT%
call cmnvars.bat
@echo on
REM ...
if "%OWTRAVIS_DEBUG%" == "1" (
    echo "INCLUDE=%INCLUDE%"
    echo "LIB=%LIB%"
    echo "LIBPATH=%LIBPATH%"
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
