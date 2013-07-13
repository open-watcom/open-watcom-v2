#pmake: lcl os_os2 cpu_i86 trap
#
# Create the OS/2 STD16.DLL
#
host_cpu = i86
host_os  = os2

os      = os2
srv     = lcl
cpu     = x86
name    = std16
which = TRAP

extra_objs = &
        accmisc.obj &
        os2v1acc.obj &
        os2rfx.obj &
        loadstd.obj &
        misc7086.obj &
        x86cpu.obj &
        supp.obj &
        core.obj

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
