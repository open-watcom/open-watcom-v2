compile = $(comp386) $[@ -ox-4-d1-w4-zq-i$(watcom_h)

.c: ../c
.c.obj :
        $(compile)

objs = &
cmdall.obj &
cmddos.obj &
cmdline.obj &
cmdnov.obj &
cmdos2.obj &
cmdphar.obj &
cmdqnx.obj &
cmdutils.obj &
demangle.obj &
msg.obj &
ntio.obj &
linkutil.obj &
wlnkmsg.obj &
global.obj &
trmem.obj &
dmem.obj &
command.obj

command.exe : $(objs) wlink.res
        @%create tmp.lnk
        @for %i in ($(objs)) do @%append tmp.lnk file %i
        $(linker) debug all sys pharlap option undefsok,map libpath $(wres_dir)/lib lib wresf name command @tmp.lnk
!ifndef new_pharlap
        $(pharlap)bind386 $(pharlap)run386b.exe command -exe $^@
!else
        copy/b $(pharlap)stub386.exe+command.exp $^@
!endif
        $(wstrip) -q -a -r command.exe . wlink.res

trmem.obj : $(%OWROOT)/bld/trmem/trmem.c
        $(compile) -i="$(%OWROOT)/bld/trmem"

dmem.obj : mem.c
        $(compile) -dTRACKER -fo=dmem.obj -of+ -i="$(%OWROOT)/bld/trmem"

msg.obj : msg.c ../h/msg.h ../h/wlnkmsg.h
        $(compile) -i="$(lib_misc_dir)/h"

demangle.obj : $(lib_misc_dir)/c/demangle.c
        $(compile) -i="$(lib_misc_dir)/h"

host_os  = nt
host_cpu = 386

!include ../wlink.mif
