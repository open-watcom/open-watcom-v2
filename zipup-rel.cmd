@echo off
rem Check for command line option
if x%1 == x goto Usage

pushd
set LABEL=open_watcom_%1
set PREFIX=open_watcom_%1
set P4OPT=-f
set ARCHIVES=\archives
cd %ARCHIVES%
del /q %PREFIX%-src.zip >& NUL

rem ##########################################################################
rem ZIP all Open Watcom source files
del /szx %ARCHIVES%\ow_src\
p4 -cOPENWATCOM_SRC sync %P4OPT% @%LABEL%
cd %ARCHIVES%\ow_src
zip -r ..\%PREFIX%-src.zip *

popd
goto end

:usage
echo Usage: zipup-rel [RELEASE]
echo.
echo Where 'RELEASE' is the public release number (ie: 1.5.0 etc).
:end
