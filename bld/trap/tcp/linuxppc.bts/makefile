#pmake: serv remote os_linux cpu_ppc tcp ppc monolithic

bootstrap = 1

host_os  = linux
host_cpu = ppc

os    = linux
srv   = tcp
cpu   = ppc
which = SERVER
type  = remote_link

builtin_trap = 1

extra_objs = clibext.obj

!define substitute_linker

!include $(trap_dir)/lcl/linux/trap.mif
!include $(trap_dir)/master.mif

$(target) : $(libs) $(objs) $(__MAKEFILES__)
    $(cl) $(clflags) $(objs) $(cl_libs)
