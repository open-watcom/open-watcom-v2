#pmake: serv remote os_nto cpu_x86 tcp x86 monolithic

bootstrap = 1

host_os  = nto
host_cpu = 386

os    = nto
srv   = tcp
cpu   = x86
which = SERVER
remote_link = 1
builtin_trap = 1


extra_objs = clibext.obj
cl_libs = -lsocket

!define substitute_linker

!include $(trap_dir)/lcl/nto/trap.mif
!include $(trap_dir)/master.mif

$(target) : $(libs) $(objs) $(__MAKEFILES__)
    $(cl) $(clflags) $(objs) $(cl_libs)
