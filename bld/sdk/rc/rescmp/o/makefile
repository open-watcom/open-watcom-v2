#
#  Makefile for   rescmp
#
#   Date        Name        Reason
#   ----        ----        ------
#   7 June 91   Steve B.    created
#   18 June 91  S.Bosnick   modified for rescmp

#which memory model to use
MODEL = s       #small

# path names used in the script
TRMEM_DIR = $(utils_root)\trmem
wresdir =   $(build_root)\sdk\rc\wres\

# name the compiler and linker
CC = wccp
LINK = wlinkp

NAME = rescmp
LNK = $(NAME).lnk

# options to use
CFLAGS = -d1 -m$(MODEL) -fpc -oxt -zq -w4
TRMEM = -dTRMEM -of+

# where to look for various files
.c : ..\c;$(TRMEM_DIR)
.h : ..\h;$(TRMEM_DIR)

OBJS = bincmp.obj cmpcont.obj cmpfiles.obj cmpres.obj global.obj &
       layer0.obj param.obj rescmp.obj swchar.obj &
       trmem.obj trmemcvr.obj

.BEFORE
        set INCLUDE=$(TRMEM_DIR);$(wresdir)h;$(watcom_h);$(%include);

# explicit rules

$(NAME).exe : $(OBJS) $(wresdir)lib\wres$(MODEL).lib $(LNK)
        $(LINK) @$(LNK)

$(LNK): makefile
        @%write $(LNK) option quiet
!ifndef on_build_machine
        @%append $(LNK) debug all
!endif
        @%append $(LNK) sys dos
        @%append $(LNK) libpath $(WRESDIR)lib
        @%append $(LNK) name $(NAME)
        @%append $(LNK) option map
        @for %i in ($(objs)) do @%append $(lnk) file %i

trmemcvr.obj : trmemcvr.c
        $(CC) $[* $(CFLAGS) $(TRMEM)

clean : .SYMBOLIC
        rm *.obj
        rm $(NAME).exe

# implicit rules

.c.obj :
        $(CC) $[* $(CFLAGS)
