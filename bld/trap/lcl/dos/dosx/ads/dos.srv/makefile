#pmake: serv lcl os_dos dsx cpu_386 x86

tree_depth = 7

host_cpu = 386
host_os  = dos

target_cpu = x86
target_os  = ads

srv = ads
which = SERVER

!include tools.mif

extra_objs = adsacc.obj adsintr.obj
extra_incs = -I"$(adsdir)"

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
