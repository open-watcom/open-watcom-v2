CC = wpp386
CFLAGS = -bt=nt -zq -fh
OBJS = textedit.obj editfrm.obj textdoc.obj textview.obj
LNK = textedit.lnk

textedit.exe : $(OBJS) $(LNK) textedit.res
    wlink @$(LNK)
    wrc -q textedit.res

textedit.res : textedit.rc resource.h toolbar.bmp
    wrc -q -r -fo=textedit.res $[@

$(LNK) : makefile
    %create $(LNK)
    @%append $(LNK) name textedit.exe
    @%append $(LNK) op quiet
    @%append $(LNK) system nt_win
    @for %i in ($(OBJS)) do @%append $(LNK) file %i

.cpp.obj : .autodepend
    $(CC) $(CFLAGS) $[*

clean : .symbolic
    rm -f *.obj *.err *.exe *.lnk *.res *.pch
