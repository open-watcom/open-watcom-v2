#pmake: build trap remote os_nt cpu_i86 cpu_x64 tcp x86
host_cpu = x64
host_os  = nt

target_cpu = x86
target_os  = nt

srv = tcp
which = TRAP
remote_link = 1

bld_ldflags_ = -ENTRY:InfoFunction -SUBSYSTEM:CONSOLE -DEFAULTLIB:wsock32.lib -DEFAULTLIB:libucrt.lib
bld_ldflags_ += -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3 -EXPORT:InfoFunction,@4
#sys_dll = 1
#bld_ldflags_dll_ = -ENTRY:InfoFunction
bld_ldflags_dll_ = -ENTRY:InfoFunction -SUBSYSTEM:CONSOLE -DEFAULTLIB:wsock32.lib -DEFAULTLIB:libucrt.lib
bld_ldflags_dll_ += -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3 -EXPORT:InfoFunction,@4

!include $(trap_dir)/master.mif
