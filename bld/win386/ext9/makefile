EXTNAME = win386

OBJS = begin.9bj  &
        wininit.obj &
        winext.9bj &
        winmain.obj &
        winglue.obj &
        wincall.obj &
        windpmi.obj &
        clib.obj &
        fints.obj &
        tinyio.obj &
        windata.obj &
        winstubs.obj &
        winmisc.obj &
        wincb.obj &
        bitmap.obj &
        cbgen.obj &
        dllthk.obj &
        dllthunk.pbj &
        commdlg.nbj &
        ole.nbj &
        penwin.nbj &
        mmsystem.nbj

!include ..\win386.mif

extralink : .SYMBOLIC
    @%create $(EXTRALNK)
    @%append $(EXTRALNK) name win386.ext
    @%append $(EXTRALNK) sys windows mem font
    @%append $(EXTRALNK) option stack=12K,heapsize=10k,align=512
    @%append $(LNK) segment CLASS 'STACK' PRELOAD FIXED
