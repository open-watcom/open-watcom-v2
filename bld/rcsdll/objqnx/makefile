CC = wpp
CFLAGS = -d2-w4-zq-ml-bt=QNX
OBJS = rcsdll.obj inifile.obj

.EXTENSIONS:
.EXTENSIONS: .obj
.EXTENSIONS: .cpp

rcsdll.lib : $(OBJS)
    wlib $^*.lib -+ $(OBJS)

.cpp : ..

.cpp.obj : .AUTODEPEND
    $(CC) $< $(CFLAGS)
