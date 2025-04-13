#pmake: build serv remote os_nov cpu_386 par x86

tree_depth = 4

host_cpu = 386
host_os  = nov

target_cpu = x86
target_os  = nlm4

srv = par
which = SERVER
remote_link = 1
builtin_trap = 1

extra_objs = parnlm.obj $(nlm_trap_objs)

!include $(trap_dir)/lcl/nlm/nlm.mif
!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif
