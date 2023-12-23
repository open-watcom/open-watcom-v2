#pmake: build trap remote os_nt cpu_i86 cpu_386 ser x64

tree_depth = 4

host_cpu = x64
host_os  = nt

sys_dll = 1

target_cpu = x86

srv = ser
which = TRAP
remote_link = 1

extra_objs = sernt.obj busywait.obj

bld_ldflags_ = -SUBSYSTEM:CONSOLE
bld_ldflags_ += -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3
bld_ldflags_dll_ = -SUBSYSTEM:CONSOLE
bld_ldflags_dll_ += -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3

!include $(trap_dir)/master.mif
