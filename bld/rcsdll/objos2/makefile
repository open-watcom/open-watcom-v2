CC = wpp386
!ifeq tools_10_0 1
CFLAGS = -d2-w4-bd-zq-mf-bt=os2 -i=$(watcom_os220_h)
!else
CFLAGS = -d2-w4-bd-zq-mf-bt=os2 -i=$(lang_root)\h\os2
!endif
OBJS = rcsdll.obj inifile.obj

.EXTENSIONS:
.EXTENSIONS: .obj
.EXTENSIONS: .cpp

rcsdll.dll : $(OBJS)
    *wlink op map d all sys os2v2 dll initinstance terminstance pm debug all option manyautodata, quiet, caseexact fi { $(OBJS) }
    wlib /n /q /b /c $^*.lib +$^*.dll

.cpp : ..;..\mkssi

.cpp.obj : .AUTODEPEND
    $(CC) $< $(CFLAGS)
