@echo off
echo Creating DOS/32 Build
echo *************************************
echo.

wcl %WCLFLAGS% build.c
del *.obj
