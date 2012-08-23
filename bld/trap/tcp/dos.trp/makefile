#pmake: trap remote os_dos cpu_i86 cpu_386 tcp x86
host_CPU = i86
host_OS = dos

memory_model_i86 = l

# NOTE: This is not working at present...

os = dos
srv = tcp
cpu = x86
which = TRAP
type  = remote_link

inc_dirs = -I"$(wattcp_dir)/inc"
extra_libs = $(wattcp_dir)/dosi86/wattcpwl.lib

extra_a_flags = -D__LARGE__

!include $(trap_dir)/master.mif
