#pmake: build lcl os_nt cpu_386 trap x86

tree_depth = 5

host_os  = nt
host_cpu = 386

target_cpu = x86

#
# Create the NT STD.DLL
#

srv     = lcl
which = TRAP

extra_objs = x86cpu.obj supphdlr.obj $(nt_trap_objs)

extra_cppflags += -DWOW

!include $(trap_dir)/lcl/nt/nt.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
