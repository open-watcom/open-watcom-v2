#pmake: lcl os_qnx cpu_i86 cpu_386 trap x86
#
# Create the QNX STD.TRP
#
host_cpu = 386
host_os  = qnx

target_cpu = x86

os      = qnx
srv     = lcl
which = TRAP

extra_objs = &
        qnxacc.obj &
        qnxcomm.obj &
        qnxfile.obj &
        misc7386.obj &
        supp.obj &
        core.obj

inc_dirs = -I"$(trap_dir)/lcl/h"

!include $(trap_dir)/master.mif
