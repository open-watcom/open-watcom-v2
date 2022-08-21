#pmake: nobuild lcl os_nt cpu_x64 trap x86

tree_depth = 5

host_os  = nt
host_cpu = x64

target_cpu = x86

#
# Create the NT STD32.DLL
#
name  = std32
srv   = lcl
which = TRAP

extra_objs = x86cpu.obj supphdlr.obj $(nt_trap_objs)

extra_cppflags += -DWOW

!include $(trap_dir)/lcl/nt/nt.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
