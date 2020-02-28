#pmake: build trap remote os_nt cpu_i86 cpu_386 ser x64
host_cpu = x64
host_os  = nt

target_cpu = x86
target_os  = nt

srv = ser
which = TRAP
remote_link = 1

extra_objs = sernt.obj busywait.obj

bld_ldflags_ = -SUBSYSTEM:CONSOLE
bld_ldflags_ += -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3
sys_dll = 1
bld_ldflags_dll_ = -SUBSYSTEM:CONSOLE
bld_ldflags_dll_ += -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3

!include $(trap_dir)/master.mif
