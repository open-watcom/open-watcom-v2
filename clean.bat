@echo off
set NUL=NUL
if '%OS%' == 'Windows_NT' set NUL=
if not exist %OWBINDIR%\%OWOBJDIR%\builder.exe goto nobuilder
REM delete the builder and wmake build directories
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.exe del %OWSRCDIR%\builder\%OWOBJDIR%\*.exe
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.obj del %OWSRCDIR%\builder\%OWOBJDIR%\*.obj
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.map del %OWSRCDIR%\builder\%OWOBJDIR%\*.map
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.err del %OWSRCDIR%\builder\%OWOBJDIR%\*.err
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.pdb del %OWSRCDIR%\builder\%OWOBJDIR%\*.pdb
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.sym del %OWSRCDIR%\builder\%OWOBJDIR%\*.sym
if exist %OWSRCDIR%\builder\%OWOBJDIR%\%NUL% rmdir %OWSRCDIR%\builder\%OWOBJDIR%
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.exe del %OWSRCDIR%\wmake\%OWOBJDIR%\*.exe
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.obj del %OWSRCDIR%\wmake\%OWOBJDIR%\*.obj
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.map del %OWSRCDIR%\wmake\%OWOBJDIR%\*.map
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.gh del %OWSRCDIR%\wmake\%OWOBJDIR%\*.gh
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.err del %OWSRCDIR%\wmake\%OWOBJDIR%\*.err
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.pdb del %OWSRCDIR%\wmake\%OWOBJDIR%\*.pdb
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.sym del %OWSRCDIR%\wmake\%OWOBJDIR%\*.sym
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.tmp del %OWSRCDIR%\wmake\%OWOBJDIR%\*.tmp
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\%NUL% rmdir %OWSRCDIR%\wmake\%OWOBJDIR%
REM clean all projects
cd %OWSRCDIR%
builder -i clean
builder -i bootclean
REM Finally delete the builder and wmake executables
if exist %OWBINDIR%\%OWOBJDIR%\builder.exe del %OWBINDIR%\%OWOBJDIR%\builder.exe
goto wmakedel
:nobuilder
echo Cannot find builder - did you run build.bat?
:wmakedel
if exist %OWBINDIR%\%OWOBJDIR%\wmake.exe del %OWBINDIR%\%OWOBJDIR%\wmake.exe
cd %OWROOT%
