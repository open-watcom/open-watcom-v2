#pmake: build serv remote nlm 32 par x86

!ifndef nlm_ver
nlm_ver = 3
!endif
os = nlm
srv = par
cpu = x86

extra_objs = $(remote_objs) $(nlm_objs) parnlm.obj

!include $(trap_dir)\lcl\nlm\nlm.mif
!include $(trap_dir)\serv.mif
