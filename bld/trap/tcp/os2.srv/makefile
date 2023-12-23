#pmake: serv remote os_os2 cpu_i86 tcp

tree_depth = 4

host_os  = os2
host_cpu = i86

target_cpu = x86

srv = tcp
which = SERVER
remote_link = 1

inc_dirs   = -I"$(watcom_dir)/os220_h/tcpip"
extra_libs = $(watcom_dir)/lib/tcpipdll.lib
extra_objs = tcpifi.obj

!include $(trap_dir)/master.mif
