#pmake: lcl pmd os_qnx cpu_i86 cpu_386 trap x86
#
# Create the QNX PMD.TRP
#
host_cpu = 386
host_os  = qnx

target_cpu = x86
target_os  = qnx

srv     = lcl
name    = pmd
which = TRAP

extra_objs = &
        qnxpmd.obj &
        qnxcomm.obj &
        qnxfile.obj &
        misc7386.obj &
        supp.obj &
        core.obj

!include $(trap_dir)/lcl/qnx/qnx.mif
!include $(trap_dir)/master.mif
