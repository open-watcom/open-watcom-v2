#pmake: os_dos cpu_i86 i86 intel
#===================================================================
#
#   Make file for HELLO sample program provided by IBM Corporation.
#   Adapted by Watcom Systemms, Inc.
#
#===================================================================

#===================================================================
#   Sample application makefile, common definitions for the WATCOM C
#   compiler environment
#===================================================================

.SUFFIXES:
.SUFFIXES: .res .rc .lst .obj .c .asm .hlp .itl .ipf

#===================================================================
# Default compilation macros for sample programs
#
# Compile switches  that are enabled
#

CC         = wcc386 -d2 -D_Optlink= /i\dev\watcom\os220_h

AFLAGS  = /Mx -t -z
ASM     = ml /c /Zm
LFLAGS  =
LINK    = WLINK  $(LFLAGS)

.obj.lst:
    wdisasm $*/l/s

.c.obj:
    $(CC) $*.c

.asm.obj:
    $(ASM)   $*.asm

.ipf.hlp:
        ipfc $*.ipf /W3

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
    wcc386 try /bw /d2 /i$(watcom_dir)\OS220_H
    wlink sys os2v2_pm @try

hello.exe: hello.obj  hello.lnk hello.res
    wlink @hello.lnk op symfile
    rc -p -x hello.res hello.exe
    copy/b hello.exe+hello.sym

dbg.exe: dbg.obj dbg.lnk hello.res
    wlink @dbg.lnk op symfile
    rc -p -x hello.res dbg.exe
    copy/b dbg.exe+dbg.sym
