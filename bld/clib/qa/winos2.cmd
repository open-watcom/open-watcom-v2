@echo off
if not exist win*.exe goto Usage
dir /f win*.exe > d:\dev\clib\qa\winos2.lst
if exist c:\cmds\tmp.log erase c:\cmds\tmp.log
c:\cmds\winrun.exe -q -x @d:\dev\clib\qa\winos2.lst
if not errorlevel 1 goto SkipError
echo ***NOTE: WinRun encountered problem(s).
echo Displaying contents of WINOS2.LOG:
cat c:\cmds\winrun.log
:SkipError
erase d:\dev\clib\qa\winos2.lst
copy c:\cmds\winrun.log winos2.log >nul
copy c:\cmds\tmp.log . >nul
cat tmp.log
goto End
:Usage
echo This file should be run from the directory that contains the test programs.
echo To invoke, type "..\winos2.cmd".
:End
