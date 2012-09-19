#pmake: build serv lcl os_dos dsx cpu_386 x86 rsi

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

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
