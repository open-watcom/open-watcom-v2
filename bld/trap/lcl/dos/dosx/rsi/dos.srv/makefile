#pmake: build serv lcl dos dsx 32 x86

os = rsi
srv = rsi
cpu = x86
name = rsihelp

extra_objs = &
        $(dosx_serv_objs) &
        dosredir.obj &
        squish87.obj &
        x86cpu.obj &
        npxtype.obj &
        rsiacc.obj &
        rsi87.obj &
        realmode.obj &
        dbglib.obj &
        dbgliba.obj &
        rmhdlr.obj &
        sbrk.obj

extra_cflags = -dDOS4G -dDOSXHELP -zdp -zu
extra_incs = $(trap_dir)\lcl\dos\h
extra_srcs = $(trap_dir)\lcl\dos\c;$(rsilib)

!define substitute_linker

!include $(trap_dir)\lcl\dos\dosx\dosx.mif
!include $(trap_dir)\serv.mif

objs = &
        $(%WATCOM)\lib286\dos\dos16m.obj &
        segs_16m.obj &
        magicstr.obj &
        $+$(objs)$-

lf = $(%ramdisk)\tmp.lnk
libnam  = d16libc

$(target) $(dtarget) : eraseall.obj $(objs) $(__MAKEFILES__)
    @%create $(lf)
    @%append $(lf) exp($^@) map($^&) log($^&.err)
    @%append $(lf) -mpl -nod -DPMI -map -acg -farcall -auto -quiet -rtnerr 2
    @for %i in ($(objs)) do @%append $(lf) %i
    @%append $(lf) $(rsilib)\$(libnam).lib $(lang_root)\lib286\dos\clibc.lib
    !$(tools_root)\glu @$(lf)

#until wasm gets fixed up

assemble = $(tools_root)\masm /mx /s /zd /I$(rsilib) /Dcompact $[@;

segs_16m.obj : segs_16m.asm
        $(assemble)

dbgliba.obj : dbgliba.asm
        $(assemble)

rmhdlr.obj : rmhdlr.asm
        $(assemble)
