CC = wpp386
CFLAGS = -d2-w4-zq-mf-bt=DOS -D__PHAR__
OBJS = rcsdll.obj inifile.obj

.EXTENSIONS:
.EXTENSIONS: .obj
.EXTENSIONS: .cpp

rcsdll.lib : $(OBJS)
    wlib $^*.lib -+ $(OBJS)

.cpp : ..

.cpp.obj : .AUTODEPEND
    $(CC) $< $(CFLAGS)
