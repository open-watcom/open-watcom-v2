#pmake: lcl os_os2 cpu_386 trap x86

host_os  = os2
host_cpu = 386

#
# Create the 32-bit OS/2 core.dll
#

os      = os2v2
srv     = lcl
cpu     = x86
name    = core
which   = TRAP

extra_cpp_flags = -DELFCORE
extra_cflags = -bd

extra_srcs = $(trap_dir)/lcl/linux/c

extra_objs = &
    lnxcore.obj &
    coremisc.obj &
#    accmisc.obj &
    x86cpu.obj &
    supp.obj &
    core.obj

inc_dirs = -I"$(trap_dir)/lcl/linux/h"

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
