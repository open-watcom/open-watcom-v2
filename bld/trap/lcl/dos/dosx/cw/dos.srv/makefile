#pmake: build serv lcl os_dos dsx cpu_386 x86

host_os = dos
host_cpu = 386

os = cw
srv = cw
cpu = x86
name = cwhelp
which = SERVER

extra_objs = &
    autolock.obj &
    cwacc.obj &
    cwintr.obj

extra_c_flags = -ms -DCAUSEWAY

extra_a_flags = -ms -DCAUSEWAY

inc_dirs = -I"$(cw_dir)/inc" -I"$(comp_cfg_dir)/h"

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
