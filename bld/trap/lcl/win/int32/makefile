#pmake: build lcl os_win cpu_i86 cpu_386 support x86
host_cpu = i86
host_os = win

NAME = wint32

memory_model_i86 = c

sys_dll = 1

objs =  intmain.obj ints.obj int32.obj

!include cproj.mif
!include deftarg.mif
!include defrule.mif

extra_c_flags = -s -fpc -zu -zw -3

lflags_win_i86 += initinstance memory

extra_l_flags = option map lib clibc file {$(objs)}
extra_l_flags_win = op heapsize=1k, rwr
extra_l_flags_win += export WEP.1 RESIDENT
extra_l_flags_win += segment CLASS 'CODE' PRELOAD FIXED
extra_l_flags_win += segment CLASS 'FAR_DATA' PRELOAD FIXED
extra_l_flags_win += segment CLASS 'BEGDATA' PRELOAD FIXED
extra_l_flags_win += segment CLASS 'DATA' PRELOAD FIXED
extra_l_flags_win += segment CLASS 'BSS' PRELOAD FIXED

w : .symbolic $(name).dll
        @%null

$(name).dll: $(objs)
       $(linker) $(lflags) name $^@

.c : ../c

.asm : ../asm
