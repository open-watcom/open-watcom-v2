@echo off
REM Script to build the Open Watcom tools
set NUL=NUL
if '%OS%' == 'Windows_NT' set NUL=
if not exist "%OWROOT%\build\%OWOBJDIR%\%NUL%" mkdir "%OWROOT%\build\%OWOBJDIR%"
set OWBUILDER_BOOTX_OUTPUT=%OWROOT%\build\%OWOBJDIR%\bootx.log
if '%OS%' == 'Windows_NT' set "OWBUILDER_REDIR_ERROUT=2>&1"
if exist "%OWBUILDER_BOOTX_OUTPUT%" del "%OWBUILDER_BOOTX_OUTPUT%"
cd "%OWROOT%\bld\wmake"
if not exist "%OWOBJDIR%\%NUL%" mkdir "%OWOBJDIR%"
cd "%OWOBJDIR%"
if exist "%OWROOT%\build\%OWOBJDIR%\wmake.exe" del "%OWROOT%\build\%OWOBJDIR%\wmake.exe"
if '%OWTOOLS%' == 'VISUALC' goto visualc
if '%OWTOOLS%' == 'INTEL' goto intel
wmake -m -f ..\wmake clean >>"%OWBUILDER_BOOTX_OUTPUT%" %OWBUILDER_REDIR_ERROUT%
wmake -m -f ..\wmake >>"%OWBUILDER_BOOTX_OUTPUT%" %OWBUILDER_REDIR_ERROUT%
if errorlevel == 1 goto exiterr
goto mkbuilder
:intel
:visualc
nmake -f ..\nmake clean >>"%OWBUILDER_BOOTX_OUTPUT%" %OWBUILDER_REDIR_ERROUT%
nmake -f ..\nmake >>"%OWBUILDER_BOOTX_OUTPUT%" %OWBUILDER_REDIR_ERROUT%
if errorlevel == 1 goto exiterr
:mkbuilder
cd "%OWROOT%\bld\builder"
if not exist "%OWOBJDIR%\%NUL%" mkdir "%OWOBJDIR%"
cd "%OWOBJDIR%"
if exist "%OWROOT%\build\%OWOBJDIR%\builder.exe" del "%OWROOT%\build\%OWOBJDIR%\builder.exe"
"%OWROOT%\build\%OWOBJDIR%\wmake" -f ..\preboot clean >>"%OWBUILDER_BOOTX_OUTPUT%" %OWBUILDER_REDIR_ERROUT%
"%OWROOT%\build\%OWOBJDIR%\wmake" -f ..\preboot >>"%OWBUILDER_BOOTX_OUTPUT%" %OWBUILDER_REDIR_ERROUT%
if errorlevel == 1 goto exiterr
if '%1' == 'preboot' goto exiterr
cd "%OWROOT%\bld"
builder boot
if errorlevel == 1 goto exiterr
if '%1' == 'boot' goto exiterr
if '%1' == '' goto build
builder %1
goto exiterr
:build
builder build
:exiterr
cd "%OWROOT%"
