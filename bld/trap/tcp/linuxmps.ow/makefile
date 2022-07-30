#pmake: serv remote os_linux cpu_mips tcp mips monolithic

tree_depth = 4

host_os  = linux
host_cpu = mps

target_cpu = mps

srv   = tcp
which = SERVER
remote_link = 1
builtin_trap = 1

!include $(trap_dir)/lcl/linux/linux.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
