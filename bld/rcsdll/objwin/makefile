CC = wpp
CFLAGS = -d1-w4-bd-zw-zq-zu-ml-bt=windows -i=$(lang_root)\h\win;..
OBJS = rcsdll.obj mkssi.obj inifile.obj

.EXTENSIONS:
.EXTENSIONS: .obj
.EXTENSIONS: .cpp

rcsdll.dll : $(OBJS)
    wlink op map d all system windows_dll fi { $(OBJS) } lib windows
    wlib -b -n rcsdll.lib +rcsdll.dll

.cpp : ..;..\mkssi

.cpp.obj : .AUTODEPEND
    $(CC) $< $(CFLAGS)
