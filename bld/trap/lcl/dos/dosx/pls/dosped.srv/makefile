#pmake: serv lcl os_dos dsx cpu_386 x86 pls
host_cpu = 386
host_os  = dos

!ifdef %OWUSE_FILENAME_VERSION
name = pedhel$(bld_ver)
!else
name = pedhelp
!endif

target_sys = tnt

extra_objs = plsord.obj   # must be first

extra_linker = &
        op offset=0, objalign=4k &
        alias __DOSseg__=main_ &
        lib $(plsdir)/wdos32.lib, $(plsdir)/wdosx32.lib &
        file $(plsdir)/dilseg.obj

!include $(trap_dir)/lcl/dos/dosx/pls/pls.mif
