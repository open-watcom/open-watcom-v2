#pmake: build serv remote os_nov cpu_386 ser x86
host_cpu = 386
host_os  = nov

target_cpu = x86
target_os  = nlm

nlm_ver = 4
srv = ser
which = SERVER
remote_link = 1
builtin_trap = 1

extra_objs = sernlm.obj busywait.obj

!include $(trap_dir)/lcl/nlm/trap.mif
!include $(trap_dir)/master.mif
