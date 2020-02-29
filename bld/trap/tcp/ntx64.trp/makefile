#pmake: build trap remote os_nt cpu_i86 cpu_x64 tcp x86
host_cpu = x64
host_os  = nt

target_cpu = x86
target_os  = nt

srv = tcp
which = TRAP
remote_link = 1

bld_ldflags_ = -SUBSYSTEM:CONSOLE -DEFAULTLIB:wsock32.lib
bld_ldflags_ += -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3
sys_dll = 1
bld_ldflags_dll_ = -SUBSYSTEM:CONSOLE -DEFAULTLIB:wsock32.lib
bld_ldflags_dll_ += -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3

!include $(trap_dir)/master.mif
