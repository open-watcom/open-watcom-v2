#pmake: trap remote os_dos cpu_i86 cpu_386 tcp x86

tree_depth = 4

host_cpu = i86
host_os  = dos

target_cpu = x86

srv = tcp
which = TRAP
remote_link = 1

memory_model_i86 = l

# NOTE: This is not working at present...

extra_incs = -I"$(wattcp_dir)/inc"
extra_libs = $(wattcp_dir)/dosi86/wattcpl.lib

!include $(trap_dir)/master.mif
