#pmake: os_nt cpu_386

host_os  = nt
host_cpu = 386

sys_windowed = 1

dll = 1
extra_c_flags= -br

.c : ../../win/c

all : wdb.exe .SYMBOLIC

sysobjs = ntstub.obj ntfilio.obj winlkup.obj winchk.obj ntcmd.obj ntfinfo.obj

!include ../../master.mif

wdb.exe : $(resfile) wdbstub.obj wd.lib $(__MAKEFILES__)
        $(linker) sys nt name $^@ debug all op symf,map file wdbstub.obj lib wd.lib
        $(rc) -k $(resfile) wdb.exe
