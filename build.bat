@echo off
REM Script to build the Open Watcom tools
if '%OS%' == 'Windows_NT' set "OWBUILD_REDIR_ERROUT=2>&1"
cd "%OWROOT%\build"
cd %OWOBJDIR% > NUL %OWBUILD_REDIR_ERROUT%
if not errorlevel 1 goto build_clean
mkdir %OWOBJDIR%
cd %OWOBJDIR%
goto build_ok
:build_clean
if exist bootx.log del bootx.log
if exist wmake.exe del wmake.exe
if exist builder.exe del builder.exe
:build_ok
set OWBUILD_LOG=..\..\..\build\%OWOBJDIR%\bootx.log
cd "%OWROOT%\bld\wmake"
cd %OWOBJDIR% > NUL %OWBUILD_REDIR_ERROUT%
if not errorlevel 1 goto wmake_ok
mkdir %OWOBJDIR%
cd %OWOBJDIR%
:wmake_ok
if '%OWTOOLS%' == 'VISUALC' goto visualc
if '%OWTOOLS%' == 'INTEL' goto intel
wmake -m -f ..\wmake clean >>%OWBUILD_LOG% %OWBUILD_REDIR_ERROUT%
wmake -m -f ..\wmake >>%OWBUILD_LOG% %OWBUILD_REDIR_ERROUT%
if errorlevel == 1 goto exiterr
goto mkbuilder
:intel
:visualc
nmake -f ..\nmake clean >>%OWBUILD_LOG% %OWBUILD_REDIR_ERROUT%
nmake -f ..\nmake >>%OWBUILD_LOG% %OWBUILD_REDIR_ERROUT%
if errorlevel == 1 goto exiterr
:mkbuilder
cd "%OWROOT%\bld\builder"
cd %OWOBJDIR% > NUL %OWBUILD_REDIR_ERROUT%
if not errorlevel 1 goto builder_ok
mkdir %OWOBJDIR%
cd %OWOBJDIR%
:builder_ok
"%OWROOT%\build\%OWOBJDIR%\wmake" -f ..\preboot clean >>%OWBUILD_LOG% %OWBUILD_REDIR_ERROUT%
"%OWROOT%\build\%OWOBJDIR%\wmake" -f ..\preboot >>%OWBUILD_LOG% %OWBUILD_REDIR_ERROUT%
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
