#pmake: build serv remote os_os2 cpu_i86 cpu_386 par x86

tree_depth = 4

host_cpu = 386
host_os  = os2

target_cpu = x86

srv = par
which = SERVER
remote_link = 1

extra_objs = paros2.obj

extra_linker = &
    imp OUTPUT_PORT wdio.OUTPORT &
    imp INPUT_PORT wdio.INPORT &
    imp DOSPORTACCESS DOSCALLS.69

!include $(trap_dir)/master.mif
