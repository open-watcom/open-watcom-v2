#pmake: serv remote os_nov cpu_386 nov x86
host_CPU = 386
host_OS = nov

nlm_ver = 5
os = nlm
srv = tcp
cpu = x86
which = SERVER
nwlib = libc
type  = remote_link

extra_objs = $(nlm_objs)

!include $(trap_dir)/lcl/nlm/trap.mif
!include $(trap_dir)/master.mif

extra_c_flags += -d_USE_NEW_KERNEL
