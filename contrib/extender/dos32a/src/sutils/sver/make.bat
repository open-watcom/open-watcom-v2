@echo off
echo Creating DOS/32 SVER
echo *************************************
echo.

wcl %WCLFLAGS% /lr /fe=SVER.EXE MAIN.C
copy sver.exe %DOS32A%\binw >nul
del *.obj
del *.exe



