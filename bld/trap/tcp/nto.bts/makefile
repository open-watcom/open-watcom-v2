#pmake: serv remote os_nto cpu_x86 tcp x86 monolithic

bootstrap = 1

host_os  = nto
host_cpu = 386

target_cpu = x86

os    = nto
srv   = tcp
which = SERVER
remote_link = 1
builtin_trap = 1

extra_ldflags = -lsocket

!include $(trap_dir)/lcl/nto/trap.mif
!include $(trap_dir)/master.mif
