#pmake: serv lcl os_dos dsx cpu_386 x86 pls

tree_depth = 7

host_cpu = 386
host_os  = dos

target_cpu = x86
target_os  = tnt

srv = ped
which = SERVER

trap_nodoslfn = 1

!include tools.mif

extra_objs = plsord.obj $(pls_objs)

extra_linker = &
        op offset=0, objalign=4k &
        alias __DOSseg__=main_ &
        lib $(plsdir)/wdos32.lib, $(plsdir)/wdosx32.lib &
        file $(plsdir)/dilseg.obj

!include $(trap_dir)/lcl/dos/dosx/pls/pls.mif
!include $(trap_dir)/master.mif
