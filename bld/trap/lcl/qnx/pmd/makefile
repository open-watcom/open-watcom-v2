#pmake: lcl pmd os_qnx cpu_i86 cpu_386 trap x86
#
# Create the QNX PMD.TRP
#
host_cpu = 386
host_os = qnx

os      = qnx
srv     = lcl
cpu     = x86
name    = pmd
which = TRAP

extra_objs = &
        qnxpmd.obj &
        qnxcomm.obj &
        qnxfile.obj &
        misc7386.obj &
        supp.obj &
        core.obj

inc_dirs = -I"$(trap_dir)/lcl/h"

!include $(trap_dir)/master.mif
