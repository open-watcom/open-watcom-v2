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

#
# cflags
#
.c: ../c

inc_dirs = -I. -I"../h" -I"$(trap_dir)/h"

extra_c_flags = -bm -zu

#
# lflags
#
extra_l_flags = op stack=8k

# explicit rules
#################

!ifeq release 0
default_targ    = d
!else
default_targ    = w
!endif

first : $(default_targ)

w : nmpbind.exe .symbolic

d : dnmpbind.exe .symbolic

#
# EXE's
#
exetarg_prebuild_objs = _err.gh _jerr.gh
exetarg_objs = nmpbind.obj nmpos2.obj
exetarg_name = nmpbind
!include exetarg.mif

exetarg_prebuild_objs = _err.gh _jerr.gh
exetarg_objs = nmpbind.obj nmpos2.obj
exetarg_name = dnmpbind
!include exetarg.mif

#
# rc and others
#
rcui_inc_dirs = -I"$(trap_dir)/h"

_err.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_ui_flags) $(rc_ui_inc_path) -o$@ $[@

_jerr.gh : $(trap_dir)/h/trapmsg.h $(trap_dir)/h/trap.msg
    @%make echo_cpp
    $(cpp) $(rc_ui_flags) -DJAPANESE $(rc_ui_inc_path) -o$@ $[@
