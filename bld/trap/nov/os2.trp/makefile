#pmake: trap remote os_os2 cpu_i86 nov
host_CPU = i86
host_OS = os2

os  = os2
srv = nov
cpu = x86
which = TRAP
type  = remote_link

extra_objs = novos2.obj
inc_dirs = -I$(nwos2dir)
extra_libs = $(nwos2dir)/ipxcalls.lib $(nwos2dir)/nwcallsp.lib $(nwos2dir)/spxcalls.lib

!include $(trap_dir)/master.mif
