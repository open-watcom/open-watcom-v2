#pmake: serv lcl os_dos dsx cpu_386 x86 pls

!ifdef %OWUSE_FILENAME_VERSION
name = pedhel$(bld_ver)
!else
name = pedhelp
!endif

sys_x86_pls = sys tnt op st=8K

objs_pls = plsord.obj   # must be first

extra_linker = &
        op offset=0, objalign=4k &
        alias __DOSseg__=main_ &
        lib $(plsdir)/wdos32.lib, $(plsdir)/wdosx32.lib &
        file $(plsdir)/dilseg.obj

!include ../dos.srv/makefile

plsord.obj : plsord.asm
        $(as) $[@ -fo=.obj
