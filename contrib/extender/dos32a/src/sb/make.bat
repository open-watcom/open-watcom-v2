@echo off
echo Creating SUNSYS Bind Utility
echo ****************************
echo.

%dos32a%\src\sutils\build\build oemtitle.inf 62
tasm32 %TASMFLAGS% sbind.asm
wcl386 %WCLFLAGS% -l=dos32a -fe=sb -k65536 sbind.obj main.c
rem _sc -b -q sb
rem _ss sb sb -q
rem _ss sb -lock -q
rem copy sb.exe %WATCOM%\BINW\sb.exe
rem copy sb.exe %WATCOM%\BINW\_sb.exe
copy sb.exe  %DOS32A%\binw >nul
del *.obj
del *.exe