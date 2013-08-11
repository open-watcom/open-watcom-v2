@echo on
@echo **********************************************************************
:segment win | dos
@echo This is %WATCOM%\binw\gen_ci.bat.
:elsesegment os2
@echo This is %WATCOM%\binp\gen_ci.bat.
:elsesegment nt & x64
@echo This is %WATCOM%\binnt64\gen_ci.bat.
:elsesegment nt & 386
@echo This is %WATCOM%\binnt\gen_ci.bat.
:elsesegment nt & axp
@echo This is %WATCOM%\axpnt\gen_ci.bat.
:endsegment
@echo You can edit this batchfile to provide checkin ( and unlock ) 
@echo functionality to any generic revision control system.
@echo **********************************************************************
@rem   From IDE: Name=%1, File Containing Message=%2, Project=%3, Target=%4
