# MSGSTRM utility -- make message streams

.EXTENSIONS:
.EXTENSIONS: .exe .exp .obj .cpp

.cpp: ..\

OBJS = files.obj        &
       fini.obj         &
       model.obj        &
       msgdscr.obj      &
       msgstrm.obj      &
       readgml.obj

compiler = /d1 /w4 /zq /ox /os /xs /ew /ms /bt=dos

msgstrm.exe : $(OBJS) makefile msgstrm.os2 msgstrm.stb
    $(linker) @msgstrm.os2
#   $(pharlap)bind386 $(pharlap)run386b msgstrm.exp -exe msgstrm.exe
#   @rm -f msgstrm.exp

msgstrm.stb : $(OBJS) makefile msgstrm.dos
    $(linker) @msgstrm.dos

msgstrm.os2 : makefile $(objs)
    %create $^@
    %append $^@ SYSTEM os2 pmc
    @%append $^@ OPTION quiet, eliminate, map, stub=msgstrm.stb
    @%append $^@ NAME   $^*
    @%append $^@ DEBUG  all
    @for %i in ( $(OBJS) ) do @%append $^@ FILE %i

msgstrm.dos : makefile $(objs)
    %create $^@
    %append $^@ SYSTEM dos
    @%append $^@ OPTION quiet, eliminate, map
    @%append $^@ NAME   $^*.stb
    @%append $^@ DEBUG  all
    @for %i in ( $(OBJS) ) do @%append $^@ FILE %i

.cpp.obj:
    $(comp286plus) $[* $(compiler)

reference : .symbolic
    cp msgstrm.exe $(plusplus_dir)/bin

global : .symbolic
    @rm -f *.obj *.exe *.exp *.err
