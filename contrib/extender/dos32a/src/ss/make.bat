@echo off
echo Creating SUNSYS Setup Utility
echo *****************************
echo.

%dos32a%\src\sutils\build\build oemtitle.inf 63
tasm32 %TASMFLAGS% setup.asm
wcl386 %WCLFLAGS% -l=dos32a -fe=ss -k65536 setup.obj main.c
rem _sc -b -q ss
rem _ss ss ss -q
rem _ss ss -lock -q
rem copy ss.exe %WATCOM%\BINW\ss.exe
rem copy ss.exe %WATCOM%\BINW\_ss.exe
copy ss.exe %DOS32A%\binw >nul
del *.obj
del *.exe