#pmake: build serv remote os_nov cpu_386 par x86
host_CPU = 386
host_OS = nov

!ifndef nlm_ver
nlm_ver = 3
!endif
os = nlm
srv = par
cpu = x86
which = SERVER
type  = remote_link

extra_objs = parnlm.obj

!include $(trap_dir)/lcl/nlm/trap.mif
!include $(trap_dir)/master.mif
