#pmake: serv lcl os_dos dsx cpu_386 x86 pls

tree_depth = 7

host_cpu = 386
host_os  = dos

target_cpu = x86
target_os  = pls

srv = pls
which = SERVER

trap_nodoslfn = 1

!include tools.mif

extra_objs = $(pls_objs)

extra_linker = &
        op maxdata=0 runtime maxreal=1023K,maxibuf=4K,flag=2 &
        lib $(plsdir)/wdos32.lib,$(plsdir)/wdosx32.lib &
        file $(plsdir)/dilseg.obj

!include $(trap_dir)/lcl/dos/dosx/pls/pls.mif
!include $(trap_dir)/master.mif
