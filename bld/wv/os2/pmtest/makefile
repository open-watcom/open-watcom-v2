#pmake: os_dos cpu_i86 i86 intel

.SUFFIXES:
.SUFFIXES: .res .rc .lst .obj .c .asm .hlp .itl .ipf

CC         = wcc386 -d2 -i"$(lang_root)/h/os2"

AFLAGS  = -Mx -t -z
ASM     = ml -c -Zm
LFLAGS  =
LINK    = WLINK  $(LFLAGS)

.obj.lst:
    wdis -l-s $*

.c.obj:
    $(CC) $*.c

.asm.obj:
    $(ASM)   $*.asm

.ipf.hlp:
        ipfc $*.ipf -W3

.itl.hlp:
        cc  -P $*.itl
        ipfc $*.i
        del $*.i

.rc.res:
        rc -r $*.rc

HEADERS = hello.h

#-------------------------------------------------------------------
#   A list of all of the object files
#-------------------------------------------------------------------


all: dbg.exe hello.exe try.exe .symbolic
    @%null

hello.res: hello.rc hello.ico hello.h

hello.obj: hello.c $(HEADERS)
dbg.obj: dbg.c $(HEADERS)

try.exe : try.c
    wcc386 try -bw -d2 -i"$(lang_root)/h/os2"
    wlink sys os2v2_pm @try

hello.exe: hello.obj  hello.lnk hello.res
    wlink @hello.lnk op symfile
    rc -p -x hello.res hello.exe
    copy/b hello.exe+hello.sym

dbg.exe: dbg.obj dbg.lnk hello.res
    wlink @dbg.lnk op symfile
    rc -p -x hello.res dbg.exe
    copy/b dbg.exe+dbg.sym

clean: .SYMBOLIC
    @if exist *.obj @del *.obj
    @if exist *.res @del *.res
    @if exist *.exe @del *.exe
