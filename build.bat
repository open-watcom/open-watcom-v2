@echo off
REM Script to build the Open Watcom tools
if not exist %OWBINDIR%\%NUL% mkdir %OWBINDIR%
set OWBUILDER_BOOTX_OUTPUT=%OWBINDIR%\bootx.log
set NUL=NUL
if not '%OS%' == 'Windows_NT' goto skip_errout
set OWBUILDER_REDIR_ERROUT=2^>^&1
set NUL=
:skip_errout
if exist %OWBUILDER_BOOTX_OUTPUT% del %OWBUILDER_BOOTX_OUTPUT%
cd %OWSRCDIR%\wmake
if not exist %OWOBJDIR%\%NUL% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\wmake.exe del %OWBINDIR%\wmake.exe
if '%OWTOOLS%' == 'VISUALC' goto visualc_tools
if '%OWTOOLS%' == 'INTEL' goto intel_tools
wmake -f ..\wmake clean >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
wmake -f ..\wmake >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
if errorlevel == 1 goto error_exit
goto make_builder
:intel_tools
:visualc_tools
nmake -f ..\nmake clean >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
nmake -f ..\nmake >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
if errorlevel == 1 goto error_exit
:make_builder
cd %OWSRCDIR%\builder
if not exist %OWOBJDIR%\%NUL% mkdir %OWOBJDIR%
cd %OWOBJDIR%
if exist %OWBINDIR%\builder.exe del %OWBINDIR%\builder.exe
%OWBINDIR%\wmake -f ..\binmake clean >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
%OWBINDIR%\wmake -f ..\binmake bootstrap=1 builder.exe >>%OWBUILDER_BOOTX_OUTPUT% %OWBUILDER_REDIR_ERROUT%
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
