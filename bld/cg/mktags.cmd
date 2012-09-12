@echo off
if not exist mktags.cmd goto wrongdir
if exist .\tags del .\tags
ctags -aeqdstf .\tags %OWSRCDIR%\owl\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\owl\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cc\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\cc\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\plusplus\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\plusplus\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\cg\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\intel\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\cg\intel\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\intel\386\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\cg\intel\386\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\intel\i86\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\cg\intel\i86\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\risc\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\risc\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\cg\risc\axp\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\risc\axp\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\cg\risc\ppc\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\risc\ppc\c\*.c
ctags -aeqdstf .\tags %OWSRCDIR%\cg\risc\sparc\h\*.h
ctags -aeqdstf .\tags %OWSRCDIR%\cg\risc\sparc\c\*.c
goto exend
:wrongdir
echo This must be run in BLD\CG
:exend
