#pmake: lcl os_nt cpu_axp trap axp
#
# Create the NT STD.DLL for Alpha
#

tree_depth = 5

host_os  = nt
host_cpu = axp

srv     = lcl
which   = TRAP

extra_objs = $(nt_trap_objs)

#extra_cppflags += -DWOW

!include $(trap_dir)/lcl/nt/nt.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
