@set OWECHO=off
@if "%OWAZURE_DEBUG%" == "1" set OWECHO=on
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
if "%OWAZURE_STAGE_NAME%" == "Documentation" (
    set OWGHOSTSCRIPTPATH=%OWROOT%\travis\gs927w64
    set OWWIN95HC=%OWROOT%\travis\hcw\hcrtf.exe
    set OWHHC=%OWROOT%\travis\hhc\hhc.exe
)
if "%OWAZURE_STAGE_NAME%" == "Tests" (
    set WATCOM=%OWROOT%\test
    set PATH=%WATCOM%\binnt;$PATH
    set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt
    set LIB=
)
REM ...
call cmnvars.bat
set
REM ...
@echo %OWECHO%
REM ...
echo INCLUDE="%INCLUDE%"
echo LIB="%LIB%"
echo LIBPATH="%LIBPATH%"
REM ...
if "%OWAZURE_STAGE_NAME%" == "Bootstrap" (
    mkdir %OWBINDIR%
    cd %OWSRCDIR%
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
	    builder -q boot
        )
    )
)
cd %OWSRCDIR%
if "%OWAZURE_STAGE_NAME%" == "Build" (
    builder -q rel
)
if "%OWAZURE_STAGE_NAME%" == "Build1" (
    builder -q rel1
)
if "%OWAZURE_STAGE_NAME%" == "Build2" (
    builder -q rel2
)
if "%OWAZURE_STAGE_NAME%" == "Build3" (
    builder -q rel3
)
if "%OWAZURE_STAGE_NAME%" == "Tests" (
    if "%OWAZURETEST%" == "WASM" (
	cd %OWSRCDIR%\wasmtest
    }
    if "%OWAZURETEST%" == "C" (
	cd %OWSRCDIR%\ctest
    }
    if "%OWAZURETEST%" == "F77" (
	cd %OWSRCDIR%\f77test
    }
    if "%OWAZURETEST%" == "CXX" (
	cd %OWSRCDIR%\plustest
    }
    if "%OWAZURETEST%" == "CRTL" (
	cd %OWSRCDIR%\clibtest
    }
    builder -q -i testclean
    builder -q -i test
    cat result.log
)
if "%OWAZURE_STAGE_NAME%" == "Documentation" (
    REM register all Help Compilers DLL's
    regsvr32 -u -s itcc.dll
    regsvr32 -s %OWROOT%\travis\hhc\itcc.dll
    builder -q docs
)
if "%OWAZURE_STAGE_NAME%" == "Release windows" (
    builder missing
    builder install os_nt cpu_x64
)
set RC=%ERRORLEVEL%
cd %OWROOT%
exit %RC%
