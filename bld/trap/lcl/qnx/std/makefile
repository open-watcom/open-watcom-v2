#pmake: lcl os_qnx cpu_i86 cpu_386 trap x86
#
# Create the QNX STD.TRP
#
host_CPU = 386
host_os = qnx

os      = qnx
srv     = lcl
cpu     = x86
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
