ALL	: cwd.exe cwd.ovl


cwd.exe	: cwd.asm macros.asm equates.asm
 ml cwd.asm
 del cwd.obj

cwd.ovl	: cwd-ovl.asm disas.asm generr.asm fpu.asm disas.inc ..\cw.inc ..\cw32.exe ..\strucs.inc win.asm evaluate.asm
 ml /c /DENGLISH=1 /Flcwd-ovl.lst /Fmcwd-ovl.map cwd-ovl.asm >err.log
 cwl /map /sym cwd-ovl
 del cwd.ovl
 ren cwd-ovl.exe cwd.ovl
 del cwd-ovl.exe
