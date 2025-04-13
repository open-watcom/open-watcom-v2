#pmake: nobuild lcl os_nt cpu_x64 trap x86

tree_depth = 5

host_os  = nt
host_cpu = x64

sys_dll = 1

#
# Create the NT STD.DLL
#
name  = std
srv   = lcl
which = TRAP

extra_objs = x86cpu.obj supphdlr.obj $(nt_trap_objs)

!include $(trap_dir)/lcl/nt/nt.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
