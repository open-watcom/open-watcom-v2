#pmake: serv lcl os_dos dsx cpu_386 x86 rsi

host_os  = dos
host_cpu = i86

os   = rsi
srv  = rsi
cpu  = x86
name = rsihelp
which = SERVER

extra_objs = &
        rsiacc.obj &
        dbglib.obj &
        dbgliba.obj &
        rmhdlr.obj &
        sbrk.obj &
        segs_16m.obj &
        magicstr.obj

extra_c_flags        = -DDOS4G -zu -zdp
extra_c_flags_rsiacc = -3

extra_a_flags = -Dcompact -DDOS4G
extra_a_flags_dbgliba = -zcm

extra_srcs = $(rsilib);
inc_dirs = -I"$(rsilib)"

extra_libs = $(rsilib)/d16libc.lib

!ifndef rsi_wlink
!define substitute_linker
!endif

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif

!ifndef rsi_wlink

objs += $(lang_root)/lib286/dos/dos16m.obj

libs = $(lang_root)/lib286/dos/clibc.lib $(extra_libs)

$(target) : $(libs) $(objs) $(__MAKEFILES__)
    @%create tmp.lnk
    @%append tmp.lnk exp($^@) map($^&) log($^&.err)
!ifeq release 0
    @%append tmp.lnk -codeview
!endif
    @%append tmp.lnk -mpl -nod -DPMI -map -acg -farcall -auto -quiet -rtnerr 2
    @for %i in ($(objs)) do @%append tmp.lnk %i
    @for %i in ($(libs)) do @%append tmp.lnk %i
    -!$(tools_root)\glu @tmp.lnk

!endif
