@echo off
@echo Creating DOS/32 Advanced DOS Extender Beta
@echo ******************************************
@echo.

build	TEXT\oemtitle.asm 1958
tasm	-dEXEC_TYPE=2 -c -la kernel.asm, EXE\kernel.obj, EXE\kernel.lst
tasm	-dEXEC_TYPE=2 -c -la dos32a.asm, EXE\dos32a.obj, EXE\dos32a.lst

cd EXE
wcl	-zq -lr -fm=dos32a -fe=dos32a dos32a.obj kernel.obj

rem	copy dos32a.exe %WATCOM%\BINW

del *.obj
cd ..
