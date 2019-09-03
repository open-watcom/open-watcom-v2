@set OWECHO=off
@if "%OWDEBUG%" == "1" set OWECHO=on
@echo %OWECHO%
SETLOCAL EnableExtensions
REM Script to build the Open Watcom bootstrap tools
REM By Microsoft Visual Studio
REM ...
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
REM ...
@echo %OWECHO%
REM ...
REM setup DOSBOX
REM ...
set OWDOSBOXPATH=%OWCIBIN%
set OWDOSBOX=dosbox.exe
REM ...
REM setup Help Compilers
REM ...
set OWGHOSTSCRIPTPATH=%OWCIBIN%
set OWWIN95HC=%OWCIBIN%\hcrtf.exe
set OWHHC=%OWCIBIN%\hhc.exe
REM ...
call %OWROOT%\cmnvars.bat
REM ...
@echo %OWECHO%
REM ...
if "%OWDEBUG%" == "1" (
    echo INCLUDE="%INCLUDE%"
    echo LIB="%LIB%"
    echo LIBPATH="%LIBPATH%"
)
REM ...
set RC=0
cd %OWSRCDIR%
if "%OWBUILD_STAGE%" == "boot" (
    mkdir %OWBINDIR%
    mkdir %OWSRCDIR%\wmake\%OWOBJDIR%
    cd %OWSRCDIR%\wmake\%OWOBJDIR%
    nmake -f ..\nmake
    set RC=%ERRORLEVEL%
    if not %RC% == 1 (
    	mkdir %OWSRCDIR%\builder\%OWOBJDIR%
    	cd %OWSRCDIR%\builder\%OWOBJDIR%
        %OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe
        set RC=%ERRORLEVEL%
        if not %RC% == 1 (
            cd %OWSRCDIR%
            builder boot
            set RC=%ERRORLEVEL%
        )
    )
)
if "%OWBUILD_STAGE%" == "build" (
    builder rel
    set RC=%ERRORLEVEL%
)
if "%OWBUILD_STAGE%" == "tests" (
REM    builder test %OWTESTTARGET%
REM    set RC=%ERRORLEVEL%
)
if "%OWBUILD_STAGE%" == "docs" (
    REM register all Help Compilers DLL's
    regsvr32 -u -s itcc.dll
    regsvr32 -s %OWCIBIN%\itcc.dll
    builder docs %OWDOCTARGET%
    set RC=%ERRORLEVEL%
)
if "%OWBUILD_STAGE%" == "inst" (
    builder install os_nt cpu_x64
    set RC=%ERRORLEVEL%
)
cd %OWROOT%
exit %RC%
