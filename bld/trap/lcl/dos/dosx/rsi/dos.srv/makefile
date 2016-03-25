#pmake: build serv lcl os_dos dsx cpu_386 x86 rsi

host_os  = dos
host_cpu = i86

target_cpu = x86
target_os  = rsi

srv  = rsi
name = rsihelp
which = SERVER

!include $(rsilib_dir)/client.mif

extra_objs = rsiacc.obj
extra_libs = $(rsilib_lib)

extra_cpp_flags = -DDOS4G
extra_c_flags = -zu
inc_dirs = $(rsilib_inc_dirs)

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
