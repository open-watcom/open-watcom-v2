#
#  Makefile for   rescmp
#
#   Date        Name            Reason
#   ----        ----            ------
#   7 June 91   Steve B.        created
#   18 June 91  S.Bosnick       modified for rescmp
#   26 June 91  S.Bosnick       modified for wresdmp
#   16-apr-93   Craig Eisler    fixed up

#which memory model to use
MODEL = c

# path names used in the script
TRMEM_DIR = $(utils_root)\trmem
wresdir =   $(build_root)\sdk\rc\wres\

# name the compiler and linker
CC = $(comp286)
LINK = $(linker)

# options to use
CFLAGS = -m$(MODEL)-fpc-oxt-zq-w4-d1
TRMEM = -dTRMEM -of+

# where to look for various files
.c : ..\c;$(TRMEM_DIR)
.h : ..\h;$(TRMEM_DIR)

.BEFORE
        @set INCLUDE=$(TRMEM_DIR);$(wresdir)h;$(watcom_h);$(%include);

NAME = wresdmp
LNK = $(name).lnk

OBJS = &
    dmpcont.obj &
    dmpcurgr.obj &
    dmpdiag.obj &
    dmpfile.obj &
    dmpicogr.obj &
    dmpmenu.obj &
    dmpstr.obj &
    fatal.obj &
    flag.obj &
    global.obj &
    layer0.obj &
    param.obj &
    swchar.obj &
    wresdmp.obj &
    trmem.obj &
    trmemcvr.obj
# explicit rules

$(name).exe : $(OBJS) $(wresdir)lib\wres$(MODEL).lib $(lnk)
        $(LINK) libpath $(wresdir)lib @$^&.lnk

$(lnk) : makefile
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system dos
        @%append $(lnk) name $(name).exe
        @%append $(lnk) option map
        @for %i in ($(objs)) do @%append $(lnk) file %i

trmemcvr.obj : trmemcvr.c
        $(CC) $[* $(CFLAGS) $(TRMEM)

clean : .SYMBOLIC
        rm *.obj
        rm rescmp.exe

# implicit rules

.c.obj :
        $(CC) $[* $(CFLAGS)
