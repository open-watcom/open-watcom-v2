#pmake: build serv remote os_nov cpu_386 ser x86
host_cpu = 386
host_os  = nov

nlm_ver = 3
os = nlm
srv = ser
cpu = x86
which = SERVER
remote_link = 1
builtin_trap = 1

extra_objs = sernlm.obj busywait.obj

!include $(trap_dir)/lcl/nlm/trap.mif
!include $(trap_dir)/master.mif
