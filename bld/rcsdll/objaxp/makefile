host_OS = nt
host_CPU = axp

.mif : $(mif_path)

!include cproj.mif

CC = wppaxp
CFLAGS = -d2-w4-bd-zq-bt=nt-i..
OBJS = rcsdll.obj mkssi.obj inifile.obj

.EXTENSIONS:
.EXTENSIONS: .obj
.EXTENSIONS: .cpp

rcsdll.dll : $(OBJS)
    $(linker) op map d all system ntaxp_dll fi { $(OBJS) } op undefsok
    $(librarian) -b -n rcsdll.lib +rcsdll.dll

.cpp : ..;..\mkssi

.cpp.obj : .AUTODEPEND
    $(cpp) $< $(CFLAGS)
