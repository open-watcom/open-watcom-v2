#pmake: build os_nt cpu_386

host_os  = nt
host_cpu = 386

wv_rtdll = 1

.c : ../../nt/c

all : wdb.exe .SYMBOLIC

!include ../../master.mif

wdb.exe : $(resfile_dll) wdbstub.obj wd.lib $(__MAKEFILES__)
    @%make echo_link
    $(linker) name $^@ sys nt debug all op symf,map file wdbstub.obj lib wd.lib
    @%make bind_res_dll
