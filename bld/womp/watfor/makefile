!include $(womp_dir)\watfor\files.mif

.c: $(womp_dir)\c
!ifdef __386__
.c.obj
    $(libcomp386) $[@ /mf/d1/oax/w4/zq/zl/fo=$^@
!else
.c.obj
    $(libcomp286) $[@ /ml/d1/oax/w4/zq/zl/fo=$^@
!endif

.before:
    set include=$(wf)lg86\h;$(womp_dir)\h;$(bld_h);$(watcom_dir)\h;$(wf)h

!ifdef __386__
$(womp_dir)\watfor\obj386\wompobj.lib : $(womp_dir)\h\wmpmsg.h $(womp_objs)
!else
$(womp_dir)\watfor\obj\wompobj.lib : $(womp_dir)\h\wmpmsg.h $(womp_objs)
!endif
    %create tmp.lbc
    @for %i in ($(womp_objs)) do @%append tmp.lbc -+%i
    wlib /b/n/q $^@ @tmp.lbc
    del tmp.lbc

__watfor__      =
!include $(womp_dir)\mif\master.mif
