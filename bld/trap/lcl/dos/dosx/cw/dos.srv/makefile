#pmake: build serv lcl os_dos dsx cpu_386 x86

tree_depth = 7

host_os  = dos
host_cpu = 386

target_cpu = x86
target_os  = cw

srv = cw
which = SERVER

extra_objs = autolock.obj cwacc.obj cwintr.obj
extra_incs = -I"$(causeway_dir)/inc" -I"$(comp_cfg_dir)/h"

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
