#pmake: serv remote os_nov cpu_386 nov x86
host_cpu = 386
host_os  = nov

target_cpu = x86
target_os  = nlm

nlm_ver = 5
srv = tcp
which = SERVER
nwlib = libc
remote_link = 1
builtin_trap = 1

extra_objs = $(nlm_objs)

!include $(trap_dir)/lcl/nlm/trap.mif
!include $(trap_dir)/master.mif
