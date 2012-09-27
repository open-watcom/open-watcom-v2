@echo off
REM Script to build the Open Watcom tools
cd %OWSRCDIR%\wmake
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
del %OWBINDIR%\wmake.exe
wmake -f ..\wmake clean
wmake -f ..\wmake
cd %OWSRCDIR%\builder
if not exist %OWOBJDIR% mkdir %OWOBJDIR%
cd %OWOBJDIR%
del %OWBINDIR%\builder.exe
%OWBINDIR%\wmake -f ..\binmake clean
%OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe rm.exe
cd %OWSRCDIR%
builder boot
if not errorlevel == 1 builder build
cd %OWROOT%
