@echo off

rem Path configuration
rem ==================
set WWWPATH=\www
set OWROOT=c:\ow\ow
set OWRELROOT=c:\ow\ow\pass
set ARCH7Z="c:\Program Files\7-Zip\7z"

if exist %OWRELROOT% goto prerequisite_ok
echo Missing %OWRELROOT%. Can't continue with rotation.
goto done

:prerequisite_ok

rem Build Archives
rem ==============
if exist %WWWPATH%\snaparch\ss.7z del %WWWPATH%\snaparch\ss.7z
%ARCH7Z% a -t7z -r %WWWPATH%\snaparch\ss.7z %OWRELROOT%\*

rem Move build to snapshot directory
rem ================================
if exist %WWWPATH%\snapshot move %WWWPATH%\snapshot %WWWPATH%\snapshot.bak
if exist %WWWPATH%\snapshot goto done
move %OWRELROOT% %WWWPATH%\snapshot

rem Move Archives
rem =============
if exist %WWWPATH%\snaparch\ow-snapshot.7z del %WWWPATH%\snaparch\ow-snapshot.7z
move %WWWPATH%\snaparch\ss.7z %WWWPATH%\snaparch\ow-snapshot.7z

rem Move installers
rem =============
move %OWROOT%\distrib\ow\bin\*.* %WWWPATH%\install\

rem Final Cleanup
rem =============
rmdir /S /Q %WWWPATH%\snapshot.bak

:done
