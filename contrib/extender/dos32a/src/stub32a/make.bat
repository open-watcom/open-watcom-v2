@echo off
echo Creating DOS/32 STub
echo *************************************
echo.

%dos32a%\src\sutils\build\build oemtitle.inf 15
tasm32 -dEXEC_TYPE=0 %TASMFLAGS% stub32a.asm
tasm32 -dEXEC_TYPE=0 %TASMFLAGS% stub32c.asm
wcl %WCLFLAGS% -lr -fe=stub32a stub32a.obj
wcl %WCLFLAGS% -lr -fe=stub32c stub32c.obj
rem copy stub32a.exe %WATCOM%\BINW
rem copy stub32c.exe %WATCOM%\BINW
copy stub32a.exe %DOS32A%\binw >nul
copy stub32c.exe %DOS32A%\binw >nul
del *.obj
del *.exe
