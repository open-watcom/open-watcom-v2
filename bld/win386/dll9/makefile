EXTNAME = w386dll

OBJS = begin.9bj &
       wininit.dbj &
        windll.obj &
        libmain.dbj &
        windpmi.dbj &
        wincall.dbj &
        clib.dbj &
        fints.dbj &
        tinyio.dbj &
        windata.dbj &
        winstubs.dbj &
        winglue.obj &
        winmisc.obj &
        wincb.obj &
        cbgen.dbj &
        bitmap.dbj &
        dllthunk.ebj &
        dllthk.obj &
        commdlg.cbj &
        ole.cbj &
        penwin.cbj &
        mmsystem.cbj

!include ..\win386.mif

extralink : .SYMBOLIC
    @%create $(EXTRALNK)
    @%append $(EXTRALNK) name w386dll.ext
!ifdef tools_8_0
    @%append $(EXTRALNK) form windows dll initinstance memory
!else
    @%append $(EXTRALNK) sys windows dll initinstance memory
!endif
    @%append $(EXTRALNK) option heapsize=32K
    @%append $(EXTRALNK) option align=512
    @%append $(EXTRALNK) export WEP.1 RESIDENT
    @%append $(EXTRALNK) export Win386LibEntry.2 RESIDENT
