#pmake: build remote support nmp os_os2 cpu_i86 cpu_386

host_os  = os2
host_cpu = i86

proj_name = nmpbind

!ifndef nmpbind_autodepends
nmpbind_autodepends = .AUTODEPEND
!endif

!include cproj.mif
!include deftarg.mif
!include defrule.mif

objs = nmpbind.obj nmpos2.obj

extra_c_flags = -bm -zu
extra_l_flags = op stack=8k

!ifeq release 0
default_targ    = d
!else
default_targ    = w
!endif

first : $(default_targ)

w : nmpbind.exe .symbolic

d : dnmpbind.exe .symbolic

nmpbind.exe : _err.gh _jerr.gh $(objs)
    $(linker) $(lflags) file { $objs } name $^@

dnmpbind.exe : _err.gh _jerr.gh $(objs)
    $(linker) $(lflags) file { $objs } name $^@

.c: ../c

inc_dirs = -I. -I"../h" -I"$(trap_dir)/h"

_err.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
        $(cpp) $(rc_aui_flags) -I"$(trap_dir)/h" $[@ >$@

_jerr.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
        $(cpp) $(rc_aui_flags) -DJAPANESE -I"$(trap_dir)/h" $[@ >$@
