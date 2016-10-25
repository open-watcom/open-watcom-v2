#pmake: build os_nt cpu_386
#
# WATCOM Debugger makefile for NT (real GUI version)
#

host_os=nt
host_cpu=386

dll = 1
extra_c_flags= -br

all : dve.exe .SYMBOLIC

!include ../../master.mif

dve.exe : $(resfile_dll) stubdb.obj wd.lib $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ sys nt debug all op symf,map file stubdb.obj lib wd.lib
    @%make bind_res_dll
