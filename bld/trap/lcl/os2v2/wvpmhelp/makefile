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

extra_c_flags = -s -fpc

inc_dirs = -I"$(trap_dir)/h"

w d : wdpmhook.dll wdpmhelp.exe .SYMBOLIC
        @%null

tmplnk = tmp.lnk

wdpmhook.dll: pmhook.obj
    @%write  $(tmplnk) name $^@ sys os2v2 dll
    @%append $(tmplnk) debug all op symf
    @%append $(tmplnk) option map
    @%append $(tmplnk) file pmhook.obj
    @%append $(tmplnk) library os2386.lib
    @%append $(tmplnk) export SendMsgHookProc.1,SetHmqDebugee.2
    $(linker) @$(tmplnk)

wdpmhelp.exe: _err.gh _jerr.gh pmhelp.obj pmhelp.res
    @%write  $(tmplnk) name $^@ system os2v2 pm
    @%append $(tmplnk) debug all op symf
    @%append $(tmplnk) option map
    @%append $(tmplnk) import WinLockInput PMMERGE.5225
    @%append $(tmplnk) import WinThreadAssocQueue PMMERGE.5398
    @%append $(tmplnk) file pmhelp.obj
    @%append $(tmplnk) option modname=wdpmhelp
    @%append $(tmplnk) option description $(description)
    $(linker) @$(tmplnk)
    $(rc) pmhelp.res $^@

.c: ../c

pmhelp.res: ../h/pmhelp.rc ../h/wdpmhelp.h
        $(rc) $(rc_flags) $(inc_dirs_sys_os2) -fo=./pmhelp.res $[@

_err.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
        $(cpp) $(rc_aui_flags) -I"$(trap_dir)/h" $[@ >$@

_jerr.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
        $(cpp) $(rc_aui_flags) -DJAPANESE -I"$(trap_dir)/h" $[@ >$@
