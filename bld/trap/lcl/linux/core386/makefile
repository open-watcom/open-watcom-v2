#pmake: lcl core os_linux cpu_386 trap x86
#
# Create the Linux core.trp
#
host_cpu = 386
host_os  = linux

os      = linux
srv     = lcl
cpu     = x86
name    = core
which   = TRAP

extra_objs = &
        lnxcore.obj &
        lnxcomm.obj &
        lnxfile.obj &
        supp.obj &
        core.obj
        
inc_dirs   = -I"$(trap_dir)/lcl/linux/h"

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
