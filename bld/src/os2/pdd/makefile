#===================================================================
#
#   Auto-dependency information
#
#===================================================================
OS22_H = $(%WATCOM)\h\os2
C32FLAGS  = -i=$(OS22_H) -bt=OS2 -zq
WC32    = wcc386 $(C32FLAGS)

OS21_H = $(%WATCOM)\h\os21x
C16FLAGS  = -i=$(OS21_H) -bt=os2 -ms -5 -omi -s -zdp -zff -zgf -zu -zl -zq
WC16    = wcc $(C16FLAGS)

ASM     = wasm $(AFLAGS)

.SUFFIXES:
.SUFFIXES: .obj .c .asm
.c.obj: .AUTODEPEND
        $(WC16) $*.c

.asm.obj: .AUTODEPEND
        $(ASM) $*.asm

all:    .SYMBOLIC hrtimer.sys hrtest.exe timer.exe
        @echo All done!
        @echo To install driver using CONFIG.SYS: DEVICE=[path]HRTIMER.SYS
        @echo To run the test program: HRTEST [milliseconds]
        @echo To run the timer program: TIMER program [program args]

HROBJS  =  devsegs.obj  strategy.obj stratini.obj header.obj
hrtimer.sys : $(HROBJS)
        @%create hrtimer.lnk
        @%append hrtimer.lnk name hrtimer.sys
        @%append hrtimer.lnk sys os2 dll initglobal
        @%append hrtimer.lnk option protmode
        @%append hrtimer.lnk option map
        @%append hrtimer.lnk option quiet
        @%append hrtimer.lnk option stack=0
        @%append hrtimer.lnk lib os2
        @for %f in ($(HROBJS)) do @%append hrtimer.lnk file %f
        @%append hrtimer.lnk segment type DATA SHARED PRELOAD
        @%append hrtimer.lnk segment '_TEXT' PRELOAD IOPL
        @%append hrtimer.lnk segment '_INITCODE' PRELOAD IOPL
        wlink  @hrtimer.lnk

hrtest.exe: hrtest.obj
        wlink file hrtest system os2v2 option quiet

hrtest.obj: hrtest.c
        $(WC32) $*.c

timer.exe: timer.obj
        wlink file timer system os2v2 option quiet

timer.obj: timer.c
        $(WC32) $*.c
