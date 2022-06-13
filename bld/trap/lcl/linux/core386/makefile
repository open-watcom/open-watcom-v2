#pmake: lcl core os_linux cpu_386 trap x86
#
# Create the Linux core.trp
#
host_cpu = 386
host_os  = linux

target_cpu = x86
target_os  = linux

srv     = lcl
name    = core
which   = TRAP

extra_objs = &
        lnxcore.obj &
        lnxcomm.obj &
        lnxfile.obj &
        supp.obj &
        core.obj

!include $(trap_dir)/lcl/linux/linux.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
