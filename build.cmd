@echo off
REM Script to build the Open Watcom tools
cd %OWSRCDIR%\wmake
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\wmake.exe del %OWBINDIR%\wmake.exe
wmake -f ..\wmake clean
wmake -f ..\wmake
if errorlevel == 1 goto error_exit
cd %OWSRCDIR%\builder
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\builder.exe del %OWBINDIR%\builder.exe
%OWBINDIR%\wmake -f ..\binmake clean
%OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe
cd %OWSRCDIR%
builder boot
if errorlevel == 1 goto error_exit
builder build
:error_exit
cd %OWROOT%
