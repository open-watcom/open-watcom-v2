@echo off

rem Path configuration
rem ==================
set OWWWWPATH=\www
set OWROOT=c:\ow\ow
set OWRELROOT=c:\ow\ow\pass
set OWARCH7Z="c:\Program Files\7-Zip\7z"

set NUL=NUL
if '%OS%' == 'Windows_NT' set NUL=

if exist %OWRELROOT% goto prerequisite_ok
echo Missing %OWRELROOT%. Can't continue with rotation.
goto done

:prerequisite_ok

if not exist %OWWWWPATH%\snaparch\%NUL% mkdir %OWWWWPATH%\snaparch
if not exist %OWWWWPATH%\install\%NUL% mkdir %OWWWWPATH%\install

rem Build Archives
rem ==============
if exist %OWWWWPATH%\snaparch\ss.7z del %OWWWWPATH%\snaparch\ss.7z
%OWARCH7Z% a -t7z -r %OWWWWPATH%\snaparch\ss.7z %OWRELROOT%\*

rem Move build to snapshot directory
rem ================================
if exist %OWWWWPATH%\snapshot move %OWWWWPATH%\snapshot %OWWWWPATH%\snapshot.bak
if exist %OWWWWPATH%\snapshot goto done
move %OWRELROOT% %OWWWWPATH%\snapshot

rem Move Archives
rem =============
if exist %OWWWWPATH%\snaparch\ow-snapshot.7z del %OWWWWPATH%\snaparch\ow-snapshot.7z
move %OWWWWPATH%\snaparch\ss.7z %OWWWWPATH%\snaparch\ow-snapshot.7z

rem Move installers
rem =============
move %OWROOT%\distrib\ow\bin\*.* %OWWWWPATH%\install\

rem Final Cleanup
rem =============
rmdir /S /Q %OWWWWPATH%\snapshot.bak

:done
set NUL=
