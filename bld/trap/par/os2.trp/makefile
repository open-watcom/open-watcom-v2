#pmake: trap remote os_os2 cpu_i86 par

tree_depth = 4

host_cpu = i86
host_os  = os2

target_cpu = x86

srv = par
which = TRAP
remote_link = 1

extra_objs = paros2.obj portos2.obj

extra_c_flags_f_portos2 = -nt=port -nc=IOPL_CODE

extra_linker = segment 'port' IOPL

!include $(trap_dir)/master.mif
