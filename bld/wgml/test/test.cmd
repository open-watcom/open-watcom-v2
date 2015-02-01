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
   set wgml_dir=nt386
   set wg4_dir=dos
   goto bld1

:NOWIN
:CONT3
   set wgml_dir=os2386
   set wg4_dir=os2

:BLD1

set options=set date 'April 28, 2009' set time '21:11:02' wscript dev ps pass 1 verbose warn stat incl index
rem set options=wscript dev ps pass 1 verbose warn stat incl index

set GMLLIB=%OWDOCSDIR%\gml\syslib;%OWDOCSDIR%\doc\whelp;%OWDOCSDIR%\doc\hlp
set GMLINC=%OWDOCSDIR%\doc\devguide;%OWDOCSDIR%\doc\gml;%OWDOCSDIR%\doc\whelp;%OWDOCSDIR%\doc\hlp

If [%OWDOSBOX%] == [] %OWDOCSDIR%\gml\%wg4_dir%\wgml %1.gml ( %options% out %1.ops %2 %3 %4 %5 %6 %7 %8 %9 >%1.old
If [%OWDOSBOX%] == [] goto pause1
REM create DOSBOX batch script for run wgml on 64-bit Windows under DOSBOX
   echo d: > wgml.bat
   echo set GMLINC=c:\doc\devguide;c:\doc\gml;c:\doc\whelp;c:\doc\hlp >> wgml.bat
   echo set GMLLIB=c:\gml\syslib;c:\doc\whelp;c:\doc\hlp;.\testlib >> wgml.bat
   echo c:\gml\dos\wgml.exe %1.gml ( %options% out %1.ops %2 %3 %4 %5 %6 %7 %8 %9 ^>%1.old >> wgml.bat
   echo exit >> wgml.bat
   %OWDOSBOX% -noautoexec -c "mount c %OWDOCSDIR%" -c "mount d ." -c "mount e %OWSRCDIR%" -c "d:wgml.bat" -noconsole
:PAUSE1
   pause
   %OWSRCDIR%\wgml\%wgml_dir%\wgml %1.gml    ( %options% out %1.nps %2 %3 %4 %5 %6 %7 %8 %9 >%1.ntr
   %OWSRCDIR%\wgml\%wgml_dir%\wgml %1.gml -r ( %options% out %1.nps %2 %3 %4 %5 %6 %7 %8 %9 >%1.new
   pause
   wdw %OWSRCDIR%\wgml\%wgml_dir%\wgml %1.gml -r ( %options% out %1.nps %2 %3 %4 %5 %6 %7 %8 %9
   goto eof

:PARMMISS
   echo Parameter missing
   echo usage: test gmlfile

:EOF
   endlocal
