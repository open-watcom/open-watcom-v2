@echo off
REM Script to build the Open Watcom tools
set "OWBUILDER_OUTPUT=>>%OWROOT%\bootx.log"
cd %OWSRCDIR%\wmake
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\wmake.exe del %OWBINDIR%\wmake.exe
if '%OWUSENATIVE%' == '1' goto native_tools
wmake -f ..\wmake clean %OWBUILDER_OUTPUT%
wmake -f ..\wmake %OWBUILDER_OUTPUT%
if errorlevel == 1 goto error_exit
goto make_builder
:native_tools
nmake -nologo -f ..\nmake clean %OWBUILDER_OUTPUT%
nmake -nologo -f ..\nmake %OWBUILDER_OUTPUT%
if errorlevel == 1 goto error_exit
:make_builder
cd %OWSRCDIR%\builder
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\builder.exe del %OWBINDIR%\builder.exe
%OWBINDIR%\wmake -f ..\binmake clean %OWBUILDER_OUTPUT%
%OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe rm.exe %OWBUILDER_OUTPUT%
if errorlevel == 1 goto error_exit
cd %OWSRCDIR%
builder boot
if errorlevel == 1 goto error_exit
builder build
:error_exit
cd %OWROOT%
