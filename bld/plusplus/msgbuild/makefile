# msgbuild -- version(2) message compaction utility

.extensions:
.extensions: .exe .lib .lnk .obj .cpp

optimized = yes

wpp286 = $(comp286plus)

!ifdef optimized
.cpp.obj : .AUTODEPEND
    $(wpp286) $[* -oailt -s -d1 -zq -w3 -ml -xs

dbg_options = line
map_options =
!else
.cpp.obj : .AUTODEPEND
    $(wpp286) $[* -xs-ew-d2-zm-bt=os2-w4-zq -ml

dbg_options = all
map_options = ,map
!endif

object_files =
object_files +=  carve.obj
object_files +=  encstats.obj
object_files +=  exc.obj
object_files +=  filectl.obj
object_files +=  hash.obj
object_files +=  msg.obj
object_files +=  ring.obj
object_files +=  str.obj
object_files +=  utility.obj
object_files +=  word.obj

msgencod.exe : $(object_files) dosencod.exe msgencod.lnk
    $(linker) @msgencod.lnk

msgencod.lnk : makefile
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ SYSTEM os2 pmc
    @%append $^@ OPTION quiet, heap=256k, stub=dosencod.exe, stack=16k $(map_options)
    @%append $^@ DEBUG $(dbg_options)
    @%append $^@ FILE msgbuild
    @%append $^@ LIBRARY msgbuild.lib

dosencod.exe : msgbuild.lib msgbuild.obj dosencod.lnk
    $(linker) @dosencod.lnk

dosencod.lnk : makefile
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ SYSTEM dos
    @%append $^@ OPTION quiet, stack=32k $(map_options)
    @%append $^@ DEBUG $(dbg_options)
    @%append $^@ FILE msgbuild
    @%append $^@ LIBRARY msgbuild.lib

msgbuild.wlb : makefile
    @%create $^@
    @for %i in ($(object_files)) do @%append $^@ + %i

msgbuild.lib : $(object_files) msgbuild.wlb
    wlib -b -q -n msgbuild.lib @msgbuild.wlb

global: .symbolic
    -rm -f *.err *.exe *.obj *.lib *.lnk *.wlb *.map

reference : .symbolic msgencod.exe
    cp msgencod.exe ../bin
