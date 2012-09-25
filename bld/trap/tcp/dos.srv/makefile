#pmake: build serv remote os_dos cpu_i86 cpu_386 tcp x86

host_os  = dos
host_cpu = i86

memory_model_i86 = l

os  = dos
srv = tcp
cpu = x86
which = SERVER
remote_link = 1

inc_dirs     = -I"$(wattcp_dir)/inc"
extra_cflags = -zm
extra_linker = op eliminate
extra_libs   = $(wattcp_dir)/dosi86/wattcpl.lib

!include $(trap_dir)/master.mif

