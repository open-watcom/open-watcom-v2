@echo off
echo Creating DOS/32 PCTEST
echo *************************************
echo.
tasm32 %TASMFLAGS% pctest.asm
wcl386 %WCLFLAGS% -fe=PCTEST.EXE -k65535 -l=dos32a MAIN.C PCTEST.OBJ
copy pctest.exe %DOS32A%\pctest >nul
del *.obj
del *.exe