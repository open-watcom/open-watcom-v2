@set OWECHO=off
@if "%OWDEBUG%" == "1" set OWECHO=on
@echo %OWECHO%
@if "%OWDEBUG%" == "1" set | sort
REM ...
REM Script to build the Open Watcom bootstrap tools
REM ...
if "%OWTOOLS%" == "VISUALC" (
    if "%1" == "vs2017" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
    if "%1" == "vs2019" call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
    if "%1" == "vs2022" call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
)
REM ...
@echo %OWECHO%
REM ...
REM setup Help Compilers
REM ...
if "%OWBUILD_STAGE%" == "docs" (
    set OWGHOSTSCRIPTPATH=%OWROOT%\ci\ntx64
    set OWWIN95HC=%OWROOT%\ci\nt386\hcrtf.exe
    set OWHHC=%OWROOT%\ci\nt386\hhc.exe
)
REM ...
call %OWROOT%\cmnvars.bat
REM ...
set OWVERBOSE=1
REM ...
@echo %OWECHO%
REM ...
if "%OWDEBUG%" == "1" (
    echo INCLUDE="%INCLUDE%"
    echo LIB="%LIB%"
    echo LIBPATH="%LIBPATH%"
)
REM ...
SETLOCAL EnableDelayedExpansion
set RC=0
cd %OWROOT%\bld
if "%OWBUILD_STAGE%" == "boot" (
    mkdir %OWROOT%\build\%OWOBJDIR%
    mkdir %OWROOT%\bld\wmake\%OWOBJDIR%
    cd %OWROOT%\bld\wmake\%OWOBJDIR%
    if "%OWTOOLS%" == "WATCOM" (
        wmake -m -f ..\wmake
    ) else (
        nmake -f ..\nmake
    )
    set RC=!ERRORLEVEL!
    if not %RC% == 1 (
        mkdir %OWROOT%\bld\builder\%OWOBJDIR%
        cd %OWROOT%\bld\builder\%OWOBJDIR%
        %OWROOT%\build\%OWOBJDIR%\wmake -f ..\preboot
        set RC=!ERRORLEVEL!
        if not %RC% == 1 (
            if "%OWDOCTARGET%" == "" (
                cd %OWROOT%\bld
                builder boot
                set RC=!ERRORLEVEL!
            ) else (
                cd %OWROOT%\bld\watcom
                builder boot
                cd %OWROOT%\bld\builder
                builder boot
                cd %OWROOT%\bld\whpcvt
                builder boot
                cd %OWROOT%\bld\bmp2eps
                builder boot
                set RC=!ERRORLEVEL!
            )
        )
    )
)
if "%OWBUILD_STAGE%" == "build" (
    builder rel
    set RC=!ERRORLEVEL!
)
if "%OWBUILD_STAGE%" == "tests" (
REM    builder test %OWTESTTARGET%
REM    set RC=!ERRORLEVEL!
)
if "%OWBUILD_STAGE%" == "docs" (
    REM register all Help Compilers DLL's
    regsvr32 -u -s itcc.dll
    regsvr32 -s %OWROOT%\ci\nt386\itcc.dll
    builder docs %OWDOCTARGET%
    set RC=!ERRORLEVEL!
)
if "%OWBUILD_STAGE%" == "inst" (
    builder install %OWINSTTARGET%
    set RC=!ERRORLEVEL!
)
cd %OWROOT%
exit %RC%
