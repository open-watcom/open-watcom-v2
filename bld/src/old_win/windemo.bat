@echo off
cd life
wmake
if errorlevel 1 goto errorexit
..\iswin
if errorlevel 1 goto win
win life
goto exit
:win
echo Return to windows with ALT-ESCAPE.
echo Select "File" from the program manager, and select
echo the RUN option to run life
goto exit
:errorexit
echo make did not work!
:exit
