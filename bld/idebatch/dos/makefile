.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .wbj .obj
.EXTENSIONS: .c .asm

CC = wcc
RC = wrc
LINK = wlink

CFLAGS = -osl-s-w3-d1

NAME = int
NAMEWIN = winint

LNK = $(name).lnk
LNKWIN = $(namewin).lnk
LNKWINE = $(namewin)E.lnk

OBJS = $(name).obj link.obj int21.obj

OBJSWIN = $(namewin).wbj link.wbj
OBJSWINE = $(namewin)e.wbj link.wbj

#all : $(name).exe $(namewin).exe $(namewin)e.exe .SYMBOLIC
all : $(name).exe $(namewin).exe .SYMBOLIC
    @%null

$(name).exe : $(OBJS) $(LNK)
    $(link) d a @$(LNK)

$(namewin).exe : $(OBJSWIN) $(LNKWIN) $(namewin).res
    $(link) @$(LNKWIN)
    $(rc) $(namewin).res
    @copy /b $(namewin).exe+$(namewin).sym
    @del $(namewin).sym

$(namewin)e.exe : $(OBJSWINE) $(LNKWINE) $(namewin).res
    $(link) @$(LNKWINE)
    $(rc) $(namewin).res $(namewin)e.exe
    @copy /b $(namewin)E.exe+$(namewin)E.sym
    @del $(namewin)E.sym

$(namewin).res : $(namewin).rc
    $(rc) -bt=windows -r $(namewin).rc

$(LNK) : makefile
    %create $(LNK)
    @%append $(LNK) sys dos
    @%append $(LNK) option stack=1500
    @%append $(LNK) name $(name)
    @for %i in ($(OBJS)) do @%append $(LNK) file %i

$(LNKWIN) : makefile
    %create $(LNKWIN)
    @%append $(LNKWIN) debug all
    @%append $(LNKWIN) sys windows font mem
    @%append $(LNKWIN) option symfile=$(namewin).sym
    @%append $(LNKWIN) option stack=10k
    @%append $(LNKWIN) name $(namewin)
    @for %i in ($(OBJSWIN)) do @%append $(LNKWIN) file %i
    @%append $(LNKWIN) segment CLASS 'CODE' PRELOAD FIXED
    @%append $(LNKWIN) segment CLASS 'FAR_DATA' PRELOAD FIXED
    @%append $(LNKWIN) segment CLASS 'BEGDATA' PRELOAD FIXED
    @%append $(LNKWIN) segment CLASS 'DATA' PRELOAD FIXED
    @%append $(LNKWIN) segment CLASS 'BSS' PRELOAD FIXED

$(LNKWINE) : makefile
    %create $(LNKWINE)
    @%append $(LNKWINE) debug all
    @%append $(LNKWINE) sys windows font mem
    @%append $(LNKWINE) option symfile=$(namewin)E.sym
    @%append $(LNKWINE) option stack=10k
    @%append $(LNKWINE) name $(namewin)e
    @for %i in ($(OBJSWINE)) do @%append $(LNKWINE) file %i
    @%append $(LNKWINE) segment CLASS 'CODE' PRELOAD FIXED
    @%append $(LNKWINE) segment CLASS 'FAR_DATA' PRELOAD FIXED
    @%append $(LNKWINE) segment CLASS 'BEGDATA' PRELOAD FIXED
    @%append $(LNKWINE) segment CLASS 'DATA' PRELOAD FIXED
    @%append $(LNKWINE) segment CLASS 'BSS' PRELOAD FIXED

$(namewin)e.wbj : $(namewin).c
        $(CC) -zW -ml -d__EDITOR__ -dSERVER -zu $(CFLAGS) $[* -fo=$(namewin)e.wbj

.c.obj : $[*
        $(CC) -ms $(CFLAGS) $[*

.c.wbj : $[*
        $(CC) -zW -ml -dSERVER -zu $(CFLAGS) $[* -fo=.wbj

.asm.obj : $[*
        masm /Mx /Zd /s $[*;
