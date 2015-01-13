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

dve.exe : $(resfile) stubdb.obj wd.lib $(__MAKEFILES__)
        $(linker) name $^@ sys nt debug all op symf,map file stubdb.obj lib wd.lib
        $(rc) -k $(resfile) dve.exe
