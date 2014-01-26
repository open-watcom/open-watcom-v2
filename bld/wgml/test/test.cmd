@echo off
REM *****************************************************************
REM test.cmd - process the specified GML file with wgml4.0 and new WGML
REM
REM adapted from %owroot%\build.cmd for determining operating system
REM
REM combined OS/2, eCS and Windows version
REM
REM Needed arguments:
REM     1   Base directoryc for OW source
REM     2   name of gml file without extension (.gml always used)
REM
REM Example call             device PS is always used
REM         test h:\ow ttag
REM
REM will create logfile ttag.old and output file ttag.ops with wgml 4
REM         and logfile ttag.new and output file ttag.nps with WGML new
REM
REM *****************************************************************
setlocal
If [%1] == [] goto parmmiss
If [%2] == [] goto parmmiss
set owroot=%1
shift

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

set gmlinc=%OWDOCSDIR%\doc\devguide;%OWDOCSDIR%\doc\gml;%OWDOCSDIR%\doc\whelp;%OWDOCSDIR%\doc\hlp
set gmllib=%OWDOCSDIR%\gml\syslib;%OWDOCSDIR%\doc\whelp;%OWDOCSDIR%\doc\hlp
%OWDOCSDIR%\gml\%wg4_dir%\wgml  %1.gml     ( %options% out %1.ops %2 %3 %4 %5 %6 %7 %8 %9 >%1.old
%OWSRCDIR%\wgml\%wgml_dir%\wgml %1.gml     ( %options% out %1.nps %2 %3 %4 %5 %6 %7 %8 %9 >%1.ntr
%OWSRCDIR%\wgml\%wgml_dir%\wgml %1.gml  -r ( %options% out %1.nps %2 %3 %4 %5 %6 %7 %8 %9 >%1.new
goto eof

:PARMMISS
   echo Parameter missing
   echo usage: test owrootdir gmlfile

:EOF
   endlocal
