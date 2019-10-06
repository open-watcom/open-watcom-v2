#pmake: build remote support nmp os_os2 cpu_i86 cpu_386

host_os  = os2
host_cpu = i86

proj_name = nmpbind

!ifndef nmpbind_autodepends
nmpbind_autodepends = .AUTODEPEND
!endif

#memory_model_i86 = s

!include cproj.mif
!include defrule.mif
!include deftarg.mif

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

nmpbind.exe : _err.gh _jerr.gh $(objs) $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ $(lflags) file { $objs }

dnmpbind.exe : _err.gh _jerr.gh $(objs) $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ $(lflags) file { $objs }

.c: ../c

inc_dirs = -I. -I"../h" -I"$(trap_dir)/h"

_err.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_aui_flags) -I"$(trap_dir)/h" -o$@ $[@

_jerr.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_aui_flags) -DJAPANESE -I"$(trap_dir)/h" -o$@ $[@
