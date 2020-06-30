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

description = 'Open Watcom Debugger PM Helper'

extra_c_flags = -fpc

inc_dirs = -I. -I"../h" -I"$(trap_dir)/h"

w d : wdpmhook.dll wdpmhelp.exe .SYMBOLIC
        @%null

tmplnk = tmp.lnk

wdpmhook.dll: pmhook.obj $(__MAKEFILES__)
    @%write  $^&.lnk name $@ sys os2v2 dll
    @%append $^&.lnk debug all op symf
    @%append $^&.lnk option map
    @%append $^&.lnk file pmhook.obj
    @%append $^&.lnk library os2386.lib
    @%append $^&.lnk export SendMsgHookProc.1,SetHmqDebugee.2
    @%make echo_link
    $(linker) @$^&.lnk

wdpmhelp.exe: _err.gh _jerr.gh pmhelp.obj wdpmhelp.res $(__MAKEFILES__)
    @%write  $^&.lnk name $@ system os2v2 pm
    @%append $^&.lnk debug all op symf
    @%append $^&.lnk option map
    @%append $^&.lnk import WinLockInput PMMERGE.5225
    @%append $^&.lnk import WinThreadAssocQueue PMMERGE.5398
    @%append $^&.lnk file pmhelp.obj
    @%append $^&.lnk option modname=wdpmhelp
    @%append $^&.lnk option description $(description)
    @%make echo_link
    $(linker) @$^&.lnk
    @%make bind_res

.c: ../c

wdpmhelp.res: ../h/pmhelp.rc ../h/wdpmhelp.rh
    @%make echo_rc
    $(rc) $(rc_flags) $(inc_dirs_sys_os2) -fo=$@ $[@

_err.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_aui_flags) -I"$(trap_dir)/h" -o$@ $[@

_jerr.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_aui_flags) -DJAPANESE -I"$(trap_dir)/h" -o$@ $[@
