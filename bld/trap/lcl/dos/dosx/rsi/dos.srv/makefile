#pmake: build serv lcl os_dos dsx cpu_386 x86 rsi

host_os  = dos
host_cpu = i86

os   = rsi
srv  = rsi
cpu  = x86
name = rsihelp
which = SERVER

!include $(rsilib_dir)/client.mif

extra_objs = rsiacc.obj
extra_libs = $(rsilib_lib)

extra_c_flags = -DDOS4G -zu
inc_dirs = $(rsilib_inc_dirs)

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif

