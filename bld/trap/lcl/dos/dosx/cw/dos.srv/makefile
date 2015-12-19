#pmake: build serv lcl os_dos dsx cpu_386 x86

host_os  = dos
host_cpu = 386

target_cpu = x86

os = cw
srv = cw
name = cwhelp
which = SERVER

extra_objs = autolock.obj cwacc.obj cwintr.obj
extra_deps = $(causeway_dir)/cw32/dosi86/cwdstub.exe

extra_cpp_flags = -DCAUSEWAY
extra_a_flags = -DCAUSEWAY

inc_dirs = -I"$(causeway_dir)/inc" -I"$(comp_cfg_dir)/h"

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
