@echo off
REM Script to build the Open Watcom tools
if not exist %OWBINDIR% mkdir %OWBINDIR%
set OWBUILDER_BOOTX_OUTPUT=%OWBINDIR%\bootx.log
if exist %OWBUILDER_BOOTX_OUTPUT% del %OWBUILDER_BOOTX_OUTPUT%
cd %OWSRCDIR%\wmake
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\wmake.exe del %OWBINDIR%\wmake.exe
wmake -f ..\wmake clean >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
wmake -f ..\wmake >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
if errorlevel == 1 goto error_exit
cd %OWSRCDIR%\builder
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\builder.exe del %OWBINDIR%\builder.exe
%OWBINDIR%\wmake -f ..\binmake clean >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
%OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe >>%OWBUILDER_BOOTX_OUTPUT% 2>&1
if errorlevel == 1 goto error_exit
if "%1" == "preboot" goto error_exit
cd %OWSRCDIR%
builder boot
if errorlevel == 1 goto error_exit
if "%1" == "boot" goto error_exit
if "%1" == "" goto build
builder %1
goto error_exit
:build
builder build
:error_exit
cd %OWROOT%
