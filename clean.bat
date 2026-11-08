@echo off
cd "%OWROOT%"
if not exist build\%OWOBJDIR%\builder.exe goto nobuilder
REM delete the builder and wmake build directories
cd "%OWROOT%\bld\builder"
builder -i bootclean
cd "%OWROOT%\bld\wmake"
builder -i bootclean
REM clean all projects
cd "%OWROOT%\bld"
builder -i clean
builder -i bootclean
cd "%OWROOT%"
REM Finally delete the builder and wmake executables
if exist build\%OWOBJDIR%\builder.exe del build\%OWOBJDIR%\builder.exe
goto wmakedel
:nobuilder
echo Cannot find builder - did you run build.bat?
:wmakedel
if exist build\%OWOBJDIR%\wmake.exe del build\%OWOBJDIR%\wmake.exe
