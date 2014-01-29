#pmake: os_dos cpu_i86 i86 intel

.SUFFIXES:
.SUFFIXES: .res .rc .lst .obj .c .asm .hlp .itl .ipf

CC         = wcc386 -d2 -I"$(os2api_dir)/os2386/h"

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
    wcc386 try.c -bw -d2 -I"$(os2api_dir)/os2386/h"
    %create try.lnk
    @%append try.lnk debug all
    @%append try.lnk system os2v2_pm
    @%append try.lnk op stack=20k
    @%append try.lnk file try.obj
    @%append try.lnk import WINQUEUEFROMID PMWIN.305
    @%append try.lnk import WINREPLYMSG PMWIN.307
    @%append try.lnk import WINTHREADASSOCQUEUE PMWIN.304
    @%append try.lnk import WINREPLYMSG PMWIN.307
    @%append try.lnk import WINQUERYSENDMSG PMWIN.306
    wlink name try @try.lnk

hello.exe: hello.obj hello.res
    %create hello.lnk
    @%append hello.lnk debug all
    @%append hello.lnk system os2v2_pm
    @%append hello.lnk file hello.obj
    @%append hello.lnk option map=hello.map, symfile
    @%append hello.lnk option modname=hello
    @%append hello.lnk option description 'OS/2 Presentation Manager Sample'
    @%append hello.lnk option manyautodata
    @%append hello.lnk option stack=16k
    @%append hello.lnk option heapsize=1024
    @%append hello.lnk option offset=0x10000
    @%append hello.lnk option protmode
    @%append hello.lnk import WINLOCKINPUT PMWIN.309
    wlink name hello @hello.lnk
    rc -p -x hello.res hello.exe
    copy/b hello.exe+hello.sym

dbg.exe: dbg.obj hello.res
    %create dbg.lnk
    @%append dbg.lnk debug all
    @%append dbg.lnk system os2v2_pm
    @%append dbg.lnk file dbg.obj
    @%append dbg.lnk option map=dbg.map, symfile
    @%append dbg.lnk option modname=dbg
    @%append dbg.lnk option description 'OS/2 Presentation Manager Sample Debugger'
    @%append dbg.lnk option stack=60k
#    @%append dbg.lnk option offset=0x20000
#    @%append dbg.lnk option protmode
    @%append dbg.lnk import WINQUEUEFROMID PMWIN.305
    @%append dbg.lnk import WINREPLYMSG PMWIN.307
    @%append dbg.lnk import WINTHREADASSOCQUEUE PMWIN.304
    @%append dbg.lnk import WINREPLYMSG PMWIN.307
    @%append dbg.lnk import WINQUERYSENDMSG PMWIN.306
    wlink name dbg @dbg.lnk
    rc -p -x hello.res dbg.exe
    copy/b dbg.exe+dbg.sym

clean: .SYMBOLIC
    @if exist *.obj @del *.obj
    @if exist *.res @del *.res
    @if exist *.exe @del *.exe
    @if exist *.lnk @del *.lnk
