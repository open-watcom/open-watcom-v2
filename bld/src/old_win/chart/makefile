!include ../build.mif

CFLAGS = -zW -oaxt -d1 -w4 -fpc
!ifeq WIN386 1
CC = wcc386p
!else
CC = wccp
!endif
NAME = testcht
DLLNAME = chart

LNK = $(NAME).lnk
DLLLNK = $(DLLNAME).lnk

OBJS = testcht.obj testcht2.obj ctluser.obj

DATACTL = ..\datactl\ctltype.obj ..\datactl\float.obj ..\datactl\rfloat.obj ..\datactl\rint.obj ..\datactl\textmod.obj ..\datactl\int.obj

!ifeq WIN386 0
doit : .SYMBOLIC $(DLLNAME).dll $(NAME).exe
    @%NULL
!endif

$(NAME).exe : $(OBJS) $(NAME).res $(LNK) $(DATACTL)
    wlink @$(LNK)
!ifeq WIN386 1
    wbind $(NAME) -R -30 $(NAME).res
!else
    wrc -30 $(NAME).res
    copy /b $(NAME).exe+$(NAME).sym
    del $(NAME).sym
!endif

$(DLLNAME).dll : chart.obj $(DLLLNK)
    wlink @$(DLLLNK)
    wrc -30 $(DLLNAME).dll
    copy /b $(DLLNAME).dll+$(DLLNAME).sym
    del $(DLLNAME).sym
!ifneq WIN386 1
    erase $(DLLNAME).lib
    wlib $(DLLNAME).lib +$(DLLNAME).dll
!endif

$(NAME).res : $(NAME).rc
    wrc -r -30 $(NAME).rc

$(LNK) : makefile ..\build.mif
    %create $(LNK)
    @%append $(LNK) debug all
!ifeq WIN386 1
    @%append $(LNK) sys win386
    @%append $(LNK) option mindata=100K
    @%append $(LNK) option maxdata=100K
!else
    @%append $(LNK) sys windows
    @%append $(LNK) library windows
    @%append $(LNK) library chart
    @%append $(LNK) option symfile = $(NAME).sym
    @%append $(LNK) option stack=7k,heapsize=1k
!endif
    @%append $(LNK) name $(NAME)
    @for %i in ($(OBJS)) do @%append $(LNK) file %i
    @for %i in ($(DATACTL)) do @%append $(LNK) file %i


$(DLLLNK) : makefile
    %create $(DLLLNK)
    @%append $(DLLLNK) debug all
    @%append $(DLLLNK) sys windows dll
    @%append $(DLLLNK) library windows
    @%append $(DLLLNK) library clibl
    @%append $(DLLLNK) name $(DLLNAME).dll
    @%append $(DLLLNK) option symfile = $(DLLNAME).sym
    @%append $(DLLLNK) option heapsize=1k
    @%append $(DLLLNK) option oneautodata
    @%append $(DLLLNK) libfile libentry.obj
    @%append $(DLLLNK) file chart.obj
    @%append $(DLLLNK) export WEP RESIDENT


bardlg.h : bardlg.ctl
    ..\datactl\parsectl bardlg.ctl bardlg.h DlgData

chart.obj : chart.c
    wccp -zW -zu -zc -oaxt -d1 -w4 -ml -fpc $[*

ctluser.obj : ctluser.c
    $(CC) $(CFLAGS) -I..\datactl $[*

testcht.obj : testcht.c bardlg.h
    $(CC) $(CFLAGS) -I..\datactl $[*

testcht2.obj : testcht2.c
    $(CC) $(CFLAGS) -I..\datactl $[*

..\datactl\ctltype.obj :
    cd ..\datactl
    wmake
    cd ..\chart

..\datactl\float.obj :
    cd ..\datactl
    wmake
    cd ..\chart

..\datactl\rfloat.obj :
    cd ..\datactl
    wmake
    cd ..\chart

..\datactl\rint.obj :
    cd ..\datactl
    wmake
    cd ..\chart

..\datactl\textmod.obj :
    cd ..\datactl
    wmake
    cd ..\chart

..\datactl\int.obj :
    cd ..\datactl
    wmake
    cd ..\chart
