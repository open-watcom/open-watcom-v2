op_sys=phar
os=dos
bind=1
name=wig
compiler=wcc386
objs = error.obj

!ifndef on_build_machine
ccopt += -dTRMEM -of+
objs += trmemcvr.obj trmem.obj
!endif

!include ..\mif\master.mif
