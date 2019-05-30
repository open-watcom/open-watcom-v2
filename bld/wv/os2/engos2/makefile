#pmake: build os_os2 cpu_386
#
# Open Watcom Debugger engine makefile for OS/2
#

host_os  = os2
host_cpu = 386

wv_engine_noui = 1

all : dve.exe .SYMBOLIC

!include ../../master.mif

dve.exe : $(resfile_dll) stubdb.obj wd.lib $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ sys os2v2 debug all op symf,map file stubdb.obj lib wd.lib
    @%make bind_res_dll
