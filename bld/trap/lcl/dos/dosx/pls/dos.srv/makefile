#pmake: build serv lcl dos dsx 32 x86

#NOTE: This file is also included from the ..\DOSPED.SRV makefile

os = pls
srv = pls
cpu = x86
!ifndef name
name = plshelp
!endif

extra_objs = $(dosx_serv_objs) plsacc.obj plsintr.obj x86cpu.obj dilglue.obj
extra_cflags = -fi=wcc.h -dDOSXHELP $(compflags_$[&)
extra_aflags = -w2
extra_incs = $(trap_dir)\lcl\dos\h;..\h
extra_srcs = $(trap_dir)\lcl\dos\c;$(plsdir)

compflags_dilglue = -3s

!ifndef extra_linker
extra_linker = &
        op maxdata=0 runtime maxreal=1023K,maxibuf=4K,flag=2 &
        lib $(plsdir)\wdos32,$(plsdir)\wdosx32 &
        file $(plsdir)\dilseg
!endif

!include $(trap_dir)\lcl\dos\dosx\dosx.mif
!include $(trap_dir)\serv.mif
