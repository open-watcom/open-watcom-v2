EXTNAME = w386dll

OBJS = begin.obj &
       wininit.dbj &
        windll.obj &
        libmain.dbj &
        windpmi.dbj &
        wincall.dbj &
        call16.obj &
        clib.dbj &
        fints.dbj &
        windata.dbj &
        winstubs.dbj &
        win16thk.obj &
        winglue.obj &
        wincb.obj &
        bitmap.dbj &
        int21win.obj &
        dllthunk.dbj &
        dllpatch.obj &
        dllthk.obj &
        toolhelp.cbj

#       ole.cbj

!include ..\win386.mif

extralink : .SYMBOLIC
    @%create $(EXTRALNK)
    @%append $(EXTRALNK) name w386dll.ext
    @%append $(EXTRALNK) sys windows dll initinstance memory
    @%append $(EXTRALNK) option heapsize=8K,stack=8K
    @%append $(EXTRALNK) option align=9
    @%append $(EXTRALNK) segment CLASS 'EMU' PRELOAD MOVEABLE
    @%append $(EXTRALNK) @exports.lnk
