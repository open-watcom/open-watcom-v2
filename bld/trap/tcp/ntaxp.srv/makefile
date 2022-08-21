#pmake: serv remote os_nt cpu_386 tcp axp

tree_depth = 4

host_cpu = axp
host_os  = nt

target_cpu = axp

srv = tcp
which = SERVER
gui = 1
remote_link = 1

extra_cppflags = -DGUISERVER

extra_linker = lib wsock32.lib

!include $(trap_dir)/master.mif
