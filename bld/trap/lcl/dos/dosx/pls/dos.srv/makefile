#pmake: serv lcl os_dos dsx cpu_386 x86 pls
host_cpu = 386
host_os  = dos

target_cpu = x86
target_os  = pls

#NOTE: This file is also included from the ..\DOSPED.SRV makefile

srv = pls
!ifndef name
name = plshelp
!endif
which = SERVER

pharlap_c_flags = -DWATCOM=7 -DCMPLR=7 -D__STKCALL=__cdecl -D_far=__far -D_near=__near -D_Far=__far -D_Near=__Near -I"$(plsdir)"

extra_cpp_flags = -DPHARLAP
extra_c_flags = $(compflags_$[&)

extra_c_flags_dilglue = -3s -zl $(pharlap_c_flags)
extra_c_flags_dosxlink = $(pharlap_c_flags)
extra_c_flags_plsacc = $(pharlap_c_flags)

extra_a_flags = -dPHARLAP

inc_dirs = -I"../h"

extra_objs = &
    plsacc.obj &
    plsintr.obj &
    dilglue.obj

extra_srcs = $(plsdir)

!ifndef extra_linker
extra_linker = &
        op maxdata=0 runtime maxreal=1023K,maxibuf=4K,flag=2 &
        lib $(plsdir)/wdos32.lib,$(plsdir)/wdosx32.lib &
        file $(plsdir)/dilseg.obj
!endif

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
