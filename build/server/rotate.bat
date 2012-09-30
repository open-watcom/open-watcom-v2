@echo off

rem Path configuration
rem ==================
set WWWPATH=\www
set ARCH7Z=c:\Program Files\7-Zip\7z

rem Initialization
rem ==============

if exist %OWRELROOT% goto prerequisite_ok
echo Missing %OWRELROOT%. Can't continue with rotation.
goto done

:prerequisite_ok

rem Build Archives
rem ==============
if exist %WWWPATH%\snapshots\ss.zip %OWBINDIR%\rm -f %WWWPATH%\snapshots\ss.zip
if exist %WWWPATH%\snapshots\ss.7z %OWBINDIR%\rm -f %WWWPATH%\snapshots\ss.7z
"%ARCH7Z%" a -tzip -r %WWWPATH%\snapshots\ss.zip %OWRELROOT%\*
"%ARCH7Z%" a -t7z -r %WWWPATH%\snapshots\ss.7z %OWRELROOT%\*

rem Move pass1 build
rem =================
if exist %WWWPATH%\snapshot move %WWWPATH%\snapshot %WWWPATH%\snapshot.bak
if exist %WWWPATH%\snapshot goto done
move %OWRELROOT% %WWWPATH%\snapshot

rem Move Archives
rem =============
if exist %WWWPATH%\snapshots\ow-snapshot.zip %OWBINDIR%\rm -f %WWWPATH%\snapshots\ow-snapshot.zip
if exist %WWWPATH%\snapshots\ow-snapshot.7z %OWBINDIR%\rm -f %WWWPATH%\snapshots\ow-snapshot.7z
move %WWWPATH%\snapshots\ss.zip %WWWPATH%\snapshots\ow-snapshot.zip
move %WWWPATH%\snapshots\ss.7z %WWWPATH%\snapshots\ow-snapshot.7z

rem Move installers
rem =============
move %OWROOT%\distrib\ow\open-watcom-*.* %WWWPATH%\installers\

rem Final Cleanup
rem =============
%OWBINDIR%\rm -rf %WWWPATH%\snapshot.bak

:done
