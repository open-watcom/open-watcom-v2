CC = wpp386
CFLAGS = -bt=nt -zq -fh
OBJS = hello.obj hellownd.obj
LNK = hello.lnk

hello.exe : $(OBJS) $(LNK) hello.res
    wlink @$(LNK)
    wrc -q hello.res

hello.res : hello.rc resource.h
    wrc -q -r -fo=hello.res $[@

$(LNK) : makefile
    %create $(LNK)
    @%append $(LNK) name hello.exe
    @%append $(LNK) op quiet
    @%append $(LNK) system nt_win
    @for %i in ($(OBJS)) do @%append $(LNK) file %i

.cpp.obj : .autodepend
    $(CC) $(CFLAGS) $[*

clean : .symbolic
    rm -f *.obj *.err *.exe *.lnk *.res *.pch
