op_sys=os2v2
os=os2
name=wig
compiler=wcc386
objs = error.obj

!ifndef on_build_machine
ccopt += -dTRMEM -of+
objs += $(trmem_dir)\trmemcvr.obj $(trmem_dir)\trmem.obj
!endif

!include ..\mif\master.mif
