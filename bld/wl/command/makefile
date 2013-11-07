host_os  = nt
host_cpu = 386

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

compile = $(comp386) $[@ -ox-4-d1-w4-zq-i"$(watcom_h)"

.c: ../c
.c.obj :
        $(compile)

command.exe : $(objs) wlink.res
        @%create tmp.lnk
        @for %i in ($(objs)) do @%append tmp.lnk file %i
        $(linker) name $^&.exp debug all sys pharlap option undefsok,map libpath $(wres_dir)/lib lib wresf @tmp.lnk
!ifndef new_pharlap
        $(pharlap)bind386 $(pharlap)run386b.exe $^&.exp -exe $^@
!else
        copy /b $(pharlap)stub386.exe + $^&.exp $^@
!endif
        $(wstrip) -q -a -r command.exe . wlink.res

trmem.obj : $(%OWSRCDIR)/trmem/trmem.c
        $(compile) -i="$(%OWSRCDIR)/trmem"

dmem.obj : mem.c
        $(compile) -dTRACKER -fo=dmem.obj -of+ -i="$(%OWSRCDIR)/trmem"

msg.obj : msg.c ../h/msg.h ../h/wlnkmsg.h
        $(compile) -i="$(lib_misc_dir)/h"

demangle.obj : $(lib_misc_dir)/c/demangle.c
        $(compile) -i="$(lib_misc_dir)/h"

!include ../master.mif
