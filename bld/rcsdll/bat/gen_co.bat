@echo on
@echo **********************************************************************
@echo This is %WATCOM%\binw\gen_co.bat.
@echo YOU can edit this batchfile to provide checkout ( and lock ) 
@echo functionality to any generic revision control system.
@echo **********************************************************************
@echo off
@rem   From IDE: pause=%1, name=%2, project=%3, target=%4
@rem   Note: This is also called from the Editor,
@rem         in which case the only parameter is the name (%1)
if [%1]==[no_pause] goto done
pause
:done
