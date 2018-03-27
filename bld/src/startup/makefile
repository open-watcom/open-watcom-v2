#pmake: build

proj_name = src

src_objs = &
    msgcpyrt.inc &
    wildargv.c &
    cstrt086.asm &
    cstrto16.asm &
    cstrtw16.asm &
    dos16m.asm &
    maino16.c &
    cstrt386.asm &
    cstrtw32.asm &
    cstrtx32.asm &
    dllstart.asm &
    libentry.asm

!include cproj.mif

all: $(src_objs) .symbolic

msgcpyrt.inc : $(clib_dir)/startup/h/msgcpyrt.inc
    $(sed) -e "s/%_CYEAR/$(curr_year)/" $< >$@

wildargv.c : $(clib_dir)/startup/c/wildargv.c
    $(sed) -f ../srcfix1.sed $< >$@

cstrt086.asm : $(clib_dir)/startup/a/cstrt086.asm
    $(sed) -f ../srcfix2.sed -e "s/include exitwmsg.inc/extrn   \"C\",__fatal_runtime_error : proc/" $< >$@

cstrto16.asm : $(clib_dir)/startup/a/cstrto16.asm
    $(sed) -f ../srcfix2.sed $< >$@

cstrtw16.asm : $(clib_dir)/startup/a/cstrtw16.asm
    $(sed) -f ../srcfix2.sed $< >$@

dos16m.asm : $(clib_dir)/startup/a/dos16m.asm
    $(sed) -f ../srcfix2.sed -e "s/include exitwmsg.inc//" $< >$@
#    $(sed) -f ../srcfix2.sed -e "s/include exitwmsg.inc/extrn   \"C\",__fatal_runtime_error : proc/" $< >$@

maino16.c : $(clib_dir)/startup/c/maino16.c
    $(sed) -e "s/$#include \"exitwmsg.h\"/extern _WCRTLINK _WCNORETURN void __fatal_runtime_error( char __far *, unsigned );/" $< >$@

cstrt386.asm : $(clib_dir)/startup/a/cstrt386.asm
    $(sed) -f ../srcfix2.sed $< >$@

cstrtw32.asm : $(clib_dir)/startup/a/cstrtw32.asm
    $(sed) -f ../srcfix2.sed $< >$@

cstrtx32.asm : $(clib_dir)/startup/a/cstrtx32.asm
    $(sed) -f ../srcfix2.sed $< >$@

dllstart.asm : $(clib_dir)/startup/a/dllstart.asm
    $(sed) -f ../srcfix2.sed $< >$@

libentry.asm : $(clib_dir)/startup/a/libentry.asm
    $(sed) -f ../srcfix2.sed $< >$@

clean : .symbolic
    rm -f *.inc *.asm *.c *.tmp
