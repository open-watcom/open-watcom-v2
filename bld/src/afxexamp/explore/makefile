CC = wpp386
CFLAGS = -bt=nt -zq -fh
OBJS = explore.obj explfrm.obj expldoc.obj leftview.obj rightvw.obj itemdlg.obj
LNK = explore.lnk

explore.exe : $(OBJS) $(LNK) explore.res
    wlink @$(LNK)
    wrc -q explore.res

explore.res : explore.rc resource.h toolbar.bmp
    wrc -q -r -fo=explore.res $[@

$(LNK) : makefile
    %create $(LNK)
    @%append $(LNK) name explore.exe
    @%append $(LNK) op quiet
    @%append $(LNK) system nt_win
    @for %i in ($(OBJS)) do @%append $(LNK) file %i

.cpp.obj : .autodepend
    $(CC) $(CFLAGS) $[*

clean : .symbolic
    rm -f *.obj *.err *.exe *.lnk *.res *.pch
