@echo off

rem
rem This batch file builds the latest version of Open Watcom.
rem

c:
cd %OWROOT%\bld

rem Build builder
rem -------------
cd %OWROOT%\bld\builder\nt386
wmake

rem Build the Waterloo TCP library
rem ------------------------------
cd %OWROOT%\contrib\wattcp\src
wmake -ms

rem Main build
rem ----------
cd %OWROOT%\bld
builder -i rel2

cd %OWROOT%\bld
echo Done!
echo Check %OWROOT%\bld\build.log for unexpected errors.
