!include ../build.mif

CFLAGS = -zW -oaxt -d1 -w4 -fpc
!ifeq WIN386 1
CC = wcc386p
!else
CC = wccp
!endif
NAME = testctl

LNK = $(name).lnk

OBJS = testctl.obj check.obj combo.obj combomod.obj ctltype.obj ctluser.obj dcombo.obj float.obj int.obj radio.obj rfloat.obj rint.obj text.obj textmod.obj

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
    @%append $(LNK) option heapsize=1k
    @%append $(LNK) option symfile = $(name).sym
    @%append $(LNK) library windows
!endif
    @%append $(LNK) option stack=7k
    @%append $(LNK) name $(name)
    @for %i in ($(OBJS)) do @%append $(LNK) file %i

.c.obj :
        $(CC) $(CFLAGS) $[*
