#pmake: lcl os_os2 cpu_386 trap x86

host_os  = os2
host_cpu = 386

target_cpu = x86
target_os  = os2v2

#
# Create the 32-bit OS/2 core.dll
#

srv     = lcl
name    = core
which   = TRAP
core_only = 1

extra_cppflags = -DELFCORE
extra_cflags = -bd

extra_objs = &
    elfcore.obj &
    coremisc.obj &
#    accmisc.obj &
    x86cpu.obj &
    supp.obj &
    core.obj

inc_dirs = -I"../h" -I"$(trap_dir)/lcl/elfcore/h"

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif

.c : $(trap_dir)/lcl/elfcore/c
