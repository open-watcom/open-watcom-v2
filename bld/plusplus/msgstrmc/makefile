# MSGSTRM utility -- make message streams

.EXTENSIONS:
.EXTENSIONS: .exp .obj .cpp

.cpp: ..\cpp

OBJS = msgstrmc.obj &
       conshelp.obj &
       iters.obj    &
       iterfrec.obj &
       lineout.obj  &
       excfile.obj  &
       excpgm.obj

compiler = /d1 /w4 /zq /ox /os /xs /zm /ml

msgstrmc.lnk : makefile $(OBJS)
    %create $^@
#   @%append $^@ SYSTEM pharlap
    @%append $^@ SYSTEM dos
    @%append $^@ OPTION quiet, strip, map, show
    @%append $^@ NAME   $^*
    @%append $^@ DEBUG  all
    @for %i in ( $(OBJS) ) do @%append $^@ FILE %i

msgstrmc.exe : $(OBJS) makefile msgstrmc.lnk
    $(linker) @msgstrmc
#   $(pharlap)bind386 $(pharlap)run386b msgstrmc.exp -exe msgstrmc.exe
#   @rm -f msgstrmc.exp

.cpp.obj:
    $(comp286plus) $[* $(compiler)

reference : .symbolic
    cp msgstrmc.exe $(plusplus_dir)/bin

global : .symbolic
    @rm -f *.obj *.exe *.exp
