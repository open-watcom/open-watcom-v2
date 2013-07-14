rem rotate.cmd for OS/2  eCS
rem @echo off

rem Path configuration
rem ==================
set WWWPATH=\www
set OWROOT=c:\ow\ow
set OWRELROOT=c:\ow\ow\pass
rem 7z is standard with eCS 2.1
set ARCH7Z=7z

if exist %OWRELROOT% goto prerequisite_ok
echo Missing %OWRELROOT%. Can't continue with rotation.
goto done

:prerequisite_ok

rem Build Archives
rem ==============
if exist %WWWPATH%\snaparch\ss.zip %OWROOT%\build\bin\rm -f %WWWPATH%\snaparch\ss.zip
if exist %WWWPATH%\snaparch\ss.7z %OWROOT%\build\bin\rm -f %WWWPATH%\snaparch\ss.7z
"%ARCH7Z%" a -tzip -r %WWWPATH%\snaparch\ss.zip %OWRELROOT%\*
"%ARCH7Z%" a -t7z -r %WWWPATH%\snaparch\ss.7z %OWRELROOT%\*

rem Move build to snapshot directory
rem ================================
if exist %WWWPATH%\snapshot move %WWWPATH%\snapshot %WWWPATH%\snapshot.bak
if exist %WWWPATH%\snapshot goto done
move %OWRELROOT% %WWWPATH%\snapshot

rem Move Archives
rem =============
if exist %WWWPATH%\snaparch\ow-snapshot.zip %OWROOT%\build\bin\rm -f %WWWPATH%\snaparch\ow-snapshot.zip
if exist %WWWPATH%\snaparch\ow-snapshot.7z %OWROOT%\build\bin\rm -f %WWWPATH%\snaparch\ow-snapshot.7z
move %WWWPATH%\snaparch\ss.zip %WWWPATH%\snaparch\ow-snapshot.zip
move %WWWPATH%\snaparch\ss.7z %WWWPATH%\snaparch\ow-snapshot.7z

rem Move installers
rem =============
move %OWROOT%\distrib\ow\open-watcom-*.* %WWWPATH%\install

rem Final Cleanup
rem =============
%OWROOT%\build\bin\rm -rf %WWWPATH%\snapshot.bak

:done
