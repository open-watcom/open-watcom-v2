#pmake: build lcl os_win cpu_i86 cpu_386 support x86
host_cpu = i86
host_os  = win

proj_name = wint32

wint32_autodepends = .autodepend

sys_dll = 1

!include cproj.mif
!include defrule.mif
!include deftarg.mif

#
# cflags
#
.c : ../c

inc_dirs = -I"$(trap_dir)/h" -I"$(trap_dir)/common/win" -I"$(trap_dir)/lcl/win/h"

extra_c_flags = -fpc -zu -zw

#
# aflags
#
.asm : ../asm

#
# lflags
#
lflags_dll_win_i86 += initinstance memory

extra_l_flags_dll_win = &
    op heapsize=1k, rwr &
    export WEP.1 RESIDENT &
    segment CLASS 'CODE' PRELOAD FIXED &
    segment CLASS 'FAR_DATA' PRELOAD FIXED &
    segment CLASS 'BEGDATA' PRELOAD FIXED &
    segment CLASS 'DATA' PRELOAD FIXED &
    segment CLASS 'BSS' PRELOAD FIXED

#
#
#
dlltarg_objs = intmain.obj ints.obj int32.obj
!include dlltarg.mif
