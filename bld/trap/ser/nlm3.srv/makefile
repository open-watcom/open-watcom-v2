#pmake: build serv remote nlm 32 ser x86

!ifndef nlm_ver
nlm_ver = 3
!endif
os = nlm
srv = ser
cpu = x86

extra_objs = $(remote_objs) $(nlm_objs) sernlm.obj busywait.obj

!include $(trap_dir)\lcl\nlm\nlm.mif
!include $(trap_dir)\serv.mif
