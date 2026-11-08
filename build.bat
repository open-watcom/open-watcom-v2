@echo off
REM Script to build the Open Watcom tools
set NUL=NUL
if '%OS%' == 'Windows_NT' set NUL=
set OWBUILD_LOG=..\..\..\build\%OWOBJDIR%\bootx.log
if '%OS%' == 'Windows_NT' set "OWBUILD_REDIR_ERROUT=2>&1"
cd "%OWROOT%\build"
if not exist %OWOBJDIR%\%NUL% goto do_dir_build
if exist %OWOBJDIR%\bootx.log del %OWOBJDIR%\bootx.log
if exist %OWOBJDIR%\wmake.exe del %OWOBJDIR%\wmake.exe
if exist %OWOBJDIR%\builder.exe del %OWOBJDIR%\builder.exe
goto build_ok
:do_dir_build
mkdir %OWOBJDIR%
:build_ok
cd "%OWROOT%\bld\wmake"
if not exist %OWOBJDIR%\%NUL% mkdir %OWOBJDIR%
cd %OWOBJDIR%
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
if not exist %OWOBJDIR%\%NUL% mkdir %OWOBJDIR%
cd %OWOBJDIR%
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
