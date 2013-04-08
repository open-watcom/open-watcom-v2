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
!include deftarg.mif
!include defrule.mif

w d : wdpmhook.dll wdpmhelp.exe .SYMBOLIC
        @%null

wdpmhook.dll: pmhook.obj loadstd.obj
    @%write  $^&.lnk name $^@ sys os2 dll initglobal
    @%append $^&.lnk debug all op symfile
    @%append $^&.lnk option dosseg,quiet
    @%append $^&.lnk option map
    @%append $^&.lnk file loadstd.obj,pmhook.obj
    @%append $^&.lnk export SENDMSGHOOKPROC.1,SETHMQDEBUGEE.2
    @%append $^&.lnk segment class 'DATA' shared
    @%append $^&.lnk segment class 'BSS' shared
    $(linker) @$^&.lnk

wdpmhelp.exe: pmhelp.res _err.gh pmhelp.obj
    @%write  $^&.lnk name $^@ system os2 pm
    @%append $^&.lnk debug all op symfile
    @%append $^&.lnk file pmhelp.obj
    @%append $^&.lnk library clibl.lib
    @%append $^&.lnk option alignment=16
    @%append $^&.lnk option nodefaultlibs,quiet
    @%append $^&.lnk option modname=wdpmhelp
    @%append $^&.lnk option description 'OS/2 Presentation Manager Sample'
    @%append $^&.lnk option manyautodata
    @%append $^&.lnk option stack=4096
    @%append $^&.lnk option heapsize=1024
    @%append $^&.lnk option protmode
    $(linker) @$^&.lnk
    $(rc) pmhelp.res wdpmhelp.exe

.c: ../c
.asm: ../asm

extra_c_flags = -s -zu -fpc

extra_c_flags_pmhook = -mc -oais
extra_c_flags_pmhelp = -ml

inc_dirs = -i"$(watcom_dir)/h" -I"$(os2api_dir)/os2286/h" -I"$(os2api_dir)/os2386/h" -I"$(trap_dir)/h"

extra_a_flags = -2 -bt=os2 -fo=.obj -mc

pmhelp.res: ../h/pmhelp.rc ../h/wdpmhelp.h
    $(rc) $(rc_flags) $(inc_path) $[@ -fo=$@

_err.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    $(cpp) $(rc_aui_flags) -I"$(trap_dir)/h" $[@ >$@
