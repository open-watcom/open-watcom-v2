#pmake: os_os2 lcl cpu_386 support
#
# Create WDPMHELP.EXE, WDPMHOOK.DLL
#
host_os  = os2
host_cpu = i86

proj_name = wvpmhelp

!ifndef wvpmhelp_autodepends
wvpmhelp_autodepends = .AUTODEPEND
!endif

!include cproj.mif
!include defrule.mif
!include deftarg.mif

#
# cflags
#
.c: ../c

inc_dirs = -I. -I"../h" -I"$(trap_dir)/h" -I"$(os2api_dir)/os2286/h" -I"$(os2api_dir)/os2386/h"

extra_c_flags = -zu -fpc

extra_c_flags_pmhook = -mc -oais
extra_c_flags_pmhelp = -ml

#
# aflags
#
.asm: ../asm

extra_a_flags = -2 -bt=os2 -fo=.obj -mc

#
# lflags
#
extra_l_flags_dll_os2 = &
    export SENDMSGHOOKPROC.1,SETHMQDEBUGEE.2 &
    segment class 'DATA' shared &
    segment class 'BSS' shared

extra_l_flags_os2 = &
    library clibl.lib &
    op alignment=16, nodefaultlibs, modname=wdpmhelp &
    op description 'OS/2 Presentation Manager Sample' &
    op manyautodata, stack=4096, heapsize=1024, protmode

w d : wdpmhook.dll wdpmhelp.exe .SYMBOLIC
        @%null

#
# DLL
#
dlltarg_objs = pmhook.obj loadstd.obj
dlltarg_name = wdpmhook
!include dlltarg.mif

#
# EXE
#
exetarg_prebuild_objs = _err.gh
exetarg_objs = pmhelp.obj
exetarg_res = wdpmhelp.res
exetarg_name = wdpmhelp
!include exetarg.mif

#
# rc and others
#
wdpmhelp.res: ../h/pmhelp.rc ../h/wdpmhelp.rh
    @%make echo_rc
    $(rc) $(rc_flags) $(inc_path) $[@ -fo=$@

_err.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_ui_flags) -I"$(trap_dir)/h" -o$@ $[@
