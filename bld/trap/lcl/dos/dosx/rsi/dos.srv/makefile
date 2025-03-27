#pmake: build serv lcl os_dos dsx cpu_386 x86 rsi

tree_depth = 7

host_os  = dos
host_cpu = i86

target_cpu = x86
target_os  = rsi

srv  = rsi
which = SERVER

!include $(rsilib_dir)/client.mif

extra_objs = rsiacc.obj
extra_incs = $(cli_rsilib_inc_dirs)
extra_libs = $(cli_rsilib_libs)

extra_c_flags = -zu

!include $(trap_dir)/lcl/dos/dosx/dosx.mif
!include $(trap_dir)/master.mif
