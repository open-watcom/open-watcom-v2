#pmake: build trap lcl dos dsx 32 x86

os = dos
srv = pls
cpu = x86

extra_objs = $(dosx_trap_objs)
extra_cflags = -dPHARLAP -dDOSXTRAP
extra_incs = $(trap_dir)\lcl\dos\h
extra_srcs = $(trap_dir)\lcl\dos\c;$(trap_dir)\lcl\dos\asm

!include $(trap_dir)\lcl\dos\dosx\dosx.mif
!include $(trap_dir)\trap.mif
