.erase

.c: $(wsampledir)dump
.c.obj :
    wccp $[@ -mc -oas -d1 -w4 -s -zq

.obj.exe:
    wlink NAME $^* DEBUG line OPTION quiet FORM dos FILE $[*

.before
    set include=$(%include);..\h;$(wsampledir)h;$(ui_h)

#all : smpsync.exe mergehlp.exe smpmt.exe .symbolic
#@%null

all : smpdump.exe smpsync.exe mergehlp.exe smpmt.exe .symbolic
    @%null
