@echo off
echo Creating SUNSYS Compress Utility
echo ********************************
echo.

%dos32a%\src\sutils\build\build oemtitle.inf 66
tasm32 %TASMFLAGS% scomp.asm
tasm32 %TASMFLAGS% sload.asm
wcl386 %WCLFLAGS% -l=dos32a -fe=sc -k65536 scomp.obj sload.obj encode.c main.c

rem _sc -b -q sc
rem _ss sc sc -q
rem _ss sc -lock -q
rem copy sc.exe %WATCOM%\BINW\sc.exe
rem copy sc.exe %WATCOM%\BINW\_sc.exe
copy sc.exe %DOS32A%\binw >nul
del *.obj
del *.exe