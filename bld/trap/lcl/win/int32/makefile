#pmake: build lcl os_win cpu_i86 cpu_386 support x86
host_cpu = i86
host_os  = win

proj_name = wint32

wint32_autodepends = .autodepend

sys_dll = 1

!include cproj.mif
!include defrule.mif
!include deftarg.mif

objs = intmain.obj ints.obj int32.obj

extra_c_flags = -s -fpc -zu -zw -I"$(trap_dir)/h" -I"$(trap_dir)/common/win" -I"$(trap_dir)/lcl/win/h"

lflags_dll_win_i86 += initinstance memory

extra_l_flags_dll_win = op heapsize=1k, rwr
extra_l_flags_dll_win += export WEP.1 RESIDENT
extra_l_flags_dll_win += segment CLASS 'CODE' PRELOAD FIXED
extra_l_flags_dll_win += segment CLASS 'FAR_DATA' PRELOAD FIXED
extra_l_flags_dll_win += segment CLASS 'BEGDATA' PRELOAD FIXED
extra_l_flags_dll_win += segment CLASS 'DATA' PRELOAD FIXED
extra_l_flags_dll_win += segment CLASS 'BSS' PRELOAD FIXED

.c : ../c

.asm : ../asm

wint32.dll: $(objs) $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ $(lflags) file {$(objs)}
