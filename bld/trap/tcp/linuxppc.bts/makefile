#pmake: serv remote os_linux cpu_ppc tcp ppc monolithic

tree_depth = 4

bootstrap = 1

host_os  = linux
host_cpu = ppc

target_cpu = ppc

srv   = tcp
which = SERVER
remote_link = 1
builtin_trap = 1

!include $(trap_dir)/lcl/linux/linux.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
