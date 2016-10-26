#pmake: build os_nt cpu_386

host_os  = nt
host_cpu = 386

dll = 1
extra_c_flags= -br

.c : ../../win/c

all : wdb.exe .SYMBOLIC

!include ../../master.mif

wdb.exe : $(resfile) wdbstub.obj wd.lib $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ sys nt debug all op symf,map file wdbstub.obj lib wd.lib
    @%make echo_bindres
    $(rc) -k $(resfile) wdb.exe
