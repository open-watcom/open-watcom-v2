@echo off
if not exist mktags.cmd goto wrongdir
if exist .\tags del .\tags
ctags -aeqdstf .\tags %DEVDIR%\owl\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\owl\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cc\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\cc\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\plusplus\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\plusplus\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\cg\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\intel\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\cg\intel\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\intel\386\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\cg\intel\386\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\intel\i86\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\cg\intel\i86\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\risc\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\risc\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\cg\risc\axp\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\risc\axp\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\cg\risc\ppc\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\risc\ppc\c\*.c
ctags -aeqdstf .\tags %DEVDIR%\cg\risc\sparc\h\*.h
ctags -aeqdstf .\tags %DEVDIR%\cg\risc\sparc\c\*.c
goto exend
:wrongdir
echo This must be run in BLD\CG
:exend
