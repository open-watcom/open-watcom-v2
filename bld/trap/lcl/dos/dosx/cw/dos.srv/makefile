#pmake: build serv lcl os_dos dsx cpu_386 x86

host_os  = dos
host_cpu = 386

target_cpu = x86
target_os  = cw

srv = cw
!ifdef %OWUSE_FILENAME_VERSION
name = cwhelp$(bld_ver)
!else
name = cwhelp
!endif
which = SERVER

extra_objs = autolock.obj cwacc.obj cwintr.obj

extra_cpp_flags = -DCAUSEWAY
!ifdef %OWUSE_FILENAME_VERSION
extra_cpp_flags += -DUSE_FILENAME_VERSION=$(bld_ver)
!endif
extra_a_flags = -DCAUSEWAY

inc_dirs = -I"$(causeway_dir)/inc" -I"$(comp_cfg_dir)/h"

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
