#pmake: serv remote os_linux cpu_mips tcp mips monolithic

bootstrap = 1

host_os  = linux
host_cpu = mips

target_cpu = mps
target_os  = linux

srv   = tcp
which = SERVER
remote_link = 1
builtin_trap = 1

!include $(trap_dir)/lcl/linux/trap.mif
!include $(trap_dir)/master.mif
