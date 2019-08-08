@set OWECHO=off
@if "%OWTRAVIS_DEBUG%" == "1" set OWECHO=on
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
REM setup DOSBOX
REM
set OWDOSBOXPATH=%OWROOT%\travis\dosbox
set OWDOSBOX=dosbox.exe
set SDL_VIDEODRIVER=dummy
REM
if "%TRAVIS_BUILD_STAGE_NAME%" == "Documentation" (
    set OWGHOSTSCRIPTPATH=%OWROOT%\travis\gs927w64
    set OWWIN95HC=%OWROOT%\travis\hcw\hcrtf.exe
    set OWHHC=%OWROOT%\travis\hhc\hhc.exe
)
REM ...
call cmnvars.bat
REM ...
@echo %OWECHO%
REM ...
if "%OWTRAVIS_ENV_DEBUG%" == "1" (
REM    set
) else (
    if "%OWTRAVIS_DEBUG%" == "1" (
        echo INCLUDE="%INCLUDE%"
        echo LIB="%LIB%"
        echo LIBPATH="%LIBPATH%"
    )
)
REM ...
cd %OWSRCDIR%
if "%TRAVIS_BUILD_STAGE_NAME%" == "Bootstrap" (
    cd wmake
    mkdir %OWOBJDIR%
    cd %OWOBJDIR%
    nmake -f ..\nmake clean >>%OWBINDIR%\bootx.log 2>&1
    nmake -f ..\nmake >>%OWBINDIR%\bootx.log 2>&1
    if not errorlevel == 1 (
        cd %OWSRCDIR%\builder
        mkdir %OWOBJDIR%
        cd %OWOBJDIR%
        %OWBINDIR%\wmake -f ..\binmake clean >>%OWBINDIR%\bootx.log 2>&1
        %OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe >>%OWBINDIR%\bootx.log 2>&1
        if not errorlevel == 1 (
            cd %OWSRCDIR%
            if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
                builder boot
            ) else (
                builder -q boot
            )
        )
    )
)
if "%TRAVIS_BUILD_STAGE_NAME%" == "Build1" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel1
    ) else (
        builder -q rel1
    )
)
if "%TRAVIS_BUILD_STAGE_NAME%" == "Build2" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel2
    ) else (
        builder -q rel2
    )
)
if "%TRAVIS_BUILD_STAGE_NAME%" == "Build3" (
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder rel3
    ) else (
        builder -q rel3
    )
)
if "%TRAVIS_BUILD_STAGE_NAME%" == "Documentation" (
    REM register all Help Compilers DLL's
    regsvr32 -u -s itcc.dll
    regsvr32 -s %OWROOT%\travis\hhc\itcc.dll
    if "%TRAVIS_EVENT_TYPE%" == "pull_request" (
        builder docs
    ) else (
        builder -q docs
    )
)
if "%TRAVIS_BUILD_STAGE_NAME%" == "Release windows" (
    builder missing
    builder install os_nt cpu_x64
)
set RC=%ERRORLEVEL%
REM sleep 3
ping -n 3 127.0.0.1 >NUL
exit %RC%
