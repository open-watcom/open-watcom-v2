@echo off
echo Creating DOS/32 Advanced DOS Extender
echo *************************************
echo.

%dos32a%\src\sutils\build\build TEXT\oemtitle.asm 1958
tasm32 -dEXEC_TYPE=0 %TASMFLAGS% -c kernel.asm
tasm32 -dEXEC_TYPE=0 %TASMFLAGS% -c dos32a.asm
wcl %WCLFLAGS% -lr -fm=dos32a -fe=dos32a dos32a.obj kernel.obj
rem copy dos32a.exe %WATCOM%\BINW
copy dos32a.exe %DOS32A%\binw >nul
del *.obj
del *.exe