host_CPU = i86
host_OS = dos
!include cproj.mif
.erase

objs = wrdencod.obj msgencio.obj

.c.obj:
    $(plusplus_dir)\bin\ppsp.exe $[@ $[.
    $(cc) $[. -oailt -s -d1 -zq -w3 -ml
    rm -f $[.

msgencod.exe : $(objs) setbin.obj dosencod.exe msgencod.lnk
    $(linker) @msgencod.lnk

msgencod.lnk : makefile
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ SYSTEM os2 pmc
    @%append $^@ OPTION quiet, heap=8k, stub=dosencod.exe
    @%append $^@ DEBUG line
    @for %i in ( $(objs) ) do @%append $^@ FILE %i
    @%append $^@ FILE setbin
    @%append $^@ LIB siliblw

dosencod.exe : $(objs) dosencod.lnk
    $(linker) @dosencod.lnk

dosencod.lnk : makefile
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ SYSTEM dos
    @%append $^@ OPTION quiet
    @%append $^@ DEBUG line
    @for %i in ( $(objs) ) do @%append $^@ FILE %i
    @%append $^@ LIB siliblw

global : .symbolic
    rm -f *.obj *.exe *.gh *.lnk

reference : .symbolic msgencod.exe
    cp msgencod.exe ../bin
