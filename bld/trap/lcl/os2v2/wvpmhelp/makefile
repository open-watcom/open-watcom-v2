#pmake: build os_os2 cpu_386 lcl support

host_os  = os2
host_cpu = 386

#
# Builds wdpmhelp.exe, wdpmhook.dll
#
proj_name = wdpmhelp

!ifndef wdpmhelp_autodepends
wdpmhelp_autodepends = .AUTODEPEND
!endif

!include cproj.mif
!include defrule.mif
!include deftarg.mif

#
# cflags
#
.c: ../c

inc_dirs = -I. -I"../h" -I"$(trap_dir)/h"

extra_c_flags = -fpc

#
# lflags
#
extra_l_flags_os2 = &
    import WinLockInput PMMERGE.5225 &
    import WinThreadAssocQueue PMMERGE.5398 &
    option modname=wdpmhelp &
    option description 'Open Watcom Debugger PM Helper'

extra_l_flags_dll_os2 = library os2386.lib &
    export SendMsgHookProc.1,SetHmqDebugee.2

# explicit rules
#################

w d : wdpmhook.dll wdpmhelp.exe .SYMBOLIC
        @%null

#
# DLL
#
dlltarg_objs = pmhook.obj
dlltarg_name = wdpmhook
!include dlltarg.mif

#
# EXE
#
exetarg_prebuild_objs = _err.gh _jerr.gh
exetarg_objs = pmhelp.obj
exetarg_res = wdpmhelp.res
exetarg_name = wdpmhelp
!include exetarg.mif

#
# rc and others
#
rc_inc_dirs = -I"$(trap_dir)/h"
rcui_inc_dirs = -I"$(trap_dir)/h"

wdpmhelp.res: ../h/pmhelp.rc ../h/wdpmhelp.rh
    @%make echo_rc
    $(rc) $(rc_flags) $(rc_inc_path) -fo=$@ $[@

_err.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_ui_flags) $(rc_ui_inc_path) -o$@ $[@

_jerr.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_ui_flags) -DJAPANESE $(rc_ui_inc_path) -o$@ $[@
