@echo off
if not exist win*.exe goto Usage
which win.com > nul
if errorlevel 1 goto NoWINDOWS_COM
goto continue
:NoWINDOWS_COM
which win.bat > nul
if errorlevel 1 goto NoWINDOWS
:continue
if exist dowintst.bat erase dowintst.bat
cd > __PWD__
vi -d -q -s..\winlst.vi __PWD__
if exist c:\cmds\tmp.log erase c:\cmds\tmp.log
call dowintst.bat
if not errorlevel 1 goto SkipError
echo ***NOTE: WinRun encountered problem(s).
echo Displaying contents of WINOS2.LOG:
cat c:\cmds\winrun.log
:SkipError
erase win.lst
erase __PWD__
erase dowintst.bat
copy c:\cmds\winrun.log win.log >nul
copy c:\cmds\tmp.log . >nul
cat tmp.log
goto DONE_WINDOWS
:Usage
echo This file should be run from the directory that contains the test programs.
echo To invoke, type "..\windows.bat".
goto :DONE_WINDOWS
:NoWINDOWS
echo ***NOTE: Windows test programs not executed - MS-Windows not found.
:DONE_WINDOWS
