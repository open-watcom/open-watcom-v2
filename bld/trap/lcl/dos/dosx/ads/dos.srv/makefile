#pmake: build serv lcl dos dsx 32 x86

os = ads
srv = ads
cpu = x86
name = adshelp

extra_objs = $(dosx_serv_objs) adsacc.obj adsintr.obj x86cpu.obj
extra_cflags = -dACAD -dDOSXHELP
extra_aflags = -w2
extra_incs = $(trap_dir)\lcl\dos\h
extra_srcs = $(trap_dir)\lcl\dos\c
extra_linker = op nocaseexact

!include $(trap_dir)\lcl\dos\dosx\dosx.mif
!include $(trap_dir)\serv.mif
