#pmake: build serv remote nlm 32 nov x86

!ifndef nlm_ver
nlm_ver = 3
!endif
os = nlm
srv = nov
cpu = x86

extra_objs = $(remote_objs) $(nlm_objs) novnlm.obj

!include $(trap_dir)\lcl\nlm\nlm.mif
!include $(trap_dir)\serv.mif
