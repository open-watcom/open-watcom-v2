#pmake: build lcl dos 16 trap x86

os      = dos
srv     = lcl
cpu     = x86

std_objs = &
        $(dos_objs) &
        squish87.obj &
        dosovl.obj &
        dosacc.obj &
        dbgtrap.obj &
        npxtype.obj &
        x86cpu.obj

extra_objs  = $(std_objs) realtrap.obj
extra_dobjs = $(std_objs) dumtrap.obj
extra_incs = $(trap_dir)\lcl\dos\h

!include $(trap_dir)\lcl\dos\dos.mif
!include $(trap_dir)\trap.mif

realtrap.obj : ..\asm\trap.asm

dumtrap.obj : ..\asm\trap.asm
