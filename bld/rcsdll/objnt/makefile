CC = wpp386
CFLAGS = -d2-w4-bd-zq-mf-bt=nt-i=$(lang_root)\h\nt;..
OBJS = rcsdll.obj mkssi.obj objcycle.obj inifile.obj

.EXTENSIONS:
.EXTENSIONS: .obj
.EXTENSIONS: .cpp

rcsdll.dll : $(OBJS)
    wlink op map d all system nt_dll fi { $(OBJS) }
    wlib -b -n rcsdll.lib +rcsdll.dll

.cpp : ..;..\mkssi;..\ocycle

.cpp.obj : .AUTODEPEND
    $(CC) $< $(CFLAGS)
