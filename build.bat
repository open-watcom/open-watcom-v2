@echo off
REM Script to build the Open Watcom tools
set OWBUILDER_BOOTX_OUTPUT=%OWROOT%\bootx.log
if exist %OWBUILDER_BOOTX_OUTPUT% del %OWBUILDER_BOOTX_OUTPUT%
cd %OWSRCDIR%\wmake
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\wmake.exe del %OWBINDIR%\wmake.exe
if '%OWTOOLS%' == 'VISUALC' goto visualc_tools
if '%OWTOOLS%' == 'INTEL' goto intel_tools
wmake -f ..\wmake clean >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
wmake -f ..\wmake >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
if errorlevel == 1 goto error_exit
goto make_builder
:intel_tools
:visualc_tools
nmake -f ..\nmake clean >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
nmake -f ..\nmake >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
if errorlevel == 1 goto error_exit
:make_builder
cd %OWSRCDIR%\builder
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\builder.exe del %OWBINDIR%\builder.exe
%OWBINDIR%\wmake -f ..\binmake clean >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
%OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
if errorlevel == 1 goto error_exit
cd %OWSRCDIR%
builder boot
if errorlevel == 1 goto error_exit
builder build
:error_exit
cd %OWROOT%
