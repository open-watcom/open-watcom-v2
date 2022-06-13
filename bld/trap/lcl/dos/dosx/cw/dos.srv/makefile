#pmake: build serv lcl os_dos dsx cpu_386 x86

host_os  = dos
host_cpu = 386

target_cpu = x86
target_os  = cw

srv = cw
which = SERVER

extra_objs = autolock.obj cwacc.obj cwintr.obj
extra_incs = -I"$(causeway_dir)/inc" -I"$(comp_cfg_dir)/h"

extra_cppflags = -DCAUSEWAY
extra_a_flags = -DCAUSEWAY

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
