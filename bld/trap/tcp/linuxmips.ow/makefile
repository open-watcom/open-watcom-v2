#pmake: serv remote os_linux cpu_mips tcp mips monolithic

host_os  = linux
host_cpu = mps

os    = linux
srv   = tcp
cpu   = mips
which = SERVER
remote_link = 1
builtin_trap = 1


!define substitute_linker

!include $(trap_dir)/lcl/linux/trap.mif
!include $(trap_dir)/master.mif

$(target) : $(libs) $(objs) $(__MAKEFILES__)
    $(cl) $(clflags) $(objs) $(cl_libs)
