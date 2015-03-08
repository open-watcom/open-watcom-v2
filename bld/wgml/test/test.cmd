@echo off
REM *****************************************************************
REM test.cmd - process the specified GML file with wgml4.0 and new WGML
REM
REM adapted from %OWROOT%\build.cmd for determining operating system
REM
REM combined OS/2, eCS and Windows version
REM
REM Needed arguments:
REM     1   name of gml file without extension (.gml always used)
REM
REM Example call             device PS is always used
REM         test ttag
REM
REM will create logfile ttag.old and output file ttag.ops with wgml 4
REM         and logfile ttag.new and output file ttag.nps with WGML new
REM
REM *****************************************************************
setlocal
If [%1] == [] goto parmmiss

rem NT/XP? or OS/2 or eCS?
   if [%OS2_SHELL%] == [] goto noOS2
   if [%OS%] == [] goto noWIN
   if [%OS%] == [ecs] goto noWIN
:UNKNOWN
   echo Operating System not recognized, sorry
   goto eof

:NOOS2
   if [%Systemroot%] == [] goto unknown
   set WGML_DIR=nt386
   set WG4_DIR=dos
   goto bld1

:NOWIN
:CONT3
   set WGML_DIR=os2386
   set WG4_DIR=os2

:BLD1

echo ( set date 'April 28, 2009' >wgml.opt
echo ( set time '21:11:02' >>wgml.opt
echo ( wscript >>wgml.opt
echo ( dev ps >>wgml.opt
echo ( pass 1 >>wgml.opt
echo ( verbose >>wgml.opt
echo ( warning >>wgml.opt
echo ( statistics >>wgml.opt
echo ( inclist >>wgml.opt
echo ( index >>wgml.opt

set GMLLIB=%OWDOCSDIR%\gml\syslib;%OWDOCSDIR%\doc\whelp;%OWDOCSDIR%\doc\hlp
set GMLINC=%OWDOCSDIR%\doc\devguide;%OWDOCSDIR%\doc\gml;%OWDOCSDIR%\doc\whelp;%OWDOCSDIR%\doc\hlp

if [%OWDOSBOX%] == [] goto NATIVE1
REM create DOSBOX batch script for run wgml on 64-bit Windows under DOSBOX
    echo d: > wgml.bat
    echo set GMLINC=c:\doc\devguide;c:\doc\gml;c:\doc\whelp;c:\doc\hlp >> wgml.bat
    echo set GMLLIB=c:\gml\syslib;c:\doc\whelp;c:\doc\hlp;.\testlib >> wgml.bat
    echo c:\gml\dos\wgml.exe %1.gml ( file wgml.opt out %1.ops %2 %3 %4 %5 %6 %7 %8 %9 ^>%1.old >> wgml.bat
    echo exit >> wgml.bat
    %OWDOSBOX% -noautoexec -c "mount c %OWDOCSDIR%" -c "mount d ." -c "mount e %OWSRCDIR%" -c "d:wgml.bat" -noconsole
    goto PAUSE1
:NATIVE1   
    %OWDOCSDIR%\gml\%WG4_DIR%\wgml %1.gml ( file wgml.opt out %1.ops %2 %3 %4 %5 %6 %7 %8 %9 >%1.old
:PAUSE1
    pause
    %OWSRCDIR%\wgml\%WGML_DIR%\wgml %1.gml    ( file wgml.opt out %1.nps %2 %3 %4 %5 %6 %7 %8 %9 >%1.ntr
    %OWSRCDIR%\wgml\%WGML_DIR%\wgml %1.gml -r ( file wgml.opt out %1.nps %2 %3 %4 %5 %6 %7 %8 %9 >%1.new
    pause
    wdw %OWSRCDIR%\wgml\%WGML_DIR%\wgml %1.gml -r ( file wgml.opt out %1.nps %2 %3 %4 %5 %6 %7 %8 %9
    goto eof

:PARMMISS
    echo Parameter missing
    echo usage: test gmlfile

:EOF
    endlocal
