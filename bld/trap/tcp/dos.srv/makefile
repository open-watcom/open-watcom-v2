#pmake: build serv remote os_dos cpu_i86 cpu_386 tcp x86

host_os  = dos
host_cpu = i86

memory_model_i86 = l

os  = dos
srv = tcp
cpu = x86
which = SERVER
type  = remote_link

inc_dirs     = -I"$(src_dir)/wattcp/inc"
extra_cflags = -zm
extra_linker = op eliminate
extra_libs   = $(src_dir)/wattcp/dosi86/wattcpwl.lib

!include $(trap_dir)/master.mif

