@echo off
echo Creating SUNSYS Debugger Library
echo ********************************

tasm	sdebug.asm
wlib	-b -c sdebug.lib +-sdebug.obj

rem	copy sdebug.lib %WATCOM%\LIB386\L32

del *.obj