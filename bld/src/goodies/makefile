#
#  Makefile for Q&A Booklet Demo Programs
#

#which memory model and processor to use by default
#PROCESSOR =     # for the 16-bit compilers
PROCESSOR = 386 # for the 32-bit compilers

# name the compiler
CC = wcc$(PROCESSOR)
FC = wfc$(PROCESSOR)
ASM = wasm

!ifeq PROCESSOR 386
MODEL = f       # flat
!else
MODEL = l       # large
!endif

# options to use
CFLAGS = -m$(MODEL) -zq
FFLAGS = -m$(MODEL) -noterm -quiet
AFLAGS =
LNKOPT = option quiet

.EXTENSIONS:
.EXTENSIONS: .exe .rex .lib .obj .wbj .asm .c .for

.BEFORE
        @set INCLUDE=.;$(%watcom)\h;$(%watcom)\h\win
        @set FINCLUDE=$(%watcom)\src\fortran;$(%watcom)\src\fortran\dos
        @set DOS4G=QUIET

mix_exes = &
    mix1.exe &
    mix2.exe &
    mix3.exe &
    mix4.exe &
    mix5.exe &
    mix6.exe

!ifeq PROCESSOR 386
exes = &
    bimodal.exe &
    fileio.exe &
    fhandles.exe &
    fmemory.exe &
    fscreen.exe &
    handles.exe &
    keystat.exe &
    keystapl.exp &
    memory.exe &
    mempls40.exp &
    memwin.exe &
    mouse.exe &
    screen.exe &
    screenpl.exp &
    simulate.exe &
    spwnrd4g.exe &
    spwndd4g.exe &
    spwnrpls.exp &
    spwndpls.exp
!else
exes = &
    fhandles.exe &
    handles.exe
!endif


# explicit rules

all :   most mix .SYMBOLIC
        @%null

most :  $(exes) .SYMBOLIC
        @%null

mix :   $(mix_exes) .SYMBOLIC
        @%null

bimodal.exe : bimodal.obj bimo.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g name bimodal file bimodal,bimo

fileio.exe : fileio.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g name fileio file fileio

fhandles.exe : fhandles.obj .PRECIOUS
        wlink $(LNKOPT) file fhandles name fhandles

fmemory.exe : fmemory.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file fmemory name fmemory

fscreen.exe : fscreen.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file fscreen name fscreen

handles.exe : handles.obj .PRECIOUS
        wlink $(LNKOPT) file handles name handles

keystat.exe : keystat.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file keystat name keystat

keystapl.exp : keystapl.obj .PRECIOUS
        wlink $(LNKOPT) system pharlap file keystapl name keystapl

memory.exe : memory.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file memory name memory

mempls40.exp : mempls40.obj .PRECIOUS
        wlink $(LNKOPT) system pharlap file mempls40 name mempls40

memwin.exe : memwin.rex .PRECIOUS
        wbind -n memwin

memwin.rex : memwin.wbj .PRECIOUS
        wlink $(LNKOPT) system win386 file memwin.wbj name memwin

mouse.exe : mouse.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file mouse name mouse

screen.exe : screen.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file screen name screen

screenpl.exp : screenpl.obj .PRECIOUS
        wlink $(LNKOPT) system pharlap file screenpl name screenpl

simulate.exe : simulate.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file simulate name simulate

spwnrd4g.exe : spwndd4g.exe spwnrd4g.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file spwnrd4g name spwnrd4g

spwndd4g.exe : spwndd4g.obj .PRECIOUS
        wlink $(LNKOPT) system dos4g file spwndd4g name spwndd4g

spwnrpls.exp : spwndpls.exp spwnrpls.obj .PRECIOUS
        wlink $(LNKOPT) system pharlap runt minr=300K,maxr=400K file spwnrpls name spwnrpls

spwndpls.exp : spwndpls.obj .PRECIOUS
        wlink $(LNKOPT) system pharlap file spwndpls name spwndpls

mix1.exe : mix1c.obj mix1f.obj .PRECIOUS
        wlink $(LNKOPT) file mix1c,mix1f name mix1

mix2.exe : mix2c.obj mix2f.obj .PRECIOUS
        wlink $(LNKOPT) file mix2c,mix2f name mix2

mix3.exe : mix3c.obj mix3f.obj .PRECIOUS
        wlink $(LNKOPT) file mix3c,mix3f name mix3

mix4.exe : mix4c.obj mix4f.obj .PRECIOUS
        wlink $(LNKOPT) file mix4c,mix4f name mix4

mix5.exe : mix5c.obj mix5f.obj .PRECIOUS
        wlink $(LNKOPT) file mix5c,mix5f name mix5

mix6.exe : mix6.obj .PRECIOUS
        wlink $(LNKOPT) file mix6 name mix6

run_all:    .SYMBOLIC
        %make run_mix
!ifeq PROCESSOR 386
        -bimodal.exe
        -fhandles.exe
        -fmemory.exe
        -fscreen.exe
        -handles.exe
        -keystat.exe
        -run386 keystapl.exp
        -memory.exe
        -run386 mempls40.exp
        -memwin.exe
        -mouse.exe
        -screen.exe
        -run386 screenpl.exp
        -simulate.exe
        -spwnrd4g.exe
        -run386 spwnrpls.exp
!endif

run_mix :   .SYMBOLIC
        -mix1.exe
        -mix2.exe
        -mix3.exe
        -mix4.exe
        -mix5.exe
        -mix6.exe

clean : .SYMBOLIC
        del *.obj
        del *.wbj
        del *.rex
        del *.exe
        del *.exp

# implicit rules

.c.obj :
        $(CC) $^& $(CFLAGS)

.c.wbj :
        $(CC) $^& $(CFLAGS) -zw -fo=$^&.wbj

.for.obj :
        $(FC) $^& $(FFLAGS)

.asm.obj :
        $(ASM) $(AFLAGS) $^&
