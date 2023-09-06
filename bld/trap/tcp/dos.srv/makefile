#pmake: build serv remote os_dos cpu_i86 cpu_386 tcp x86

tree_depth = 4

host_os  = dos
host_cpu = i86

target_cpu = x86

srv = tcp
which = SERVER
remote_link = 1

memory_model_i86 = l

extra_incs   = -I"$(wattcp_dir)/inc"
extra_cflags = -zm
extra_linker = op eliminate
extra_libs   = $(wattcp_dir)/dosi86/wattcpl.lib
extra_objs   = tcpifi.obj

!include $(trap_dir)/master.mif
