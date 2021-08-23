@echo off
if %WATT_ROOT%. ==. goto not_set
if not exist %WATT_ROOT%\src\makefile.all goto not_set

:start

if %1.==borland.  goto borland
if %1.==turboc.   goto turboc
if %1.==watcom.   goto watcom
if %1.==metaware. goto metaware
if %1.==djgpp.    goto djgpp
if %1.==msoft.    goto msoft
if %1.==all.      goto all
if %1.==clean.    goto clean
goto usage

::------------------------------------------------------------
:borland
echo Generating Borland makefiles, directories, errnos and dependencies
..\util\mkmake -o bcc_s.mak -d borland\small  makefile.all BORLAND SMALL
..\util\mkmake -o bcc_l.mak -d borland\large  makefile.all BORLAND LARGE
..\util\mkmake -o bcc_f.mak -d borland\flat   makefile.all BORLAND FLAT
..\util\mkdep -p$(OBJDIR)\ *.[ch] > borland\depend.wat
..\util\bcc_err -s                > borland\syserr.c
..\util\bcc_err -e                > ..\inc\sys\borland.err

echo Run make to make target(s):
echo  E.g. "make -f bcc_l.mak" for large model
goto end

::------------------------------------------------------------
:turboc
echo Generating Turbo-C makefiles, directories, errnos and dependencies
..\util\mkmake -o tcc_s.mak -d turboc\small  makefile.all TURBOC SMALL
..\util\mkmake -o tcc_l.mak -d turboc\large  makefile.all TURBOC LARGE
..\util\mkdep -p$(OBJDIR)\ *.[ch] > turboc\depend.wat
..\util\tcc_err -s                > turboc\syserr.c
..\util\tcc_err -e                > ..\inc\sys\turboc.err

echo Run make to make target(s):
echo  E.g. "make -f tcc_l.mak" for large model
goto end

::------------------------------------------------------------
:watcom
echo Generating Watcom makefiles, directories, errnos and dependencies
..\util\mkmake -o watcom_s.mak -d watcom\small  makefile.all WATCOM SMALL
..\util\mkmake -o watcom_l.mak -d watcom\large  makefile.all WATCOM LARGE
..\util\mkmake -o watcom_f.mak -d watcom\flat   makefile.all WATCOM FLAT
..\util\mkdep -p$(OBJDIR)\ *.c > watcom\depend.wat
..\util\wc_err -s              > watcom\syserr.c
..\util\wc_err -e              > ..\inc\sys\watcom.err

echo Run wmake to make target(s):
echo  E.g. "wmake -ms -f watcom_l.mak" for large model
goto end

::------------------------------------------------------------
:metaware
echo Generating Metaware makefile, directory, errnos and dependencies
..\util\mkmake -o metaware.mak -d metaware makefile.all METAWARE
..\util\mkdep -p$(OBJDIR)\ *.[ch] > metaware\depend.wat
..\util\hc_err -s                 > metaware\syserr.c
..\util\hc_err -e                 > ..\inc\sys\metaware.err

echo Run a Borland compatible make to make target:
echo   E.g. "maker -f metaware.mak"
goto end

::------------------------------------------------------------
:msoft
echo Generating Microsoft makefiles, directories, errnos and dependencies
..\util\mkmake -o msoft_s.mak -d msoft\small  makefile.all MSOFT SMALL
..\util\mkmake -o msoft_l.mak -d msoft\large  makefile.all MSOFT LARGE
..\util\mkmake -o msoft_f.mak -d msoft\flat   makefile.all MSOFT FLAT
..\util\mkdep -p$(OBJDIR)\ *.[ch] > msoft\depend.wat
..\util\ms_err -s                 > msoft\syserr.c
..\util\ms_err -e                 > ..\inc\sys\msoft.err
rem ..\util\ms32_err -s           > msoft\syserr32.c
rem ..\util\ms32_err -e           > ..\inc\sys\msoft32.err

echo Run nmake to make target(s):
echo  E.g. "nmake -f msoft_l.mak" for large model
goto end

::------------------------------------------------------------
:djgpp
echo Generating DJGPP makefile, directory, errnos and dependencies
..\util\mkmake -o djgpp.mak -d djgpp makefile.all DJGPP
..\util\mkdep -p$(OBJDIR)/ *.[ch] > djgpp\depend.wat
..\util\dj_err -s                 > djgpp\syserr.c
..\util\dj_err -e                 > ..\inc\sys\djgpp.err

echo Run GNU make to make target:
echo   make -f djgpp.mak
goto end

::------------------------------------------------------------
:usage
echo Configuring Waterloo tcp/ip targets.
echo Usage %0 {borland, turboc, watcom, msoft, metaware, djgpp, all, clean}
goto end

::------------------------------------------------------------
:clean
del djgpp.mak
del msoft_*.mak
del watcom_*.mak
del bcc_*.mak
del tcc_*.mak
del metaware.mak

del djgpp\depend.wat
del msoft\depend.wat
del watcom\depend.wat
del borland\depend.wat
del turboc\depend.wat
del metaware\depend.wat

del djgpp\syserr.c
del msoft\syserr.c
del watcom\syserr.c
del turboc\syserr.c
del borland\syserr.c
del metaware\syserr.c

del ..\inc\sys\djgpp.err
del ..\inc\sys\msoft.err
del ..\inc\sys\watcom.err
del ..\inc\sys\turboc.err
del ..\inc\sys\borland.err
del ..\inc\sys\metaware.err
goto end

::------------------------------------------------------------
:all
call %0 borland  %2
call %0 turboc   %2
call %0 watcom   %2
call %0 metaware %2
call %0 msoft    %2
call %0 djgpp    %2

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
