#pmake: trap build remote os_linux cpu_i86 cpu_386 ser x86

tree_depth = 4

host_cpu = 386
host_os  = linux

target_cpu = x86

srv = ser
which = TRAP
remote_link = 1

extra_objs = serlnx.obj

!include $(trap_dir)/lcl/linux/linux.mif
!include $(trap_dir)/master.mif
