!include ..\build.mif

CFLAGS = -zW -oaxt -d1 -w4 -i..\filedll
!ifeq WIN386 1
CC = wcc386p
!else
CC = wccp
!endif
NAME = edit

LNK = $(name).lnk

OBJS = edit.obj efile.obj eprint.obj efont.obj emem.obj

$(name).exe : $(OBJS) $(name).res $(LNK)
    wlink @$(LNK)
!ifeq WIN386 1
    wbind $(name) -R -30 $(name).res
!else
    wrc -30 $(name).res
    copy /b $(name).exe+$(name).sym
    del $(name).sym
!endif

$(name).res : $(name).rc
    wrc -r -30 $(name).rc

$(LNK) : makefile ..\build.mif
    %create $(LNK)
    @%append $(LNK) debug all
!ifeq WIN386 1
    @%append $(LNK) sys win386
    @%append $(LNK) option mindata=100K
    @%append $(LNK) option maxdata=100K
!else
    @%append $(LNK) sys windows
    @%append $(LNK) option heapsize=20k
    @%append $(LNK) option symfile = $(name).sym
    @%append $(LNK) library windows
!endif
    @%append $(LNK) option stack=8k
    @%append $(LNK) name $(name)
    @for %i in ($(OBJS)) do @%append $(LNK) file %i

.c.obj :
        $(CC) $(CFLAGS) $[*
