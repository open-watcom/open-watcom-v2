EXTNAME = win386

OBJS = begin.obj  &
        wininit.obj &
        winext.obj &
        winmain.obj &
        win16thk.obj &
        winglue.obj &
        wincall.obj &
        windpmi.obj &
        call16.obj &
        clib.obj &
        fints.obj &
        windata.obj &
        winstubs.obj &
        wincb.obj &
        bitmap.obj &
        dllpatch.obj &
        dllthk.obj &
        int21win.obj &
        dllthunk.obj &
        toolhelp.nbj

#       ole.nbj

!include ..\win386.mif

extralink : .SYMBOLIC
    @%create $(EXTRALNK)
    @%append $(EXTRALNK) name win386.ext
    @%append $(EXTRALNK) sys windows mem font
    @%append $(EXTRALNK) option verbose
    @%append $(EXTRALNK) option rwr
    @%append $(EXTRALNK) option stack=12K,heapsize=8k,align=512
    @%append $(LNK) segment CLASS 'STACK' PRELOAD
