#pmake: serv remote os_os2 cpu_i86 tcp

host_os  = os2
host_cpu = i86

target_cpu = x86

os  = os2
srv = tcp
which = SERVER
remote_link = 1

extra_libs = $(watcom_dir)/lib/tcpipdll.lib
inc_dirs = -I"$(watcom_dir)/os220_h/tcpip"

!include $(trap_dir)/master.mif
