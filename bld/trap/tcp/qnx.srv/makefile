#pmake: serv remote os_qnx cpu_i86 cpu_386 tcp x86

host_os  = qnx
host_cpu = 386

os    = qnx
srv   = tcp
cpu   = x86
which = SERVER
remote_link = 1

extra_libs = $(watcom_dir)/lib/qnx/socket3r.lib
inc_dirs   = -I"$(watcom_dir)/qnx_h"

!include $(trap_dir)/master.mif
