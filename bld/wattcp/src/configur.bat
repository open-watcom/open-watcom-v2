@echo off
if %WATT_ROOT%. ==. goto not_set
if not exist %WATT_ROOT%\src\makefile.all goto not_set

:start

if %1.==watcom.   goto watcom
if %1.==all.      goto all
if %1.==clean.    goto clean
goto usage

::------------------------------------------------------------
:watcom
echo Generating Watcom makefiles, directories, errnos and dependencies
..\util\mkmake -o watcom_s.mak -d watcom\small  makefile.all WATCOM SMALL
..\util\mkmake -o watcom_l.mak -d watcom\large  makefile.all WATCOM LARGE
..\util\mkmake -o watcom_f.mak -d watcom\flat   makefile.all WATCOM FLAT
..\util\wc_err -s              > watcom\syserr.c
..\util\wc_err -e              > ..\inc\sys\watcom.err

echo Run wmake to make target(s):
echo  E.g. "wmake -ms -f watcom_l.mak" for large model
goto end

::------------------------------------------------------------
:usage
echo Configuring Waterloo tcp/ip targets.
echo Usage %0 {watcom, all, clean}
goto end

::------------------------------------------------------------
:clean
del watcom_*.mak

del watcom\syserr.c

del ..\inc\sys\watcom.err
goto end

::------------------------------------------------------------
:all
call %0 watcom   %2

:end
shift
echo.
if not %1. == . goto start
goto quit

:not_set
echo Environment variable WATT_ROOT not (or incorrectly) set.
echo Put this in your AUTOEXEC.BAT.
echo     e.g. "SET WATT_ROOT=C:\NET\WATT"
:quit
