@echo off
REM *****************************************************************
REM build.cmd - build Open Watcom using selected compiler
REM
REM will build the builder and watcom
REM
REM combined OS/2, eCS and Windows version
REM
REM If first argument is "self", uses tools in rel2 to build,
REM requiring customized devvars.cmd. Otherwise, customized
REM myvars.cmd is needed. If the appropriate file does not exist,
REM owconfig.bat will be invoked to automatically generate it.
REM If running on OS/2 or eCS, it has to be created manually.
REM
REM Call without parms for "builder rel2" operation -> build
REM Call with clean for "builder clean"  operation  -> build clean
REM --> requires a customized setvars.bat/cmd named myvars.cmd
REM --> set WATCOM to the existing OW 1.x installation
REM *****************************************************************
setlocal
if [%1] == [self] goto self
   set myow=myvars
   goto doneself
:SELF
   shift
   set myow=devvars
:DONESELF

   set target=%1
   if [%target%] == [] set target=rel2
rem the makefiles dont know a target rel2, so only pass target if clean
   set makeclean=
   if [%target%] == [clean] set makeclean=clean

rem NT/XP? or OS/2 or eCS?
   if [%OS2_SHELL%] == [] goto noOS2
   if [%OS%] == [] goto noWIN
   if [%OS%] == [ecs] goto noWIN
:UNKNOWN
   echo Operating System not recognized, sorry
   goto eof

:NOOS2
   if exist %myow%.cmd goto cont2
      call owconfig.bat %myow%.cmd

:CONT2
   if [%Systemroot%] == [] goto unknown
   set builderdir=nt386
   goto bld1

:NOXP
   set builderdir=nt386
   goto bld1

:NOWIN
   if exist %myow%.cmd goto cont3
       echo Customized %myow%.cmd not found, cannot continue
       echo must be in the same dir as build.cmd
       echo copy setvars.cmd to %myow%.cmd and customize
       goto eof

:CONT3
   set builderdir=os2386

:BLD1
   call %myow%.cmd
rem start with the builder
   cd %devdir%
   cd builder\%builderdir%
   wmake %makeclean%

REM now we can start the builder with 9 generations of logfiles
   cd %devdir%
   builder %target% -b 9

:EOF
   endlocal
