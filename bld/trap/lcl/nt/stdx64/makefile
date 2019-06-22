#pmake: nobuild lcl os_nt cpu_x64 trap x86

host_os  = nt
host_cpu = x64

target_cpu = x86
target_os  = nt

#
# Create the NT STD32.DLL
#
name  = std32
srv   = lcl
which = TRAP

extra_objs = x86cpu.obj supphdlr.obj

!include $(trap_dir)/lcl/nt/nt.mif

