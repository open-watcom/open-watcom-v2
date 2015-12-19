#pmake: trap remote os_dos cpu_i86 cpu_386 tcp x86
host_cpu = i86
host_os  = dos

target_cpu = x86
target_os  = dos

memory_model_i86 = l

# NOTE: This is not working at present...

srv = tcp
which = TRAP
remote_link = 1

inc_dirs   = -I"$(wattcp_dir)/inc"
extra_libs = $(wattcp_dir)/dosi86/wattcpl.lib

!include $(trap_dir)/master.mif
