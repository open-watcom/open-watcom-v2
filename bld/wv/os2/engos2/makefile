#pmake: build os_os2 cpu_386
#
# Open Watcom Debugger engine makefile for OS/2
#

host_os  = os2
host_cpu = 386

dll = 1
extra_c_flags = -br

all : dve.exe .SYMBOLIC

!include ../../master.mif

dve.exe : $(resfile) stubdb.obj wd.lib $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ sys os2v2 debug all op symf,map file stubdb.obj lib wd.lib
    @%make echo_bindres
    $(rc) $(resfile) $@
