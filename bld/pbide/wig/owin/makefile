op_sys=windows
os=windows
ccopt = -zW -ml
name=wig
compiler=wcc
objs = error.obj

!ifndef on_build_machine
ccopt += -dTRMEM -of+
objs += $(trmem_dir)\trmemcvr.obj $(trmem_dir)\trmem.obj
!endif

!include ..\mif\master.mif
