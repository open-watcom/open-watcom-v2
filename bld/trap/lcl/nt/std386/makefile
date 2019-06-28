#pmake: build lcl os_nt cpu_386 trap x86

host_os  = nt
host_cpu = 386

target_cpu = x86
target_os  = nt

#
# Create the NT STD.DLL
#

srv     = lcl
which = TRAP

extra_objs = x86cpu.obj supphdlr.obj

!include $(trap_dir)/lcl/nt/nt.mif

