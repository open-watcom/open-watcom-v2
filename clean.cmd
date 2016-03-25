@echo off
if not exist %OWBINDIR%\builder.exe goto no_builder
REM delete the builder and wmake build directories
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.exe del %OWSRCDIR%\builder\%OWOBJDIR%\*.exe
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.obj del %OWSRCDIR%\builder\%OWOBJDIR%\*.obj
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.map del %OWSRCDIR%\builder\%OWOBJDIR%\*.map
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.err del %OWSRCDIR%\builder\%OWOBJDIR%\*.err
if exist %OWSRCDIR%\builder\%OWOBJDIR%\*.sym del %OWSRCDIR%\builder\%OWOBJDIR%\*.sym
if exist %OWSRCDIR%\builder\%OWOBJDIR% rmdir %OWSRCDIR%\builder\%OWOBJDIR%
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.exe del %OWSRCDIR%\wmake\%OWOBJDIR%\*.exe
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.obj del %OWSRCDIR%\wmake\%OWOBJDIR%\*.obj
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.map del %OWSRCDIR%\wmake\%OWOBJDIR%\*.map
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.gh del %OWSRCDIR%\wmake\%OWOBJDIR%\*.gh
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.err del %OWSRCDIR%\wmake\%OWOBJDIR%\*.err
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.sym del %OWSRCDIR%\wmake\%OWOBJDIR%\*.sym
if exist %OWSRCDIR%\wmake\%OWOBJDIR%\*.tmp del %OWSRCDIR%\wmake\%OWOBJDIR%\*.tmp
if exist %OWSRCDIR%\wmake\%OWOBJDIR% rmdir %OWSRCDIR%\wmake\%OWOBJDIR%
REM clean all projects
cd %OWSRCDIR%
builder -i clean
builder -i bootclean
REM Finally delete the builder and wmake executables
if exist %OWBINDIR%\builder.exe del %OWBINDIR%\builder.exe
goto wmake_del
:no_builder
echo Cannot find builder - did you run build.bat?
:wmake_del
if exist %OWBINDIR%\wmake.exe del %OWBINDIR%\wmake.exe
cd %OWROOT%

