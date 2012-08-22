@echo off
if not exist mktags.cmd goto wrongdir
if exist .\tags del .\tags
ctags -aeqdstf .\tags %OWROOT%\bld\owl\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\owl\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cc\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\cc\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\plusplus\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\plusplus\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\cg\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\intel\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\cg\intel\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\intel\386\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\cg\intel\386\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\intel\i86\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\cg\intel\i86\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\risc\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\risc\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\cg\risc\axp\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\risc\axp\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\cg\risc\ppc\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\risc\ppc\c\*.c
ctags -aeqdstf .\tags %OWROOT%\bld\cg\risc\sparc\h\*.h
ctags -aeqdstf .\tags %OWROOT%\bld\cg\risc\sparc\c\*.c
goto exend
:wrongdir
echo This must be run in BLD\CG
:exend
