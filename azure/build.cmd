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
set OWGHOSTSCRIPTPATH=%OWROOT%\travis\gs927w64
set OWWIN95HC=%OWROOT%\travis\hcw\hcrtf.exe
set OWHHC=%OWROOT%\travis\hhc\hhc.exe
REM ...
set OWTOOLS=VISUALC
REM ...
call cmnvars.bat
REM ...
@echo %OWECHO%
REM ...
mkdir %OWBINDIR%
@echo INCLUDE="%INCLUDE%"
@echo LIB="%LIB%"
@echo LIBPATH="%LIBPATH%"
REM ...
set RC=0
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
builder -q rel
REM ...
REM Update Build
REM ...
REM Tests
set OWTOOLS=WATCOM
set WATCOM=%OWROOT%\test
set PATH=%WATCOM%\binnt;%WATCOM%\binw;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\nt
set LIB=
cd %OWSRCDIR%\wasmtest
builder -q -i testclean
builder -q -i test
set RC=%ERRORLEVEL%
cat result.log
cd %OWSRCDIR%\ctest
builder -q -i testclean
builder -q -i test
set RC=%ERRORLEVEL%
cat result.log
cd %OWSRCDIR%\f77test
builder -q -i testclean
builder -q -i test
set RC=%ERRORLEVEL%
cat result.log
cd %OWSRCDIR%\plustest
builder -q -i testclean
builder -q -i test
set RC=%ERRORLEVEL%
cat result.log
cd %OWSRCDIR%\clibtest
builder -q -i testclean
builder -q -i test
set RC=%ERRORLEVEL%
cat result.log
REM ...
REM Documentation
REM ...
set OWTOOLS=VISUALC
REM ...
REM register all Help Compilers DLL's
regsvr32 -u -s itcc.dll
regsvr32 -s %OWROOT%\travis\hhc\itcc.dll
builder -q docs
set RC=%ERRORLEVEL%
REM ...
REM Release
REM ...
builder missing
builder install os_nt cpu_x64
set RC=%ERRORLEVEL%
cd %OWROOT%
exit %RC%
