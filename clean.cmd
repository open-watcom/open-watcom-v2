@echo off
if not exist %OWBINDIR%\builder.exe goto no_builder
REM Delete the builder and wmake build directories
if exist %OWSRCDIR%\builder\%OWOBJDIR% rm -rf %OWSRCDIR%\builder\%OWOBJDIR%
if exist %OWSRCDIR%\wmake\%OWOBJDIR% rm -rf %OWSRCDIR%\wmake\%OWOBJDIR%
REM clean all projects
cd %OWSRCDIR%
builder -i clean
builder -i bootclean
REM Finally delete the builder and wmake executables
del %OWBINDIR%\builder.exe
goto wmake_del
:no_builder
echo Cannot find builder - did you run build.bat?
:wmake_del
if exist %OWBINDIR%\rm.exe del %OWBINDIR%\rm.exe
if exist %OWBINDIR%\wmake.exe del %OWBINDIR%\wmake.exe
cd %OWROOT%

