@echo off
REM Script to build the Open Watcom tools
set NUL=NUL
if '%OS%' == 'Windows_NT' set NUL=
if not exist %OWBINDIR%\%OWOBJDIR%\%NUL% mkdir %OWBINDIR%\%OWOBJDIR%
set OWBUILDER_BOOTX_OUTPUT=%OWBINDIR%\%OWOBJDIR%\bootx.log
if '%OS%' == 'Windows_NT' set "OWBUILDER_REDIR_ERROUT=2>&1"
if exist %OWBUILDER_BOOTX_OUTPUT% del %OWBUILDER_BOOTX_OUTPUT%
cd %OWSRCDIR%\wmake
if not exist %OWOBJDIR%\%NUL% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\%OWOBJDIR%\wmake.exe del %OWBINDIR%\%OWOBJDIR%\wmake.exe
if '%OWTOOLS%' == 'VISUALC' goto visualc
if '%OWTOOLS%' == 'INTEL' goto intel
wmake -f ..\wmake clean >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
wmake -f ..\wmake >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
if errorlevel == 1 goto exiterr
goto mkbuilder
:intel
:visualc
nmake -f ..\nmake clean >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
nmake -f ..\nmake >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
if errorlevel == 1 goto exiterr
:mkbuilder
cd %OWSRCDIR%\builder
if not exist %OWOBJDIR%\%NUL% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\%OWOBJDIR%\builder.exe del %OWBINDIR%\%OWOBJDIR%\builder.exe
%OWBINDIR%\%OWOBJDIR%\wmake -f ..\binmake clean >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
%OWBINDIR%\%OWOBJDIR%\wmake -f ..\binmake bootstrap=1 >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
if errorlevel == 1 goto exiterr
if "%1" == "preboot" goto exiterr
cd %OWSRCDIR%
builder boot
if errorlevel == 1 goto exiterr
if "%1" == "boot" goto exiterr
if "%1" == "" goto build
builder %1
goto exiterr
:build
builder build
:exiterr
cd %OWROOT%
