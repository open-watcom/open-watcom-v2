@echo off
REM *****************************************************************
REM OWCONFIG.BAT - automatically generate batch file to set
REM environment variables
REM *****************************************************************

REM All of the work is done in owconfig.vbs
cscript -nologo owconfig.vbs %1 %2 %3 %4
