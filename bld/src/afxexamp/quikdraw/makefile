CC = wpp386
CFLAGS = -bt=nt -zq -fh
OBJS = quikdraw.obj quikfrm.obj drawfrm.obj drawdoc.obj drawview.obj shapes.obj
LNK = quikdraw.lnk

quikdraw.exe : $(OBJS) $(LNK) quikdraw.res
    wlink @$(LNK)
    wrc -q quikdraw.res

quikdraw.res : quikdraw.rc resource.h toolbar.bmp
    wrc -q -r -fo=quikdraw.res $[@

$(LNK) : makefile
    %create $(LNK)
    @%append $(LNK) name quikdraw.exe
    @%append $(LNK) op quiet
    #@%append $(LNK) system nt_win
    @for %i in ($(OBJS)) do @%append $(LNK) file %i

.cpp.obj : .autodepend
    $(CC) $(CFLAGS) $[*

clean : .symbolic
    rm -f *.obj *.err *.exe *.lnk *.res *.pch
