@echo off
echo Creating SUNSYS Debugger
echo ************************
echo.

%dos32a%\src\sutils\build\build oemtitle.inf 50
tasm32 -dSVR=0 %TASMFLAGS% sd.asm
wcl386 %WCLFLAGS% -l=dos32a -fe=sd sd.obj
rem _sc -b -q sd
rem _ss sd sd -q
rem _ss sd -lock -q
rem copy sd.exe %WATCOM%\BINW\sd.exe
rem copy sd.exe %WATCOM%\BINW\_sd.exe
copy sd.exe %DOS32A%\binw >nul
del *.obj
del *.exe