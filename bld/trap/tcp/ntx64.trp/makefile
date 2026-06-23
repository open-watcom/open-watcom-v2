#pmake: build trap remote os_nt cpu_i86 cpu_x64 tcp x86

tree_depth = 4

host_cpu = x64
host_os  = nt

sys_dll = 1

target_cpu = x86

srv = tcp
which = TRAP
remote_link = 1

extra_ldflags = -SUBSYSTEM:CONSOLE -DEFAULTLIB:wsock32.lib -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3
extra_ldflags_dll = -SUBSYSTEM:CONSOLE -DEFAULTLIB:wsock32.lib -EXPORT:TrapInit,@1 -EXPORT:TrapFini,@2 -EXPORT:TrapRequest,@3

!include $(trap_dir)/master.mif
